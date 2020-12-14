#include "game.h"
#include "VariousFunctions.h"
#include "sfx.h"


bool ontile = false;
int tiletimer = 0; 

void hook_020FD1D8(MarioActor *athis)
{	
	u8 *Frozen = (u8*)(0x020CA850);
	if (athis->currentMarioAnim == 0x22) tiletimer = 0;
	if ((tiletimer) && (*Frozen == 0)) tiletimer--;
	
	if ((athis->IsMarioGroundPounding77E & 0x2) == 0 && athis->currentMarioAnim != 0x11 && athis->currentMarioAnim != 0xD && athis->currentMarioAnim != 0xC)
		{
			u32 xPos = (athis->xPos>>12) / 16; u32 yPos = ((-athis->yPos)>>12) / 16;
			int tb = getTileBehaviorAtPos2(xPos*16, yPos*16);
			int tbN = (tb & 0xF0) >> 4;
			
			if (tbN == 0xF)
			{
				if (!ontile) 
				{
					Rumble(1);
					PlaySNDEffect(SE_SYS_RED_RING, &athis->unk05C);
					SpawnParticle(67, &athis->unk05C);
					athis->direction = 0;
				}
				ontile = true;
				tiletimer = 80;
			}
			else ontile = false;
			
			if ((tiletimer>=20) && (tiletimer))
				{
					athis->xVelocity = 0x5000; 
					*((u32*)(((u32*)(athis->unk6BC))+3)) = 0x5000;
					
				}
			if ((tiletimer<20) && (tiletimer))
				{
					if (athis->xVelocity > 0) athis->xVelocity+=0x100; 
					(*((u32*)(((u32*)(athis->unk6BC))+3))) -=0x100;
				}
		}
	if (abs(athis->xVelocity) <= 0x3000) *((u32*)(((u32*)(athis->unk6BC))+3)) = 0x3000;
}

void hook_020A12D0()
	{
		tiletimer = 0;
	}