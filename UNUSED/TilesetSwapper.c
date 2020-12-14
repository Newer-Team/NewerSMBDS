#include "game.h"
#include "files.h"

u32 Pa0TilesetTable[] = 
	{
		FOLDER_ZB_TILESETS_PA0_MAIN - 0x83,
		FOLDER_ZB_TILESETS_PA0_UNDERGROUND - 0x83,
		FOLDER_ZB_TILESETS_PA0_LAVA - 0x83,
		FOLDER_ZB_TILESETS_PA0_SNOW - 0x83,
		FOLDER_ZB_TILESETS_PA0_FREEZEFLAME - 0x83,
		0
	};
	
u32 Pa1TilesetTable[] = 
	{
		FOLDER_ZB_TILESETS_PA1_NOHARA - 0x83,
		0
	};
	
u32 Pa2TilesetTable[] = 
	{
		FOLDER_ZB_TILESETS_PA2_SUBNOHARA - 0x83,
		0
	};

void nsub_020A16A8_ov_00()
	{
		u8 **PtrToBlock1 = (u8**)(0x208B168);
		
		int Pa0Nr = *(*PtrToBlock1 + 0x20);
		int Pa2Nr = *(*PtrToBlock1 + 0x21);
		
		int Pa0NCGFile = Pa0TilesetTable[Pa0Nr] + 2;
		int Pa2NCGFile = Pa2TilesetTable[Pa2Nr] + 2;
		loadFileByExtId_LZ_Dest(Pa0NCGFile, (u16*)0x6000000);
		loadFileByExtId_LZ_Dest(Pa2NCGFile, (u16*)0x600A000);
	
		int Pa0NCLFile = Pa0TilesetTable[Pa0Nr] + 6;
		
		u8* ptr = (u8*) allocFromGameHeap(1024);					
		loadFileByExtId_LZ_Dest(Pa0NCLFile, ptr);	
		
		DC_FlushRangeOrAll(ptr, 0x400);							
		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt(ptr, 0x4000, 0x400);
		GX_EndLoadBGExtPltt();
		
		DC_FlushRangeOrAll(ptr, 0x400);								
		GX_BeginLoadOBJExtPltt();
		GX_LoadOBJExtPltt(ptr, 0x0, 0x400);
		GX_EndLoadOBJExtPltt();

		int Pa2NCLFile = Pa2TilesetTable[Pa2Nr] + 6;
							
		loadFileByExtId_LZ_Dest(Pa2NCLFile, ptr);	

		DC_FlushRangeOrAll(ptr, 0x400);							
		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt(ptr, 0x4C00, 0x400);
		GX_EndLoadBGExtPltt();

		freeToGameHeap(ptr);		
		
		int Pa0PNLFile = Pa0TilesetTable[Pa0Nr] + 3;
		int Pa2PNLFile = Pa2TilesetTable[Pa2Nr] + 3;
		
		u32* Map16 = *((u32**)0x20CAD30);
		
		loadFileByExtId_Dest_WithMax(Pa0PNLFile, (u16*)Map16, 0x800);
		loadFileByExtId_Dest_WithMax(Pa2PNLFile, (u16*)Map16 + 0x1000, 0x1000);
		
		u32* tilesetBehaviorsPtr = *((u32**)0x20CAD28);
		
		u32* Jyotyu_CHK = (u32*)0x20C8484;
		int Pa0CHKFile = Pa0TilesetTable[Pa0Nr] + 1;
		int Pa2CHKFile = Pa2TilesetTable[Pa2Nr] + 1;
		
		loadFileByExtId_Dest_WithMax(Pa0CHKFile, Jyotyu_CHK, 3200);	
		loadFileByExtId_Dest(Pa2CHKFile, tilesetBehaviorsPtr + (0xC00 / 4), 0);
		
		int Pa0ANIMFile = Pa0TilesetTable[Pa0Nr];
		loadFileByExtId_LZ_Dest(Pa0ANIMFile, (u16*)ptrToAnimatedJyotyu);	
		loadFileByExtId_LZ_Dest(FILE_ZD_SPRITES_JYOTYU_BASIC_PARTICLES_NCG_LZ - 0x83 + (Pa0Nr * 2), (u16*)0x6408200);
		loadFileByExtId_LZ_Dest(FILE_ZD_SPRITES_JYOTYU_BASIC_SPRITE_BLOCKS_NCG_LZ - 0x83 + (Pa0Nr * 2), (u16*)0x6405E00);
		
		loadFileByExtId_3dModel(0x69F, 1);
		loadFileByExtId_Cached_intoObjPalette(0x69F, 0x400);
	}
	


	


	