#include "game.h"
#include "registers.h"
#include "Enemy.h"
#include "OSReport.h"
#include "WavyLiquidChecks.h"
#include <math.h>

Vec3 WavyLiquidPosition = {0,0,0};
Vec3 WavyLiquidScale = {0,0,0};
int WavyLiquidFrame = -1;
int WavyLiquidSpaceBetween = -1;
bool WavyLiquidSpawned = false;

int WavyLiquidHeightAt(int x)
    {
        u32* WaterPosition = (u32*)0x20CAE0C;
        
        if (!WavyLiquidSpawned)
            return *WaterPosition;
        
        const float TAU = 6.28318530718;

        int waveTexTileHeight = WavyLiquidScale.y + 1;

        int topOfWave = WavyLiquidPosition.y + (waveTexTileHeight - 1) * 0x10000;
        float waveSize = WavyLiquidScale.x * 0x10000;

        int relativeX = x - WavyLiquidPosition.x;

        while (relativeX < 0)
            relativeX += waveSize + WavyLiquidSpaceBetween;
        while (relativeX > waveSize + WavyLiquidSpaceBetween) 
            relativeX -= waveSize + WavyLiquidSpaceBetween;

        if (relativeX > waveSize)
            return topOfWave - (waveTexTileHeight - 1) * 0x10000;

        float relativeNormalizedX = relativeX / waveSize;
        int waveHeight = topOfWave - ((cos(TAU * relativeNormalizedX) + 1) / 2) * (waveTexTileHeight * 0.75) * 0x10000;
        waveHeight -= (1 + cos(TAU * (relativeNormalizedX * 4 + (float)WavyLiquidFrame / 8 / 12))) * waveTexTileHeight * 0x800;
        waveHeight -= 0x2000; // Move the colls down 1/8 of a tile -- fixes some bugs

        return waveHeight;
    }
    
bool IsSpriteUnderWavyLiquid(int XPos, int YPos, u32* WaterPos)
    {
        int WavePos = WavyLiquidHeightAt(XPos);
        
        if (WaterPos != 0)
            *WaterPos = WavePos;
        
        return YPos <= WavePos;
    }