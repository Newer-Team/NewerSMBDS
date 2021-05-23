#include "game.h"
#include "Enemy.h"
#include "VariousFunctions.h"

u32* SSEQPointer;
int filebeingplayed = 0;

void MusicSwitcher_ChangeMusic(EnemyClassObject* ms)
{   
    int fileoffs = (ms->spriteData & 0xFFF)*3;
    
    // Enable Tempo Switcher
    if (ms->spriteData & 0xF000) return;
    
    // Clear current music being played
    if (ms->spriteData & 0xF0000) 
    {
        filebeingplayed = 0;
        Base__deleteIt(ms);
        return;
    }
    
    if (filebeingplayed == fileoffs)
    {
        Base__deleteIt(ms);
        return;
    }
    else 
        filebeingplayed = fileoffs;
    
    u8* ViewPtr = (u8*)getPtrToViewById(ms->ViewID, 0);
    int SSEQID = *(ViewPtr + 0xA);
    u8* SBNKInfoPtr = (u8*)NNS_SndArcGetSeqInfo(SSEQID, 0xFF, (u32*)0x208FBB8);
    int SBNKID = *(SBNKInfoPtr + 4);
    u8* SWARInfoPtr = (u8*)NNS_SndArcGetBankInfo(SBNKID, 0xFF, (u32*)0x208FBB8);
    int SWARID = *(SWARInfoPtr + 4);
    
    SwapASseq(SSEQID, 2756-0x83 + fileoffs);
    SwapASwar(SWARID, 2757-0x83 + fileoffs);
    SwapASbnk(SBNKID, 2758-0x83 + fileoffs);

    Base__deleteIt(ms);
    return;
}

void MusicSwitcher_TempoChanger()
{
    MarioActor* Player = (MarioActor*) getPtrToPlayerActor();
    
    int **PtrToBlock1 = (int**)(0x208B168);
    int *timer = (int*)(0x020CA8B4);    
    
    if ((((*((*PtrToBlock1)+1))&0xFF) < 0x64) || (*timer > 409600)) 
        NNS_SndPlayerSetTempoRatio((u32*)0x208FC08, 0xF0+((Player->xAccel)/0x100)); 
}

