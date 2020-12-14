#include "Enemy.h"
#include "Mario.h"

int nsub_020E0764_ov_0A(u8* Goomba)
{
	Vec3 Scale;
	Scale.x = 0x1000;
	Scale.y = 0x1000;
	Scale.z = 0x1000;
	
	if (*(Goomba + 0x452))
	{
		Scale.x = 0x800;
		Scale.y = 0x800;
		Scale.z = 0x800;
	}
	
	DrawAModel(Goomba + 0x460, Goomba + 0x5C, Goomba + 0x9C, &Scale);
	
	Scale.x = 0x1000;
	Scale.y = 0x1000;
	Scale.z = 0x800;
	
	if (*(Goomba + 0x434))
	{
		Scale.x = 0x1000;
		Scale.y = 0x1000;
		Scale.z = 0x400;		
		DrawAModel(Goomba + 0x50C, Goomba + 0x5C, Goomba + 0x9C, &Scale);
	}
	
	return 1;
}

bool Goomba_LoadFiles()
{
	loadFileByExtId_3dModel(1542-131, 0);		// Goomba
	loadFileByExtId_3dModel(1541-131, 0);		// Anim
	loadFileByExtId_3dModel(1591-131, 0);		// Wings (Pata block with blanked texture)
	loadFileByExtId_3dModel(1590-131, 0);		// Wings anim
	return true;
}

void hook_020E0ABC_ov_0A(u8* Goomba)
{
	modelAnim3d_ctor((ModelAnim3d*)(Goomba + 0x460));
	modelAnim3d_setup((ModelAnim3d*)(Goomba + 0x460), getPtrToLoadedFile(1542-131), getPtrToLoadedFile(1541-131), 0, 0, 0);
	modelAnim3d_init((ModelAnim3d*)(Goomba + 0x460), 0, 0, 0x1200, 0);

	u8 Class = *(((u16*)Goomba) + 0xC / 2);
	
	if (Class == 0x54)
	{
		modelAnim3d_ctor((ModelAnim3d*)(Goomba + 0x50C));
		modelAnim3d_setup((ModelAnim3d*)(Goomba + 0x50C), getPtrToLoadedFile(1591-131), getPtrToLoadedFile(1590-131), 0, 0, 0);
		modelAnim3d_init((ModelAnim3d*)(Goomba + 0x50C), 0, 0, -0x700, 0);	
	}
}

void hook_020E22B8_ov_0A(u8* Goomba)
{
	if ((*(Goomba + 0x434)) == 0)
		modelAnim3d_init((ModelAnim3d*)(Goomba + 0x460), 1, 0, 0xA00, 0);	
}

void hook_020E09F4_ov_0A(u16* Goomba)
{
	anim3d_update((Animation3d*)(Goomba + (0x4F0 / 2)));
	
	u8 IsParaGoom = *(((u8*)Goomba) + 0x434);
	
	if (IsParaGoom)
		anim3d_update((Animation3d*)(Goomba + (0x59C / 2)));

	
	u8 Direction = *(((u8*)Goomba) + 0x2BD);
	
	if (Direction == 0)
		*(Goomba + (0xA2 / 2)) = 0x2000;
	else
		*(Goomba + (0xA2 / 2)) = -0x2000;
}