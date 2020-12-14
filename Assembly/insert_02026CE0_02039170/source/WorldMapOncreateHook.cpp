#include "game.h"
#include "sfx.h"
#include "VariousFunctions.h"

void hook_020D0338_ov_08()		// Hook in OnCreate. Reset stuff on the world map, reload SWAR for Luigi.
	{
		// if World = 4 or World = 2, dim world map.
		// Else, set to normal brightness.
		if ((*((int*)(0x02088BFC)) == 3) || (*((int*)(0x02088BFC)) == 1))  setBrightness(1, -5);
		else setBrightness(1, 0);
		
		// If Player == Luigi, reload the MENU swar.
		if (*((int*)(0x02085A50)) == 1) SwapASwar(36, 2797 - 0x83);
		return;
	}