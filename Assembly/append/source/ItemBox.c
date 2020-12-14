#include <nds.h>
#include "registers.h"
#include "ItemBox.h"
#include "game.h"
#include "sfx.h"

u32 Itembox_physics[] = 
	{
		0x0,	
		0x0,	
		0x12000,	
		0x12000,	
		0x00D30004, 
		0x0400,
		(u32)&ItmBoxCollision,
	};


int Itembox_loadFiles()
	{
		loadFileByExtId_3dModel(2637-0x83, 0);	
		loadFileByExtId_3dModel(2638-0x83, 0);
		return 1;
	}
	
bool Itembox_heapCreated(Itembox* itmb)
	{
		return modelAnim3d_setup(&itmb->model, getPtrToLoadedFile(2637-0x83), getPtrToLoadedFile(2638-0x83), 0, 0, 0);
	}

void Itembox_setup(Itembox* itmb)
	{	
		modelAnim3d_init(&itmb->model, 0, 0, 0x2000, 0);
		itmb->scale.x = 0x20000;
		itmb->scale.y = 0x20000;
		itmb->scale.z = 0x20000;
		itmb->position.y += 0x10000;
		itmb->position.z = 0;
		
		initActivePhysics(&itmb->activephysics, itmb, Itembox_physics, 0);

		registerCollider(&itmb->activephysics);
		
		u16 SettingsID = (itmb->spriteData & 0xF);
		u32* pointerToBlock14 = (u32*)0x0208B19C;
		itmb->settings = (ItemBoxSettigs*)(*pointerToBlock14 + 16 + SettingsID*16);
		
	}

void Itembox_update_state0(Itembox* itmb)
	{	
		anim3d_update(&itmb->model.anim);
	}
	
bool Itembox_Draw(Itembox* itmb)
	{
		DrawAModel(&itmb->model, &itmb->position, &itmb->rotation, &itmb->scale);
		return 1;
	}

bool Itembox_beforeDraw(Itembox* itmb)
	{
		return 1;
	}	

void ItmBoxCollision(ActivePhysics* a, ActivePhysics* b)
	{
		
		Itembox* itmb = (Itembox*) a->owner;
		MarioActor* mA = (MarioActor*) b->owner;
		
		if ((u32)mA != (u32)getPtrToPlayerActor()) return; 
		
		SpawnParticle(62, &itmb->position);
		PlaySNDEffect(SE_PLY_STAR_ATTACK, &itmb->position);
		
		Vec3 sActorpos;
		
		sActorpos.x = itmb->position.x+itmb->settings->offsetX*0x1000;
		sActorpos.y = itmb->position.y+itmb->settings->offsetY*0x1000;
		sActorpos.z = itmb->position.z;
		
		for (int i = 0; i < (itmb->spriteData & 0xF0) >> 4; i++)
			{
				u32* spawned = createActor(itmb->settings->classID, itmb->settings->spriteData, &sActorpos, 0, 0, 0);
				if (mA->xVelocity<=0) 
					{
						*(spawned + 0x34)-=(abs)(mA->xVelocity)+0x200+i*0x600; 
					}
				else 
					{
						*(spawned + 0x34)+=(abs)(mA->xVelocity)+0x200+i*0x600;
					}
			}
		
		Base__deleteIt(itmb);
		return;
	}

