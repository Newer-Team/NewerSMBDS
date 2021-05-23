#include "game.h"
#include "BootDisplay.h"
#include "VariousFunctions.h"
#include "Scene8.h"

extern bool displayedboot;
extern u8 lastslot;
extern s32 ForcePlayerPowerupTo;

u8 *SceneParam = (u8*)(0x2088F38);
u8 *SceneID = (u8*)(0x203BD30);

extern WSEL* ThisWSEL; 

extern bool IsSpecialLevel;
extern bool ControlLock;
extern bool TouchscrControlLock;
extern int TitleScrNumber;
#define scene35Flag (*((u32*)0x02085A20))

const u8 TSCWorlds[8] = {7, 1, 2, 3, 4, 5, 6, 7};
const u8 TSCLevels[8] = {1, 0, 0, 0, 0, 5, 1, 7};
const u8 TSCAreas[8] = {2, 1, 0, 0, 0, 0, 0, 0};

void hook_02013238()
{       
    if (*SceneParam == 0 && *SceneID == 4 && displayedboot == false)                            
    {   
        BootScreenCredit();
        causeSceneChangeWrapper(4,0);   
        return;
    }

    if (*SceneParam == 100 && *SceneID == 100 && displayedboot)                 // Titlescreen
    {   
        int *wrld = (int*)(0x02088BFC);
        int *lvl = (int*)(0x02085A9C);
        
        *wrld = 1;
        *lvl = 1;
    
        bool secret =  CheckIfSecret();
        int world = GetWorldForTitlescreen();
        
        TitleScrNumber = 1 + world;
        if (secret) TitleScrNumber+=10;
        IsSpecialLevel = true;
        ControlLock = true;
        
        loadLevel(0xD, false, TSCWorlds[world], TSCLevels[world], TSCAreas[world], 0, 1, 0, 1, 1, 0xC, 1);
        scene35Flag = 2;
    }

    if (*SceneID == 12)                                 // Credits
    {
        ForcePlayerPowerupTo = 1;
        int *player = (int*)(0x02085A50);
        
        IsSpecialLevel = true;
        loadLevel(0xD, false, 6, 5, 2, 0, 1, *player, !(*player), 1, 0, 1);
    }
        
    if (*SceneParam == 140 && *SceneID == 100)                                  // Opening
    {
        int *wrld = (int*)(0x02088BFC);
        int *lvl = (int*)(0x02085A9C);
        
        *wrld = 1;
        *lvl = 1;
        
        IsSpecialLevel = true;
        ControlLock = true;
        loadLevel(0xD, false, 3, 4, 2, 0, 1, 0, 1, 1, 0, 1);
    }           
    return;
}

