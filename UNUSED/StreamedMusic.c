#include <nds.h>
#include "game.h"
#include "NSMB_Timers.h"
#include "OSPanic.h"
#include "OSReport.h"
#include "VariousFunctions.h"
#include "auto_consts_files.h"
#include "auto_consts_sound.h"
#include "StreamedMusic.h"

#define DS_REFRESH_RATE 59.737

extern bool PakRamIn;
u8* StreamPlayerPcmDst = 0;
u16 StreamPlayerSampleRate = 0;
int StreamPlayerPcmSrcOffset100 = 0;
int StreamPlayerPcmDstOffset100 = 0;
int StreamPlayerLoopOffset100 = 0;
int StreamPlayerTotalLength100 = 0;
int StreamPlayerFrameNum = -1;
bool StreamPlayerZeroFill = false;
bool StreamPlayerIsLooped = false;
bool StreamPlayerIsPlaying = false;

bool StreamPlayerFileOpen = false;
FSFile StreamPlayerFile;

#define STREAM_PLAYER_STACK_SIZE 192
u32 StreamPlayerStack[STREAM_PLAYER_STACK_SIZE];
OSThread StreamPlayerThread;
bool StreamPlayerUsingThread = false;

u8* StreamPlayerThreadTemp;
u32 StreamPlayerSavedStreamOffset;

#define STREAM_PLAYER_MEAN_COUNTER_FRAMES 120
int* StreamPlayerMeanCounterArray = NULL;
int StreamPlayerMeanCounterTimer;
int StreamPlayerMeanCounterLength;
int StreamPlayerMeanTimerTicks;


// GBA Expansion Pak / DSi-specific
extern vu16* ExtRAMAddr;
u8* StreamPlayerStarmanThemeAddr = NULL;
u8* StreamPlayerMegaMarioThemeAddr = NULL;
u8* StreamPlayerDeathThemeAddr = NULL;
u8* StreamPlayerCourseClearThemeAddr = NULL;
u8* StreamPlayerBossClearThemeAddr = NULL;
u8* StreamPlayerSwitchThemeAddr = NULL;
u8* StreamPlayerCustomThemeAddr = NULL;
u8* StreamPlayerPcmSrc = 0;
u8* StreamPlayerStrmAddr = 0;
int StreamPlayerMusicIDToLoad = -1;


// =============================== GENERAL ===============================


int StreamPlayer_GetTimerValueAndReset()
{
    int timeElapsed = NSMB_TimerTick(2);
    timeElapsed |=  NSMB_TimerTick(3) << 16;
    NSMB_TimerStop(2);
    NSMB_TimerStop(3);
    NSMB_TimerStart(2, ClockDivider_1, 0, 0);
    NSMB_TimerStart(3, ClockDivider_1, 0, TIMER_CASCADE);
    return timeElapsed;
}


void StreamPlayer_Prepare(int musicID)
{
    StreamPlayerFrameNum = -1;
    StreamPlayerZeroFill = false;

    u8* SwavPtr = GetPtrToSwavDataInSdat(GetCurrentlyPlayingFirstSwarID(musicID), 0);
    if (SwavPtr == NULL) OS_Panic_With_Reason("StreamPlayer_Prepare(): swav ptr is null");
    StreamPlayerPcmDst = SwavPtr + 0xC;

    StreamPlayerPcmDstOffset100 = 0;
    StreamPlayerPcmSrcOffset100 = STREAM_BUFFER_SIZE * 0x100;

    StreamPlayer_GetTimerValueAndReset();

    if (PakRamIn)
        StreamPlayer_Prepare_RAM(musicID);
    else
        StreamPlayer_Prepare_ROM(musicID);
}

static inline int StreamPlayer_GetAmountToCopy()
{
    float timeElapsed = StreamPlayer_GetTimerValueAndReset();
    float bytesPerFrame = StreamPlayerSampleRate / DS_REFRESH_RATE;

    // Return value is in units of 1/256 of a byte
    return timeElapsed * (bytesPerFrame / StreamPlayerMeanTimerTicks) * 0x100;
}


static inline void StreamPlayer_CopyToBufferOrClear(int srcOffset, int dstOffset, int amount)
{
    // If this copy will cause us to overflow to the next byte, copy one more byte
    if ((srcOffset % 0x100) + (amount % 0x100) >= 0x100)
        amount += 0x100;

    srcOffset /= 0x100;
    dstOffset /= 0x100;
    amount /= 0x100;

    if (StreamPlayerZeroFill)
        CpuClear8(StreamPlayerPcmDst + dstOffset, amount);
    else if (PakRamIn)
        MI_CpuCopy8(StreamPlayerPcmSrc + srcOffset, StreamPlayerPcmDst + dstOffset, amount);
    else
    {
        if (StreamPlayerUsingThread)
        {
            MI_CpuCopy8(StreamPlayerThreadTemp + StreamPlayerSavedStreamOffset, StreamPlayerPcmDst + dstOffset, amount);
            StreamPlayerSavedStreamOffset += amount;
        }
        else
        {
            FS_SeekFile(&StreamPlayerFile, srcOffset, 0);
            FS_ReadFileAsync(&StreamPlayerFile, StreamPlayerPcmDst + dstOffset, amount);
        }
    }
}

void StreamPlayerThreadFunction()
{
    while (true)
    {
        if (StreamPlayerUsingThread)
        {
            if (StreamPlayerIsPlaying)
                StreamPlayer_Update();
        }
        else
        {
            break;
        }
        waitForVblank();
    }
    OS_ExitThread();
}

void StreamPlayer_SwitchToThread()
{
    OS_CreateThread(&StreamPlayerThread, StreamPlayerThreadFunction, NULL, StreamPlayerStack + (STREAM_PLAYER_STACK_SIZE / sizeof(u32)), STREAM_PLAYER_STACK_SIZE, 0);
    OS_WakeupThreadDirect(&StreamPlayerThread);
    StreamPlayerUsingThread = true;
}


void StreamPlayer_SwitchToMainLoopHook()
{
    StreamPlayerUsingThread = false;

    if (!PakRamIn)
    {
        freeToCurrentHeap(StreamPlayerThreadTemp);
    }
}

void hksf_020A3310_ov_00_d() // Hook at beginning of StageScene::onCreate
{
    if (!PakRamIn)
    {
        StreamPlayerThreadTemp = allocFromCurrentHeapFromTop(16000);
        FS_SeekFile(&StreamPlayerFile, StreamPlayerPcmSrcOffset100, 0);

        if (StreamPlayerTotalLength100 >= StreamPlayerPcmSrcOffset100 + 16000)
            FS_ReadFile(&StreamPlayerFile, StreamPlayerThreadTemp, 16000);
        else
        {
            int BytesUntilEnd = StreamPlayerTotalLength100 - StreamPlayerPcmSrcOffset100;
            FS_ReadFile(&StreamPlayerFile, StreamPlayerThreadTemp, BytesUntilEnd);

            if (StreamPlayerIsLooped)
            {
                FS_SeekFile(&StreamPlayerFile, StreamPlayerLoopOffset100, 0);
                FS_ReadFile(&StreamPlayerFile, StreamPlayerThreadTemp + BytesUntilEnd, 16000 - BytesUntilEnd);
            }
        }
    }
    StreamPlayerSavedStreamOffset = 0;
    StreamPlayer_SwitchToThread();
}


void hksf_020A3394_ov_00_d(int compareToZero) // Hook at an "if (x == 0) return;" in StageScene::onCreate
{
    if (compareToZero == 0)
        StreamPlayer_SwitchToMainLoopHook();
}


void hksf_020A3438_ov_00_d(int compareToZero) // Hook at an "if (x == 0) return;" in StageScene::onCreate
{
    if (compareToZero == 0)
        StreamPlayer_SwitchToMainLoopHook();
}


void hook_020A3480_ov_00_d() // Hook at the end of StageScene::onCreate
{
    StreamPlayer_SwitchToMainLoopHook();
}

void StreamPlayer_Update()
{
    int copySize100 = StreamPlayer_GetAmountToCopy();

    float bytesPerFrame = StreamPlayerSampleRate / DS_REFRESH_RATE;
    int copyingDelay = (STREAM_BUFFER_SIZE / 2) / bytesPerFrame;

    if (StreamPlayerFrameNum < copyingDelay)
    {
        StreamPlayerFrameNum++;
        return;
    }

    const int streamBufferSize100 = STREAM_BUFFER_SIZE * 0x100;

    while (StreamPlayerPcmSrcOffset100 + copySize100 > StreamPlayerTotalLength100
        || StreamPlayerPcmDstOffset100 + copySize100 > streamBufferSize100)
    {
        int copyableSrcAmount100 = StreamPlayerTotalLength100 - StreamPlayerPcmSrcOffset100;
        int copyableDstAmount100 = streamBufferSize100 - StreamPlayerPcmDstOffset100;
        int copyableAmount100 = (copyableSrcAmount100 < copyableDstAmount100) ? copyableSrcAmount100 : copyableDstAmount100;

        StreamPlayer_CopyToBufferOrClear(StreamPlayerPcmSrcOffset100, StreamPlayerPcmDstOffset100, copyableAmount100);

        copySize100 -= copyableAmount100;

        if (!StreamPlayerZeroFill) {
            StreamPlayerPcmSrcOffset100 += copyableAmount100;
            while (StreamPlayerPcmSrcOffset100 >= StreamPlayerTotalLength100) {
                StreamPlayerPcmSrcOffset100 -= (StreamPlayerTotalLength100 - StreamPlayerLoopOffset100);
                if (!StreamPlayerIsLooped) StreamPlayerZeroFill = true;
            }
        }
        StreamPlayerPcmDstOffset100 += copyableAmount100;
        while (StreamPlayerPcmDstOffset100 >= streamBufferSize100)
            StreamPlayerPcmDstOffset100 -= streamBufferSize100;
    }

    StreamPlayer_CopyToBufferOrClear(StreamPlayerPcmSrcOffset100, StreamPlayerPcmDstOffset100, copySize100);

    if (!StreamPlayerZeroFill) {
        StreamPlayerPcmSrcOffset100 += copySize100;
        while (StreamPlayerPcmSrcOffset100 >= StreamPlayerTotalLength100) {
            StreamPlayerPcmSrcOffset100 -= (StreamPlayerTotalLength100 - StreamPlayerLoopOffset100);
            if (!StreamPlayerIsLooped) StreamPlayerZeroFill = true;
        }
    }
    StreamPlayerPcmDstOffset100 += copySize100;
    while (StreamPlayerPcmDstOffset100 >= streamBufferSize100)
        StreamPlayerPcmDstOffset100 -= streamBufferSize100;
}


// =================== GBA EXPANSION PAK / DSi SPECIFIC ==================

void StreamPlayer_Prepare_RAM(int musicID)
{
    if (musicID == MUSIC_INVINCIBILITY)
        StreamPlayerStrmAddr = StreamPlayerStarmanThemeAddr;
    else if (musicID == MUSIC_MEGA_MARIO)
        StreamPlayerStrmAddr = StreamPlayerMegaMarioThemeAddr;
    else if (musicID == MUSIC_DEATH_FANFARE)
        StreamPlayerStrmAddr = StreamPlayerDeathThemeAddr;
    else if (musicID == MUSIC_COURSE_CLEAR_FANFARE || musicID == MUSIC_COURSE_CLEAR_SMB_FANFARE)
        StreamPlayerStrmAddr = StreamPlayerCourseClearThemeAddr;
    else if (musicID == MUSIC_BOSS_CLEAR_FANFARE)
        StreamPlayerStrmAddr = StreamPlayerBossClearThemeAddr;
    else if (musicID == MUSIC_SWITCH)
        StreamPlayerStrmAddr = StreamPlayerSwitchThemeAddr;
    else
        StreamPlayerStrmAddr = StreamPlayerCustomThemeAddr;

    if (StreamPlayerStrmAddr == NULL) {
        // We get absolute earrape if this happens and we don't check for it
        OS_Panic();
    }

    StreamPlayerFrameNum = -1;
    StreamPlayerZeroFill = false;

    StreamPlayerPcmSrc = StreamPlayerStrmAddr + 0x68;
    u8* SwavPtr = GetPtrToSwavDataInSdat(GetCurrentlyPlayingFirstSwarID(musicID), 0);
    if (SwavPtr == NULL) OS_Panic();
    StreamPlayerPcmDst = SwavPtr + 0xC;
    StreamPlayerPcmDstOffset100 = 0;

    // Fill the buffer with the first STREAM_BUFFER_SIZE bytes of audio
    MI_CpuCopy8(StreamPlayerPcmSrc, StreamPlayerPcmDst, STREAM_BUFFER_SIZE);
    StreamPlayerPcmSrcOffset100 = STREAM_BUFFER_SIZE * 0x100;

    StreamPlayerIsLooped = *(StreamPlayerStrmAddr + 0x19);
    StreamPlayerLoopOffset100 = *((u32*)(StreamPlayerStrmAddr + 0x20)) * 0x100;
    StreamPlayerTotalLength100 = *((u32*)(StreamPlayerStrmAddr + 0x24)) * 0x100;
    StreamPlayerSampleRate = (u16)*((u16*)StreamPlayerStrmAddr + (0x1C / 2));

    // Start the timer
    StreamPlayer_GetTimerValueAndReset();
}


void StreamPlayer_PreloadMusic()
{
    u8* addr = (u8*)ExtRAMAddr; // we can assume this is aligned to 4

    StreamPlayerStarmanThemeAddr = addr;
    addr += loadFileByExtId_Dest(FOLDERADJ_MUSIC + MUSIC_INVINCIBILITY, addr, 0);

    while ((u32)addr % 4) addr++;
    StreamPlayerMegaMarioThemeAddr = addr;
    addr += loadFileByExtId_Dest(FOLDERADJ_MUSIC + MUSIC_MEGA_MARIO, addr, 0);

    while ((u32)addr % 4) addr++;
    StreamPlayerDeathThemeAddr = addr;
    addr += loadFileByExtId_Dest(FOLDERADJ_MUSIC + MUSIC_DEATH_FANFARE, addr, 0);

    while ((u32)addr % 4) addr++;
    StreamPlayerCourseClearThemeAddr = addr;
    addr += loadFileByExtId_Dest(FOLDERADJ_MUSIC + MUSIC_COURSE_CLEAR_FANFARE, addr, 0);

    while ((u32)addr % 4) addr++;
    StreamPlayerBossClearThemeAddr = addr;
    addr += loadFileByExtId_Dest(FOLDERADJ_MUSIC + MUSIC_BOSS_CLEAR_FANFARE, addr, 0);

    while ((u32)addr % 4) addr++;
    StreamPlayerSwitchThemeAddr = addr;
    addr += loadFileByExtId_Dest(FOLDERADJ_MUSIC + MUSIC_SWITCH, addr, 0);

    while ((u32)addr % 4) addr++;
    StreamPlayerCustomThemeAddr = addr;
}


void StreamPlayer_LoadMusicFileIntoPakRam()
{
    if (!PakRamIn || StreamPlayerMusicIDToLoad == -1)
        return;

    if (!(100 <= StreamPlayerMusicIDToLoad && StreamPlayerMusicIDToLoad <= 106))
    {   // TEMPORARY -- remove this when we implement a custom world map
        if (StreamPlayerFileOpen)
            FS_CloseFile(&StreamPlayerFile);
        else
            StreamPlayerFileOpen = true;

        FS_InitFile(&StreamPlayerFile);
        FS_OpenFileFast(&StreamPlayerFile, 0x2096114, FOLDER_MUSIC + StreamPlayerMusicIDToLoad);
        FS_SeekFile(&StreamPlayerFile, 0, 0);
        FS_ReadFileAsync(&StreamPlayerFile, StreamPlayerCustomThemeAddr, *(StreamPlayerFile + 0xD));

        StreamPlayerMusicIDToLoad = -1;
    }
}


void hksf_0204F1A8(int musicID) // Hook in LoadMusicFile
{
    StreamPlayerMusicIDToLoad = musicID;

    u32* CurrentSceneID = (u32*)0x203BD34;

    if (*CurrentSceneID != 0xD)
        StreamPlayer_LoadMusicFileIntoPakRam();
}


void hksf_02152ED4_ov_54_d() // Hook in CRSIN thread
{
    StreamPlayer_LoadMusicFileIntoPakRam();
}


void hksf_020CDD0C_ov_09_d() // Hook in LoadTitlescreenThings
{
    if (!PakRamIn) return;
    StreamPlayer_PreloadMusic();
}


// ========================= ROM-LOADING SPECIFIC ========================

void StreamPlayer_Prepare_ROM(int musicID)
{
    if (StreamPlayerFileOpen)
        FS_CloseFile(&StreamPlayerFile);
    else
        StreamPlayerFileOpen = true;

    FS_InitFile(&StreamPlayerFile);
    FS_OpenFileFast(&StreamPlayerFile, 0x2096114, FOLDER_MUSIC + musicID);

    FS_SeekFile(&StreamPlayerFile, 0x68, 0);
    FS_ReadFile(&StreamPlayerFile, StreamPlayerPcmDst, STREAM_BUFFER_SIZE);

    FS_SeekFile(&StreamPlayerFile, 0x19, 0);
    FS_ReadFile(&StreamPlayerFile, &StreamPlayerIsLooped, 1);
    FS_SeekFile(&StreamPlayerFile, 0x20, 0);
    FS_ReadFile(&StreamPlayerFile, &StreamPlayerLoopOffset100, 4);
    StreamPlayerLoopOffset100 *= 0x100;
    FS_SeekFile(&StreamPlayerFile, 0x24, 0);
    FS_ReadFile(&StreamPlayerFile, &StreamPlayerTotalLength100, 4);
    StreamPlayerTotalLength100 *= 0x100;
    FS_SeekFile(&StreamPlayerFile, 0x1C, 0);
    FS_ReadFile(&StreamPlayerFile, &StreamPlayerSampleRate, 2);
}


// ======================== START / STOP FUNCTIONS =======================

void hksf_0204E5AC(int musicID) // Hook in startMusic
{
    StreamPlayer_Prepare(musicID);
}


void hksf_02004F14() // Hook in main loop
{
    if (StreamPlayerMeanCounterArray)
    {
        if (StreamPlayerMeanCounterTimer != -1)
        {
            int value = StreamPlayer_GetTimerValueAndReset();
            //OS_Report_Num("value: ", value);
            if (value < 0x90000)
            {
                StreamPlayerMeanCounterArray[StreamPlayerMeanCounterLength] = value;
                StreamPlayerMeanCounterLength++;
            }
        }
        else
        {
            StreamPlayer_GetTimerValueAndReset();
        }

        StreamPlayerMeanCounterTimer++;

        if (StreamPlayerMeanCounterTimer == STREAM_PLAYER_MEAN_COUNTER_FRAMES + 1)
        {
            u32 TimerTotal = 0;

            for (int i = 0; i < STREAM_PLAYER_MEAN_COUNTER_FRAMES; i++)
                TimerTotal += StreamPlayerMeanCounterArray[i];

            // TimerTotal / StreamPlayerMeanCounterLength, but rounded instead of floored
            StreamPlayerMeanTimerTicks = (TimerTotal + StreamPlayerMeanCounterLength / 2) / StreamPlayerMeanCounterLength;
            freeToCurrentHeap(StreamPlayerMeanCounterArray);
           //OS_Report_Num("Mean:", StreamPlayerMeanTimerTicks);

            StreamPlayerMeanCounterArray = NULL;
        }
    }

    if (StreamPlayerIsPlaying)
        StreamPlayer_Update();
}

void hook_020CCB38_ov_01_d()      // Hook at start of boot scene
{
    StreamPlayerMeanCounterArray = allocFromCurrentHeapFromTop(STREAM_PLAYER_MEAN_COUNTER_FRAMES * 2);
    StreamPlayerMeanCounterTimer = -1;
    StreamPlayerMeanCounterLength = 0;
}

void hksf_0205D7CC(u32 MusicPlayer) // Hook in NNSi_SndPlayerStartSeq
{
    if (MusicPlayer == *((u32*)0x208FC08))
        StreamPlayerIsPlaying = true;
}


void hksf_0205D744(u32 MusicPlayer, int PauseCond) // Hook in NNSi_SndPlayerPause
{
    if (MusicPlayer == *((u32*)0x208FC08))
    {
        if(!PauseCond)
            StreamPlayerIsPlaying = true;
        else
            StreamPlayerIsPlaying = false;
    }
}


void hksf_0205D5DC(u32 MusicPlayer) // Hook in ForceStopSeq
{
    if (MusicPlayer == *((u32*)0x208FC08))
        StreamPlayerIsPlaying = false;
}


// ==================== GENERAL ===========================

u8* GetPtrToSwavDataInSdat(int SwarId, int SwavId)
{
    u32* SWARPointer = NNS_SndArcGetFileAddress(*(NNS_SndArcGetWaveArcInfo(SwarId, 0xFF, (u32*)0x208FBB8)));
    if (SWARPointer == NULL) return NULL;
    return GetPtrToSwavData(SWARPointer, SwavId);
}


int GetCurrentlyPlayingFirstSwarID(int musicID)
{
    u16* seqInfo = (u16*)NNS_SndArcGetSeqInfo(musicID, 0xFF, (u32*)0x208FBB8);
    u16 bankID = *(seqInfo + 2);
    u16* bankInfo = (u16*)NNS_SndArcGetBankInfo(bankID, 0xFF, (u32*)0x208FBB8);
    return *(bankInfo + 2);
}


u8* GetPtrToSwavData(u32* SwarPtr, int SwavId)
{
    if (*SwarPtr != 0x52415753) // "SWAR"
        return NULL;

    int SwavCount = *(SwarPtr + 14);
    if (SwavId >= SwavCount)
        return NULL;

    return (u8*)(SwarPtr + (*(SwarPtr + 15 + SwavId)) / 4);
}
