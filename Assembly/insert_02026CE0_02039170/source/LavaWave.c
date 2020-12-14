#include "game.h"
#include "registers.h"
#include "LavaWave.h"
#include <math.h>

int LavaWave_loadFiles()
	{
		for (int i=0; i<12; i++)
		{
			loadFileByExtId_3dModel(2654-0x83 + i, 0);
		}
		return 1;
	}

void LavaWave_setup(LavaWave* wave)
	{
		wave->ExistDist.x = 0xFFFF;
		wave->ExistDist.y = 0xFFFF;
		
		wave->scale.x = 1 + (wave->spriteData & 0xF);				// Nybble 12.
		wave->scale.y = 1 + ((wave->spriteData >> 4) & 0xF);		// Nybble 11.

		wave->speed = ((wave->spriteData >> 8) & 7);				// Nybble 10.
		wave->speedsign = ((wave->spriteData >> 8) & 8);

		if ((wave->spriteData >> 12) & 1) wave->position.x += 0x8000;	// Nybble 9 & 1.
		if ((wave->spriteData >> 12) & 2) wave->position.y += 0x8000;	// Nybble 9 & 2.
		if ((wave->spriteData >> 12) & 4) wave->position.y += 0x4500;	// Nybble 9 & 4. Aligns with normal lava.

		wave->position.z = 0x300000;										// Goes above everything
		
		wave->frame = 0;
		LavaWave_UpdateTextureParams(wave, 2654-0x83, wave->frame);

		wave->mode = ((wave->spriteData >> 28) & 0xF);	// Nybble 5. Mode & 0 = Spawns waves across entire zone. Mode & 1 = Spawns a single wave (used internally by mode 0). Mode & 2 = Waves don't return to original position

		if (!(wave->mode & 1))							// Mode & 0 code. Spawns mode & 1 waves across the entire zone.
		{
			int CurrentView = wave->ViewID;
			
			wave->position.y -= 0x10000;														// So bottom of box in NSMBE = bottom of lava

			wave->position.x = (16 * (*(((u16*)getPtrToViewById(CurrentView,0)) + 1)));		// Moves the mode 0 wave to beginning of the screen.

			int SpaceBetween = ((wave->spriteData >> 16) & 0xF) * 0x10000;													// Nybble 8
			int AmountToSpawn = 3 + ((getWidthForView(CurrentView)) / ((0x10 * wave->scale.x) + SpaceBetween));

			Vec3 Pos;
			Pos.x = wave->position.x;
			Pos.y = wave->position.y;
			Pos.z = wave->position.z;

			for (int i = 0; i < AmountToSpawn; i++)
				{
					Pos.x += SpaceBetween + (0x10000 * wave->scale.x);
					createActor(0x131, (wave->spriteData | (1 << 28)), &Pos, 0, 0, 0);
				}
		}

		wave->initialX = wave->position.x;
	}

void LavaWave_update_state0(LavaWave* wave)
	{
		int SpaceBetween = ((wave->spriteData >> 16) & 0xF) * 0x10000;

		if (wave->speedsign)
			{
				wave->position.x += wave->speed * 0x250;
				if (((wave->initialX + wave->scale.x * 0x10000 + SpaceBetween) <= wave->position.x) && !(wave->mode & 2))
					wave->position.x = wave->initialX;
			}
		else
			{
				wave->position.x -= wave->speed * 0x250;
				if (((wave->initialX - wave->scale.x * 0x10000 - SpaceBetween) >= wave->position.x) && !(wave->mode & 2))
					wave->position.x = wave->initialX;
			}

		wave->frame++;
		if (wave->frame >= 96) wave->frame = 0;

		if (CheckIfMarioInRange(wave))	// No need to draw or check collisions if the waves aren't on-screen.
			{
				CheckCollisions(wave);
			}
		return;
	}

bool LavaWave_beforeDraw(LavaWave* wave)
	{
		return CheckIfMarioInRange(wave);
	}

static inline void vec3(int x, int y, int z)
	{
		REG_VERTEX16 = (y << 16) | (x & 0xFFFF);
		REG_VERTEX16 = (z & 0xFFFF);
	}

static inline void texcoord2(int x, int y)
	{
		REG_TEXT_COORD = (y << 16) | (x & 0xFFFF);
	}

void G3_LoadMtx43(u32 unk);

void LavaWave_UpdateTextureParams(LavaWave* wave, int File, int frame)
	{
		u8* texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(File));
		getTextureParams(texture, 0, &wave->WaveTex.texparam);
		getTexturePalBase(texture, 0, &wave->WaveTex.palbase);		
	}

void LavaWave_draw(LavaWave* wave)
	{
		int frame = wave->frame/8;
		
		Vec3 Scale; 
		Scale.x = 0x8000 * wave->scale.x;
		Scale.y = 0x10000 * wave->scale.y;
		Scale.z = 0;
		
		Vec3 Pos;
		Pos.x = wave->position.x + (0x8000 * wave->scale.x);
		Pos.y = wave->position.y;
		Pos.z = wave->position.z;
		
		LavaWave_UpdateTextureParams(wave, (2654 + frame -0x83), 0);
		
		LavaWave_DrawPolygon(wave, wave->position, Scale, 0x400, 0, 0, 0x800);
		
		LavaWave_UpdateTextureParams(wave, (2654 + ((12 - frame) % 12) -0x83), 0);
		
		LavaWave_DrawPolygon(wave, Pos, Scale, 0, 0x400, 0, 0x800);
	}


void LavaWave_DrawPolygon(LavaWave* wave, Vec3 Position, Vec3 Scale, int TexXA, int TexXB, int TexYA, int TexYB)
	{
		NNS_G3dGeFlushBuffer();
		G3_LoadMtx43(0x02085B20);

		REG_MTX_TRANSLATE = Position.x;
		REG_MTX_TRANSLATE = Position.y;
		REG_MTX_TRANSLATE = Position.z;

		REG_MTX_SCALE = Scale.x;
		REG_MTX_SCALE = Scale.y;
		REG_MTX_SCALE = 0;
 
		REG_MTX_CONTROL = 3;
		REG_MTX_IDENTITY = 0;
		REG_MTX_CONTROL = 2;

		REG_POLY_FORMAT = 0x001F3880;
		REG_TEXT_FORMAT = wave->WaveTex.texparam;
		REG_PAL_BASE = wave->WaveTex.palbase;

		REG_COLOR= 0x7FFF;

		REG_GFX_BEGIN = 1;
		texcoord2(TexXA, TexYA);
		vec3(0x1000, 	0x1000, 	0x0);

		texcoord2(TexXB, TexYA);
		vec3(0x0000,	0x1000, 	0x0);

		texcoord2(TexXB, TexYB);
		vec3(0x0000, 	0x0000,		0x0);

		texcoord2(TexXA, TexYB);
		vec3(0x1000,	0x0000,		0x0);

		REG_GFX_END = 0;
	}

bool CheckIfFellInLava(LavaWave* wave, int x, int y)
	{
		const float TAU = 6.28318530718;
		
		int frame = wave->frame / 8;

		int waveTexTileHeight = wave->scale.y + 1;

		int topOfWave = wave->position.y + (waveTexTileHeight - 1) * 0x10000;
		float waveSize = wave->scale.x * 0x10000;
		int SpaceBetween = ((wave->spriteData >> 16) & 0xF) * 0x10000;

		int relativeX = x - wave->position.x;

		if (relativeX < 0 || relativeX > waveSize + SpaceBetween) return false;
		else if (relativeX > waveSize)
			return y <= topOfWave - (waveTexTileHeight - 1) * 0x10000;

		float relativeNormalizedX = relativeX / waveSize;
		int waveHeight = topOfWave - ((cos(TAU * relativeNormalizedX) + 1) / 2) * (waveTexTileHeight * 0.75) * 0x10000;
		waveHeight -= (1 + cos(TAU * (relativeNormalizedX * 4 + (float)frame / 12))) * waveTexTileHeight * 0x800;
		waveHeight -= 0x2000; // Move the colls down 1/8 of a tile -- fixes some bugs

		return y <= waveHeight;
	}

bool CheckIfMarioInRange(LavaWave* wave)
	{
		u16 *StageZoom = (u16*)(0x020CADB4);
		
		MarioActor* Player = (MarioActor*)getPtrToPlayerActor();
		
		int xoffs = Player->xPos - wave->position.x;
		int yoffs = Player->yPos - wave->position.y;
		int DrawDistance = (*StageZoom * 0x100) + wave->scale.x * 0x10000;
		
		if (xoffs > DrawDistance || xoffs < -DrawDistance || yoffs < -0x100000 || yoffs > 0x300000)  return false;
		
		else return true;
	}
	
void CheckCollisions(LavaWave* wave)
	{
		MarioActor* Player = (MarioActor*)getPtrToPlayerActor();
		EnemyClassObject* KoopaAddr = (EnemyClassObject*) findActorByType(94, 0);
		EnemyClassObject* GiantKoopaAddr = (EnemyClassObject*) findActorByType(54, 0);
		
		if (CheckIfFellInLava(wave, Player->xPos, Player->yPos))
			{
				Player->DeathState = 0x211997C;
				
				bool* MenuLocked = (bool*)0x020CA880;
				*MenuLocked = true;
			}

		while (KoopaAddr != 0)
			{
				if (CheckIfFellInLava(wave, KoopaAddr->position.x, KoopaAddr->position.y))
					{
						SpawnParticle(22, &KoopaAddr->position);

						if (KoopaAddr->SpriteSpecificData2 != 1)
							{
								EnemyClassObject* DryBone = createActor(55, 0, &KoopaAddr->position, 0, 0, 0);
								DryBone->direction = KoopaAddr->direction;
								if (DryBone->direction) DryBone->rotation.y = 0xE000; else DryBone->rotation.y = 0x2000; 
							}
						Base__deleteIt(KoopaAddr);
						KoopaAddr->position.x = 0;				
					}
				KoopaAddr = (EnemyClassObject*) findActorByType(94, KoopaAddr->base.stuff[6]);
			}

		while (GiantKoopaAddr != 0)
			{
				if ((GiantKoopaAddr->spriteData >> 8) & 0xF)			//Check if Buzzy Beetle is a Koopa.
					{
						if (CheckIfFellInLava(wave, GiantKoopaAddr->position.x, GiantKoopaAddr->position.y))
							{
								Vec3 BigParticlePos;

								BigParticlePos.x = GiantKoopaAddr->position.x-0x8000;
								BigParticlePos.y = GiantKoopaAddr->position.y;
								BigParticlePos.z = GiantKoopaAddr->position.z;
								
								for (int i = 0; i <= 2; i++)
									{
										SpawnParticle(22, &BigParticlePos);
										BigParticlePos.x += 0x8000;
										if (i == 1) BigParticlePos.y += 0x8000;
										if (i == 2) BigParticlePos.y -= 0x8000;
									}

								if (*((u8*)GiantKoopaAddr+0x581) != 2)				// If not in shell
									{
										EnemyClassObject* DryBone = createActor(56, 0, &GiantKoopaAddr->position, 0, 0, 0);
										
										DryBone->scale.x *= 1.3;
										DryBone->scale.y *= 1.3;
										DryBone->scale.z *= 1.3;
										DryBone->activephysics.sizex *= 1.3;
										DryBone->activephysics.sizey *= 1.4;
										DryBone->activephysics.offsety += 0x5000;
										DryBone->direction = GiantKoopaAddr->direction;
										DryBone->rotation.y = GiantKoopaAddr->rotation.y; 
									}
								
								GiantKoopaAddr->position.x = 0;								
								Base__deleteIt(GiantKoopaAddr);
							}
					}
				GiantKoopaAddr = (EnemyClassObject*) findActorByType(54, GiantKoopaAddr->base.stuff[6]);
			}
	}