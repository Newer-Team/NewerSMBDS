#include "game.h"
#include "VariousFunctions.h"
#include "keys.h"

extern "C" {
	void nds_printf(u16 color[2], u16* screenPtr, const char* str, ...);
	void clearDebugScreen();
}

u16 WHITE[2] = {0x0000, 0xD000};
u16 GREEN[2] = {0x0000, 0xE000};
u16 RED[2] = {0x0000, 0xF000};

int World = 1;
int Level = 1;
int State = 0;

FSFile SDATFile;
u8* SSARLengthsFileAddress;
u8  SSARName[64];
u8  SNDName[64];
u8  GroupName[64];

void nsub_020CC328_ov_06()				// EXECUTE
{
	int *lives = (int*)(0x0208B364);
	int *player = (int*)(0x02085A50);
	
	*lives = 99;
	myScanKeys();
	int keysDw = myKeysDown();	

	
	if (State == 0)
	{
		if (World < 8)
		{
			if (keysDw & UP) 
				World++;
		}
		if (World > 1)
		{
			if (keysDw & DOWN) 
				World--;		
		}
		
		if (keysDw & RIGHT)
			State = 1;
		
	}
	if (State == 1)
	{
		if (Level < 99)
		{
			if (keysDw & UP) 
				Level++;
		}
		if (Level > 1)
		{
			if (keysDw & DOWN) 
				Level--;		
		}
		
		if (keysDw & LEFT)
			State = 0;
	}
	
	if (keysDw & SLEFT)
		*player = 1;
	if (keysDw & SRIGHT)
		*player = 0;
	
	if ((keysDw & A) & (State != 255))
	{
		State = 255;
		loadLevel(0xD, false, World - 1, Level - 1, 0, 0, 1, *player, 1, 1, 0xC, 1);
	}
}

void nsub_020CC464_ov_06()					// DRAW
{	
	u16** topScreenPtr = (u16**)0x020859EC;
//	u16** subScreenPtr = (u16**)0x020859E8;	
	clearDebugScreen();
	
	nds_printf(WHITE, *topScreenPtr+35,  "Newer Super Mario Land DS");
	nds_printf(WHITE, *topScreenPtr+74,  "DEBUG MENU");
	
	nds_printf(WHITE, *topScreenPtr+128,  "Select a level to load:");
	
	nds_printf(WHITE, *topScreenPtr+192 + (5 * State),  "^");
	
	
	nds_printf(WHITE, *topScreenPtr+224,  "%02d -", World);
	nds_printf(WHITE, *topScreenPtr+229,  "%02d", Level);
	
	nds_printf(WHITE, *topScreenPtr+256 + (5 * State),  "v");

}

