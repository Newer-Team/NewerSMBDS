#include "game.h"
#include "VariousFunctions.h"

u32 PrevPos1 = 0; 
u32 PrevPos2 = 0; 
u32 BgPrevPos = 0;
u32 BgPrevPos2 = 0; 

u32 *CurrentPos1 = (u32*)(0x02085BFC);
u32 *CurrentPos2 = (u32*)(0x02085BD0);
u32 *CurrentBGPos = (u32*)(0x02085C00);
u32 *CurrentBGPos2 = (u32*)(0x02085BF8);

void hook_020AD27C()
	{
		PrevPos1 = *CurrentPos1;
		PrevPos2 = *CurrentPos2;
		BgPrevPos = *CurrentBGPos;
		BgPrevPos2 = *CurrentBGPos2;
	}


void hook_020AD514()
	{	
		EnemyClassObject* Mario = (EnemyClassObject*)getPtrToPlayerActor();
		
		if ((u32)Mario < 0x0200000) return;

		if (positionInLocation(99, &Mario->position))
		{
			*CurrentPos1 = PrevPos1;
			*CurrentPos2 = PrevPos2;
			*CurrentBGPos = BgPrevPos;
			*CurrentBGPos2 = BgPrevPos2;
		}
	}

