void StreamPlayer_Prepare(int musicID);
static inline int StreamPlayer_GetAmountToCopy();
static inline void StreamPlayer_CopyToBufferOrClear(int srcOffset, int dstOffset, int amount);
void StreamPlayer_Update();

void StreamPlayer_Prepare_RAM(int musicID);
void StreamPlayer_PreloadMusic();

void StreamPlayer_Prepare_ROM(int musicID);

u8* GetPtrToSwavDataInSdat(int SwarId, int SwavId);
int GetCurrentlyPlayingFirstSwarID(int musicID);
u8* GetPtrToSwavData(u32* SwarPtr, int SwavId);
