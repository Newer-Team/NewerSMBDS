#include "game.h"
#include "VariousFunctions.h"

void nsub_020A16A8_ov_00(int view)
	{
		int fileoffset = getUnk3ForView(view);

		if (fileoffset == 255)	loadFileByExtId_LZ_Dest(2643-0x83, (u16*)0x6000000);
		else loadFileByExtId_LZ_Dest(2738-0x83 +fileoffset, (u16*)0x6000000);

		if (fileoffset == 255) fileoffset = 3;
		
		loadFileByExtId_LZ_Dest(2744-0x83 +fileoffset, (u16*)ptrToAnimatedJyotyu);						
		
		u8* ptr = (u8*) allocFromGameHeap(1024);					
		loadFileByExtId_LZ_Dest(2750-0x83 +fileoffset, ptr);
	
		DC_FlushRangeOrAll(ptr, 0x400);									//Sprites palette
		GX_BeginLoadOBJExtPltt();
		GX_LoadOBJExtPltt(ptr, 0x0, 0x400);
		GX_EndLoadOBJExtPltt();
			
		DC_FlushRangeOrAll(ptr, 0x400);									//Tileset palette
		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt(ptr, 0x4000, 0x400);
		GX_EndLoadBGExtPltt();
		freeToGameHeap(ptr);
	
//		ptr = (u8*) allocFromGameHeap(1024);
//		loadFileByExtId_LZ_Dest(2725-0x83, ptr);		
		
//		DC_FlushRangeOrAll(ptr, 0x400);									//New Palette
//		GX_BeginLoadOBJExtPltt();
//		GX_LoadOBJExtPltt(ptr, 0x1000, 0x400);
//		GX_EndLoadOBJExtPltt();
//		freeToGameHeap(ptr);
		
		loadFileByExtId_LZ_Dest(2726-0x83 +fileoffset, (u16*)0x6405E00);		//Sprites
		
		loadFileByExtId_LZ_Dest(2732-0x83 +fileoffset, (u16*)0x6408200);		//Palette
		
		loadFileByExtId_Cached_intoObjPalette(0x69F, 0x400);					//Enemy Palette
	
		return;
	}
	


	


	