#include "game.h"

u8 *saveslot = (u8*)(0x02088F28);
u8 lastslot = 0;
u8 slot = 0;

bool nsub_02012D98_main()	//Write the last slot into header
	{
		lastslot = *saveslot;
		slot = *saveslot;
		
		saveData.header=0x31303037; 
		saveSave(*saveslot, &saveData); 

		for (int i = 0; i<3; i++)
			{
				loadSave2(i, 0, &saveData);
				
				if (slot != i && saveData.inited) 
					{
						saveData.header=0x30303037;
						saveSave(i, &saveData); 
					}
			}
			
		loadSave2(slot, 0, &saveData);			
		return true;	
	}

bool repl_020D16F4_ov_09()	//Jump directly to saving if copying, change last slot, reset ts, set logo
	{
		asm("BL 0x02012F24 ");
		lastslot = *saveslot;
		causeSceneChange(4,0);
		return true;
	}

bool repl_020D0494_ov_09()	//Reset ts, change last slot, set logo
	{
		asm("BL 0x02012EE0 ");
		for (int i = 0; i<3; i++)
			{
					loadSave(i, &saveData);
					if (saveData.inited)
						{
							if (saveData.inited) 
								{
									lastslot=i;
									break;
								}
						}
			}
		causeSceneChange(4,0);
		return true;
	}
