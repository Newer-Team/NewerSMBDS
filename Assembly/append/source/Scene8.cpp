#include "game.h"
#include "VariousFunctions.h"
#include "Scene8.h"
#include "sfx.h"

#include "overlays.h"

u8 *SceneParams = (u8*)(0x2088F38);

WSEL* ThisWSEL = 0;

void nsub_020CD4B0_ov_08()                      // Hook in the Scene 8 OnExecute
{
    switch (*SceneParams)
    {
        case 0: return;
        case 10: ExecuteWSEL(); break;
        case 30: ExecuteFSEL(); break;
    }
}
    
void nsub_020CD040_ov_08()                      // Hook in Scene 8 OnCreate
{
    loadOverlay(2, -1);
}
    
void nsub_020CD424_ov_08()                      // Hook in Scene 8 dtor
{
    unloadOverlayIfValidId(2, -1);
    freeToCurrentHeapIfNotNull(ThisWSEL);
    ThisWSEL = 0;
}
    
void hook_020DCFE4_ov_08()                      // Hook in the World Map OnExecute, pressing Select causes switch to WSEL
{
    u8 *menuopen = (u8*)(0x020EE398);
    u8 *JrOnMap = (u8*)(0x02088B64);
    u8 *switching = (u8*)(0x020931FC);
    u8 *levelenter = (u8*)(0x020931D8);
    u8 *sceneparam = (u8*)(0x02085974);

    bool selectPressed = GBA_BUTTON_PRESSED(GBA_BUTTON_SELECT);
    bool JrNotOnMap = *JrOnMap != 0xFF;
    bool menuNotOpen = !(*menuopen);
    bool notSwitchingWorlds = !(*switching);
    bool notEnteringLevel = !(*levelenter);
    bool notSwitchingScene = !(*sceneparam);

    if(selectPressed && JrNotOnMap && menuNotOpen && notSwitchingWorlds && notEnteringLevel && notSwitchingScene)
    {   
        if (!ThisWSEL) 
            ThisWSEL = (WSEL*)allocFromCurrentHeap(sizeof(WSEL), 0); 

        ThisWSEL->SelectionMade = 0;
        ThisWSEL->WorldID = *((int*)(0x02088BFC));
        ThisWSEL->pressedtimer = 0;
        ThisWSEL->opentimer = 0;
        ThisWSEL->held = false;
        ThisWSEL->InitDone = false;

        PlaySNDEffect(SE_SYS_DECIDE, 0);
        
        ThisWSEL->node = *((u8*)(0x02088C04));
        causeSceneChange(8, 10);
    }
}
    
void nsub_020CD6E8_ov_08()                      // Scene 8 Ondraw
{
    return;
}
    
void nsub_020CCCD4_ov_08()                      // No clue, but crashed stuff, so whatever.
{
    return;
}