
#ifndef _VARIOUSFUNCTIONS_H
#define _VARIOUSFUNCTIONS_H

#include "game.h"
#include "Enemy.h"

#ifdef __cplusplus
extern "C"
{
#endif
void StopRumble();
int CheckForRumblePak();
void Rumble(int Frames);
void DoRumble();
void SetupScreensForEnpgs();
int bgInitNerDS(int layer, BgType type, BgSize size, int mapBase, int tileBase, bool screen);
void bgSetScaleNerDS(int id, s32 sx, s32 sy);

int DrawEnpgToScreen(int FileID, int Layer, int Offs, bool screen);
void CopyEnpgToScreen(int FileID, int Layer, int Offs, bool screen);
void copyVRAMTile(void* sptr, void* dptr, int width, int height, int dx, int dy, int sx, int sy);
void copyVRAMTileWithAlpha(void* sptr, void* dptr, int width, int height, int dx, int dy, int sx, int sy);
void changeTileAny(void* ptr, int x, int y, int t);

void FadeOut(s8 screen, s8 start, int speed);
void FadeIn(s8 screen, s8 start, int speed);
void WaitFrames(int Frames);

void SetLastSaveSlot();
bool CheckSaveForBlankness();
bool CheckIfSecret();
u8 GetWorldForTitlescreen();

bool IsTouched();
u32 RNG(u32 randMax);
void AddSecondsToTimer(int amount);
bool eventIsActivated(u32 id);
void activateEvent(u32 id);

void SetTilesetTo(int NCG, int NCL, int PNL, int CHK, int id);
void SetTopBgTo(int NCG, int NCL, int NSC, int Parallax);
void SetBottomBgTo(int NCG, int NCL, int NSC, int Parallax);

void SetTilesetToID(int id);
void SetTopBgToID(int id);
void SetBottomBgToID(int id);

void PlayMusic(int ID, int unk);

u16 getNodeIndexAdjustmentValue();
PathEntry* getPath(int pathID);
bool positionInLocation(int locationID, Vec3* position);

void SwapASwar(int IDToSwap, int file);
void SwapASbnk(int IDToSwap, int file);
void SwapASseq(int IDToSwap, int file);
void SwapASsar(int IDToSwap, int file);

int ParticlesForTile(u32 collisions);


// http://problemkaputt.de/gbatek.htm#gbakeypadinput
#define GBA_BUTTON_A      (1<<0)
#define GBA_BUTTON_B      (1<<1)
#define GBA_BUTTON_SELECT (1<<2)
#define GBA_BUTTON_START  (1<<3)
#define GBA_BUTTON_RIGHT  (1<<4)
#define GBA_BUTTON_LEFT   (1<<5)
#define GBA_BUTTON_UP     (1<<6)
#define GBA_BUTTON_DOWN   (1<<7)
#define GBA_BUTTON_R      (1<<8)
#define GBA_BUTTON_L      (1<<9)
#define GBA_BUTTON_PRESSED(which) (!(*((int*)0x4000130)&which))

#define TouchDown (*(bool*)0x020888E5)
#define TouchX (*(u8*)0x020888E6)
#define TouchY (*(u8*)0x020888E7)

#define AREA_TOUCHED(x,y,w,h) ((x)<=(TouchX)&&(TouchX)<(x)+(w)&&(y)<=(TouchY)&&(TouchY)<(y)+(h))

#define ButtonsHeld ((u16*)0x02087650)
#define ButtonsPressed ((u16*)0x02087652)
#define ButtonsReleased ((u16*)0x02087640)

#ifdef __cplusplus
}
#endif

#endif
