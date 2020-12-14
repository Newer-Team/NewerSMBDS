#include "VariousFunctions.h"

extern u8 lastslot;

// Globals

u32 ViewStartAndEndForBoundsLimit[2] = {0,0};

bool MusicOffMode = false;
bool IsSpecialLevel = false;
bool ControlLock = false;
bool ControlLockWasEnabled = false;
bool TouchscrControlLock = false;
bool DisableMarioYRotationForce = false;
bool HammerThrowingEnabled = true; // used by Minecart
bool MarioUsingCart = false; // used by Minecart
int TitleScrNumber = 0;
int MinecartHP = 1;
u16 startNodeAdjustment;
s32 ForcePlayerPowerupTo = -1;
u8 MidwayPointSecond = -1;
s8 MidwayAreaSecond = -1;

u8 RumbleTimer = 0;
u8 RumblePakType = 0;
int RumbleForce = 0;

Rect2D temporaryRect; // for temporary use within a function only -- don't store permanent pointers to this

LocationEntry **locationsBlockPtr = (LocationEntry**)(0x0208B188);
PathEntry **pathsBlockPtr = (PathEntry**)(0x0208B190);
PathNode **nodesBlockPtr = (PathNode**)(0x0208B198);
u32 *locationsCount = (u32*)(0x0208B150);
u32 *pathsCount = (u32*)(0x0208B158);
u32 *nodesCount = (u32*)(0x0208B160);


int CheckForRumblePak() 
	{
		sysSetCartOwner(BUS_OWNER_ARM9);
		// First, check for 0x96 to see if it's a GBA game
		if (GBA_HEADER.is96h == 0x96)
			{
				// Check if WarioWare Twisted
				if ((GBA_HEADER.gamecode[0] == 'R') &&
					(GBA_HEADER.gamecode[1] == 'Z') &&
					(GBA_HEADER.gamecode[2] == 'W'))
						{
							WARIOWARE_ENABLE = 8;
							return 1;
						}
				else 
						{
							return 0;	
						}
			} 
		// Otherwise, normal check.
		else if (!isRumbleInserted())
			{
				for (int i = 0; i < 0xFFF; i++)
					{
						if (GBA_BUS[i] != 0xFFFD) 
							{
								return 0;
							}
					}
				return 2;
			}
			
		// Devkitpro Check... 
		else if (isRumbleInserted())
		{
			return 2;
		}
		
		else return 0;
	}


void Rumble(int Frames) 
	{
		if ((RumblePakType == 0) || (RumbleForce == 0)) return;
		
		if (RumblePakType == 1) WARIOWARE_PAK = 8; 	
		else if (RumblePakType == 2) RUMBLE_PAK = 2;
		
		if (RumbleForce == 1) RumbleTimer = Frames + 1;
		if (RumbleForce == 2) RumbleTimer = Frames * 20;
		
		return;
	}


void DoRumble()
	{
		if (RumbleTimer) 
		{
			RumbleTimer--;
			if (RumblePakType == 1) WARIOWARE_PAK = ((RumbleTimer % 2) ? 8 : 0); 	
			if (RumblePakType == 2) RUMBLE_PAK = ((RumbleTimer % 2) ? 2 : 0); 	
		}
		
		if (RumbleTimer == 1)
		{
			StopRumble();
			RumbleTimer = 0;
		}

	}


void StopRumble()
	{
		if (RumblePakType == 1) WARIOWARE_PAK = 0;
		else if (RumblePakType == 2) RUMBLE_PAK = 0;
	}


void SetupScreensForEnpgs()
	{
		videoSetMode(MODE_5_2D);
		videoSetModeSub(MODE_5_2D);
	}


int bgInitNerDS(int layer, BgType type, BgSize size, int mapBase, int tileBase, bool screen)
	{
		int ScreenOffset = 0; 
			 
		if (!screen)
			{
				BGCTRL[layer] = BG_MAP_BASE(mapBase) | BG_TILE_BASE(tileBase)
				| size | ((type == BgType_Text8bpp) ? BG_COLOR_256 : 0);
			}
		else 
			{
				BGCTRL_SUB[layer] = BG_MAP_BASE(mapBase) | BG_TILE_BASE(tileBase)
				| size | ((type == BgType_Text8bpp) ? BG_COLOR_256 : 0) ;
				ScreenOffset = 4;
			}

		memset(&bgState[layer + ScreenOffset], 0, sizeof(BgState) );

		bgSetScaleNerDS(layer + ScreenOffset, 1 << 8, 1 << 8);
		bgSetRotate(layer + ScreenOffset, 0);
			
		bgState[layer + ScreenOffset].type = type;
		bgState[layer + ScreenOffset].size = size;

		if (!screen) videoBgEnable(layer);
		else videoBgEnableSub(layer);

		bgState[layer + ScreenOffset].dirty = true;

		bgUpdate();

		return layer + ScreenOffset;
	}


void bgSetScaleNerDS(int id, s32 sx, s32 sy)
	{
		bgState[id].scaleX = sx;
		bgState[id].scaleY = sy;
		bgState[id].dirty = true;
	}


int DrawEnpgToScreen(int FileID, int Layer, int Offs, bool screen)
	{
		u8* ENPGFILEPTR = (u8*) allocFromGameHeap(66048);
		u8* ENPGPALPTR = (u8*) (ENPGFILEPTR);
		ENPGPALPTR += 0x10000;
		
		loadFileByExtId_LZ_Dest(FileID-0x83, ENPGFILEPTR);
		
		ENPGPALPTR[0] = 0x0;
		ENPGPALPTR[1] = 0x0;

		int id;
		if (!screen) 
			{
				id = bgInitNerDS(Layer, BgType_Bmp8, BgSize_B8_256x256, Offs*4, 0, false);
				MIi_CpuCopy32(ENPGPALPTR, BG_PALETTE, 512);
				MIi_CpuCopy32(ENPGFILEPTR, BG_GFX + Offs * 0x8000, 0x10000);
			}
		else 
			{
				id = bgInitNerDS(Layer, BgType_Bmp8, BgSize_B8_256x256, Offs*4,0, true);
				MIi_CpuCopy32(ENPGPALPTR, BG_PALETTE_SUB, 512);
				MIi_CpuCopy32(ENPGFILEPTR, BG_GFX_SUB + Offs * 0x8000, 0x10000);
			}
		
		freeToGameHeap(ENPGFILEPTR);
		return id;
	}


void CopyEnpgToScreen(int FileID, int Layer, int Offs, bool screen)
	{
		u8* ENPGFILEPTR = (u8*) allocFromGameHeap(66048);
		u8* ENPGPALPTR = (u8*) (ENPGFILEPTR);
		ENPGPALPTR += 0x10000;
		
		loadFileByExtId_LZ_Dest(FileID-0x83, ENPGFILEPTR);
		
		ENPGPALPTR[0] = 0x0;
		ENPGPALPTR[1] = 0x0;

		if (!screen) 
			{
				MIi_CpuCopy32(ENPGPALPTR, BG_PALETTE, 512);
				MIi_CpuCopy32(ENPGFILEPTR, BG_GFX + Offs * 0x8000, 0x10000);
			}
		else 
			{
				MIi_CpuCopy32(ENPGPALPTR, BG_PALETTE_SUB, 512);
				MIi_CpuCopy32(ENPGFILEPTR, BG_GFX_SUB + Offs * 0x8000, 0x10000);
			}
		
		freeToGameHeap(ENPGFILEPTR);
	}


void copyVRAMTile(void* sptr, void* dptr, int width, int height, int dx, int dy, int sx, int sy)
	{
		for(int j = 0; j < height; j++) 
			{
				for(int i = 0; i < width/2; i++) ((u16*)dptr)[dx/2 + ((j + dy) * 128) + i] = ((u16*)sptr)[sx/2 + ((j + sy) * 128) + i];
			}
	}


void copyVRAMTileWithAlpha(void* sptr, void* dptr, int width, int height, int dx, int dy, int sx, int sy)
	{
		for(int j = 0; j < height; j++) 
			{
				for(int i = 0; i < width/2; i++) 
				{
					if (((u16*)sptr)[sx/2 + ((j + sy) * 128) + i]) ((u16*)dptr)[dx/2 + ((j + dy) * 128) + i] = ((u16*)sptr)[sx/2 + ((j + sy) * 128) + i];
					
				}
			}
	}


void changeTileAny(void* ptr, int x, int y, int t) 
	{ 
		setTileTable[0] = t;
		changeTile(ptr, x, y, 0); 
		setTileTable[0] = 0;
	}


void FadeOut(s8 screen, s8 start, int speed)
	{
		for (s8 brightness=start;brightness>=-32;brightness-=speed) 
			{
				setBrightness(screen, brightness/2);
				waitForVblankIrqless();
			}
		
		setBrightness(screen, -16);
		return;
	}

void FadeIn(s8 screen, s8 start, int speed)
	{
		for (s8 brightness=start*2;brightness<=0;brightness+=speed) 
			{
				setBrightness(screen,brightness/2);
				waitForVblankIrqless();
			}
			
		setBrightness(screen, 0);
		return;
	}


void WaitFrames(int Frames)
	{
		for (int i=0; i<Frames; i++)
		{
			waitForVblankIrqless();
		}
	}


void SetLastSaveSlot()
	{
		for (u8 i = 0; i<3; i++)
			{
					loadSave2(i, 0, &saveData);
					if (saveData.inited)
					{
						if (saveData.header==0x31303037) lastslot=i;
					}
			}
		return;
	}


bool CheckSaveForBlankness()
	{
		for (u8 i = 0; i<3; i++)
			{
				loadSave2(i, 0, &saveData);
				if (saveData.inited) return false;
			}
		return true;
	}


bool CheckIfSecret()
	{
		for (int slot = 0; slot < 3; slot++)
			{	
				loadSave2(slot, 0, &saveData);
				
				int Completion = 0;
				
				int st = (u32)saveData.completion;
				if (st & 0x20) Completion++;
				if (st & 0x1) Completion++;
				if ((st & 0x780112) == 0x780112) Completion++;
				
				if (Completion != 3) return false;
			}
			
		return true;
	}


u8 GetWorldForTitlescreen() 
	{
		loadSave2(lastslot, 0, &saveData);
		return (u8)saveData.world;
	}


bool IsTouched()
	{
		char *touch = (char*)(0x0208B6A0);
		if (*touch) return true; else return false;
	}


u32 RNG(u32 randMax)
	{
		u32 seed = OS_GetTick();
		seed = (8253729 * seed + 2396403);
		return seed  % randMax;
	}


void AddSecondsToTimer(int amount)
	{
		int *timer = (int*)(0x020CA8B4);
		u16 *CurrentMusicTempo = (u16*)(0x02088B4C);

		if ((*timer / 4096) < 100 && (((*timer + 4096 * amount) / 4096) > 100)) 
			{
				NNS_SndPlayerSetTempoRatio((u32*)0x208FC08, 0x100); 
				*CurrentMusicTempo = 0x100;
			}

		if (((*timer + 4096*amount) / 4096) > 999) *timer = 4091904;
		else *timer += 4096 * amount;
	}


bool eventIsActivated(u32 id)
	{
		if (id == 0)
			return false;

		u64 *activatorBitmask = (u64*)(0x0208AF3C);
		return (*activatorBitmask >> (id-1)) & 1;
	}

	
void activateEvent(u32 id)
	{
		u64 *activatorBitmask = (u64*)(0x0208AF3C);
		*activatorBitmask |= 1LL << (id-1);
	}
	
	

void SetTilesetTo(int NCGFile, int NCLFile, int PNLFile, int CHKFile, int id)
	{
		u32* Map16 = *((u32**)0x20CAD30);
		u32* CurrentTileset = (u32*)0x20CACBC;
		u32* levelTilemapPointers = *((u32**)0x2085A94);
		u32* tilesetBehaviorsPtr = *((u32**)0x20CAD28);
		
		MIi_CpuClear32(0, tilesetBehaviorsPtr, 0x1400);
		u32* CHKPtr = (u32*)loadFileByExtId(CHKFile);					
		MI_DmaCopy32(3, CHKPtr, tilesetBehaviorsPtr, 0x1400);				// Reloading collisions
		freeToCurrentHeapIfNotNull(CHKPtr);			

		u32* PNLPTR = (u32*)loadFileByExtId(PNLFile);	
		MI_DmaCopy32(3,PNLPTR, (u16*)Map16 + 0x400, 0x1800);				// Reloading map16
		freeToCurrentHeapIfNotNull(PNLPTR);			
		
		loadFileByExtId_LZ_Dest(NCGFile, G2_GetBG2CharPtr()+0xC00);			// Reloading graphics
		
		u32* NCLPTR = (u32*)loadFileByExtId_LZ(NCLFile);					// Reloading palette
		DC_FlushRangeOrAll(NCLPTR, 0x400);									
		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt(NCLPTR, 0x4400, 0x800);
		GX_EndLoadBGExtPltt();
		freeToCurrentHeapIfNotNull(NCLPTR);
		
		*CurrentTileset = id;
		loadBGDatFile(ptrToLevelActor, levelTilemapPointers);		// Reloading objects
	}


void SetTilesetToID(int id)
	{
		u32* TS_NCG_TBL = (u32*)0x20C9584;
		u32* TS_NCL_TBL = (u32*)0x20C9B74;
		u32* TS_PNL_TBL = (u32*)0x20C9DD4;
		u32* TS_CHK_TBL = (u32*)0x20C8224;
		
		int NCGFile = (int)(*(TS_NCG_TBL + ((id << 2))/4));					// Grabbing file IDs from tables (table address + id << 2)
		int NCLFile = (int)(*(TS_NCL_TBL + ((id << 2))/4));
		int PNLFile = (int)(*(TS_PNL_TBL + ((id << 2))/4));
		int CHKFile = (int)(*(TS_CHK_TBL + ((id << 2))/4));
		
		SetTilesetTo(NCGFile, NCLFile, PNLFile, CHKFile, id);
	}


void SetTopBgTo(int NCGFile, int NCLFile, int NSCFile, int Parallax)
	{
		loadFileByExtId_LZ_Dest(NCGFile, G2_GetBG3CharPtr()+0x1000);			// Reloading NCG
		
		loadFileByExtId_LZ_Dest(NCLFile, G2_GetBG2ScrPtr());					// Reloading NCL
		DC_FlushRangeOrAll(G2_GetBG2ScrPtr(), 0x400);									
		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt(G2_GetBG2ScrPtr(), 0x7000, 0x400);
		GX_EndLoadBGExtPltt();
		
		loadFileByExtId_LZ_Dest(NSCFile, G2_GetBG3ScrPtr());					// Reloading NSC
		
		setupBgParallax(ptrToLevelActor, 0);
		setupTopBgParallax(ptrToLevelActor, 0, Parallax);
	}


void SetTopBgToID(int id)
	{
		u32* FG_NCG_TBL = (u32*)0x20C96B4;		
		u32* FG_NCL_TBL = (u32*)0x20C9CA4;
		u32* FG_NSC_TBL = (u32*)0x20C97E4;	
		
		int NCGFile = (int)(*(FG_NCG_TBL + ((id << 2))/4));						// Grabbing file IDs from tables (table address + id << 2)
		int NCLFile = (int)(*(FG_NCL_TBL + ((id << 2))/4));
		int NSCFile = (int)(*(FG_NSC_TBL + ((id << 2))/4));		
		int ParallaxID = id;
		
		SetTopBgTo(NCGFile, NCLFile, NSCFile, ParallaxID);
	}


void SetBottomBgTo(int NCGFile, int NCLFile, int NSCFile, int Parallax)
	{		
		loadFileByExtId_LZ_Dest(NCGFile, G2_GetBG1CharPtr()+0x2400);			// Reloading NCG
		
		loadFileByExtId_LZ_Dest(NCLFile, G2_GetBG2ScrPtr());					// Reloading NCL
		DC_FlushRangeOrAll(G2_GetBG2ScrPtr(), 0x400);									
		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt(G2_GetBG2ScrPtr(), 0x7400, 0x400);
		GX_EndLoadBGExtPltt();
		
		u32* nscptr = (u32*)allocFromHeap2(*((u32**)0x0208B730),0x2000,0xFFFFFFFC);		// Reloading NSC
		loadFileByExtId_LZ_Dest(NSCFile, nscptr);	
		
		u32* VRAMPtr = G2_GetBG1ScrPtr();
		
		int Position0 = 0;
		int Position1 = 0x20;
		int Position2 = 0x800;
		int Position3 = 0x820;
		
		for (int i = 0; i < 0x20; i++)											// The bottom NSC has to be specially setup too 
			{
				u32* srcptr = nscptr + ((Position0 << 1) / 4);
				MIi_CpuCopy16(srcptr, VRAMPtr, 0x40);
				
				srcptr = nscptr + ((Position1 << 1) / 4);
				MIi_CpuCopy16(srcptr, VRAMPtr+0x200, 0x40);
				
				srcptr = nscptr + ((Position2 << 1) / 4);
				MIi_CpuCopy16(srcptr, VRAMPtr+0x400, 0x40);	
				
				srcptr = nscptr + ((Position3 << 1) / 4);
				MIi_CpuCopy16(srcptr, VRAMPtr+0x600, 0x40);	

				VRAMPtr += 0x10;
				Position0 += 0x40;
				Position1 += 0x40;
				Position2 += 0x40;
				Position3 += 0x40;		
			}

		freeToHeap((u32*)(*(u32*)0x0208B730), (u32*)nscptr);
		
		setupBgParallax(ptrToLevelActor, 0);
		setupBottomBgParallax(findActorByType(303, 0), 0, Parallax);
	}


void SetBottomBgToID(int id)
	{
		u32* BG_NCG_TBL = (u32*)0x20C9454;
		u32* BG_NCL_TBL = (u32*)0x20C9A44;
		u32* BG_NSC_TBL = (u32*)0x20C9914;

		int NCGFile = (int)(*(BG_NCG_TBL + ((id << 2))/4));							// Grabbing file IDs from tables (table address + id << 2)
		int NCLFile = (int)(*(BG_NCL_TBL + ((id << 2))/4));
		int NSCFile = (int)(*(BG_NSC_TBL + ((id << 2))/4));	
		int ParallaxID = id;
		
		SetBottomBgTo(NCGFile, NCLFile, NSCFile, ParallaxID);
	}


void PlayMusic(int ID, int unk)
	{
		setSoundIncrementFlag();
		LoadMusicFile(ID, unk);
		PlayMusicFile(ID, unk);
		return;
	}


u16 getNodeIndexAdjustmentValue()
    {
        // Find the path starting-node adjustment value:
        // The game increments each path's startNodeIdx value
        // when the level is loaded, all by the same amount.
        // This amount (usually between 0 and 15 or so) varies
        // by level, and we can't figure out how to determine it.
        // So we simply find the minimum startNodeIdx value, and
        // assume that that path must be 0 + adjustment. Then we
        // can subtract that from other nodes' startNodeIdx values
        // later.
        PathEntry* thisEntry = (*pathsBlockPtr);
        u16 startNodeAdjustment = 0xFFFF;
        for (u32 i = 0; i < *pathsCount; i++)
            {
                if (thisEntry->startNodeIdx < startNodeAdjustment)
                    startNodeAdjustment = thisEntry->startNodeIdx;
                thisEntry++;
            }
        return startNodeAdjustment;
    }


PathEntry* getPath(int pathID)
    {
        PathEntry* thisEntry = (*pathsBlockPtr);
        int thisPathID = thisEntry->pathID;
        while (thisPathID != pathID)
            {
                thisEntry++;
                thisPathID = thisEntry->pathID;
            }
        return thisEntry;
    }


bool positionInLocation(int locationID, Vec3* position)
	{
        if (!locationID) return false;

        Rect2D* rect = &temporaryRect;

        LocationEntry* location = getLocationRect(locationID, rect);
        if (!location) return false;

        return (   (position->x >= rect->x)
                && (position->y <= rect->y)
                && (position->x <= rect->x + rect->width)
                && (position->y >= rect->y - rect->height));
	}
	
void SwapASwar(int IDToSwap, int file)
	{
		u32* SWARPointer = NNS_SndArcGetFileAddress(*(NNS_SndArcGetWaveArcInfo(IDToSwap, 0xFF, (u32*)0x208FBB8)));
		
		if (SWARPointer == 0) return;
		
		u32 LinkedListNext = *(SWARPointer + 6);
		loadFileByExtId_Dest(file, SWARPointer, 0);
		*(SWARPointer + 6) = LinkedListNext;
		return;
	}
	
void SwapASbnk(int IDToSwap, int file)
	{
		u32* SBNKPointer = NNS_SndArcGetFileAddress(*(NNS_SndArcGetBankInfo(IDToSwap, 0xFF, (u32*)0x208FBB8)));
		
		if (SBNKPointer == 0) return;
		
		u32 LinkedListObjects[6] = {0, 0, 0, 0, 0, 0};
		u32 Address030 = *(SBNKPointer + 0xC);
		u32 Address034 = *(SBNKPointer + 0xD);
		
		for (int i = 0; i < 6; i++)
			{
				LinkedListObjects[i] = *(SBNKPointer + 6 + i);
			}
		
		loadFileByExtId_Dest(file, SBNKPointer, 0);
		
		for (int i = 0; i < 6; i++)
			{
				*(SBNKPointer + 6 + i) = LinkedListObjects[i];
			}		
			
		*(SBNKPointer + 0xC) = Address030;	
		*(SBNKPointer + 0xD) = Address034;

		return;
	}
	
void SwapASseq(int IDToSwap, int file)
	{
		u32* SSEQPointer = NNS_SndArcGetFileAddress(*(NNS_SndArcGetSeqInfo(IDToSwap, 0xFF, (u32*)0x208FBB8)));
		if (SSEQPointer == 0) return;
        loadFileByExtId_Dest(file, SSEQPointer, 0);
		return;
	}
	
void SwapASsar(int IDToSwap, int file)
	{
		u32* SSARPointer = NNS_SndArcGetFileAddress(*(NNS_SndArcGetSeqArcInfo(IDToSwap, 0xFF, (u32*)0x208FBB8)));
		if (SSARPointer == 0) return;
        loadFileByExtId_Dest(file, SSARPointer, 0);		
		return;
	}

int ParticlesForTile(u32 collisions)
    {
    	// PARTICLE TYPES
    	// 0: brick
    	// 1: stone
    	// 2: wood
    	// 3: ?-block
    	// 4: dark red?
    	// 5: red
    	// 6: used item
    	// 7: stone block with little brown spot in the middle
    	// [> 7]: brick

        if (collisions == 0x5500435) // Shell block
            return 3;

        if ((collisions & 0xF0000) == 0x80000) // Explodable
            {
                if ((collisions & 0xF) == 3) // Red block
                    return 5;
                if ((collisions & 0xF) == 0) // Used item block
                    return 6;
                return collisions & 0xF;
            }

        if ((collisions & 0xF0000) == 0x50000) // Question block
            return 3;
        if ((collisions & 0xF00000) == 0x100000) // Brick block
            return 0;

        return -1;
    }