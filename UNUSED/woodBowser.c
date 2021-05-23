#include <nds.h>
#include <math.h>

#include "woodBowser.h"
#include "game.h"
#include "sfx.h"
#include "VariousFunctions.h"

bool WoodBowser_loadFiles()
{
    loadFileByExtId_3dModel(2564-0x83, 0);  // Load Model
    loadFileByExtId_3dModel(2565-0x83, 0);  // Load Animation
    return 1;
}

bool WoodBowser_heapCreated(WoodBowser* wb)
{
    return modelAnim3d_setup(&wb->model, getPtrToLoadedFile(2653-0x83), getPtrToLoadedFile(2654-0x83), 0, 0, 0);
}

void WoodBowser_setup(WoodBowser* wb)
{   
    modelAnim3d_init(&wb->model, 0, 0, 0x2000, 0);
    
    wb->scale.x = 0x6000;
    wb->scale.y = 0x6000;
    wb->scale.z = 0x6000;
    
    wb->position.y -= 0x8000;
    
    wb->tippingVar = 0;
}

void WoodBowser_update_idle(WoodBowser* wb)
{   
    anim3d_update(&wb->model.anim);
    
    if (eventIsActivated(wb->settings & 0xFF))
        wb->state = 1;
}

void WoodBowser_update_fall(WoodBowser* wb)
{
    if (wb->rotation.x >= 0x4300)
        return;
    
    wb->tippingVar++;
    wb->rotation.x = wb->tippingVar*(wb->tippingVar*2);
    
    if (wb->rotation.x >= 0x4300)
    {
        wb->rotation.x = 0x4300;
        
        PlaySNDEffect(SE_OBJ_BLOCK_BREAK, 0);
        
        Vec3 pos;
        pos.x = wb->position.x - 0x10000;
        pos.y = wb->position.y + 0x10000;
        pos.z = 0;
        createActor(0xF1, 0x00200030, &pos, 0, 0, 0);
        pos.x += 0x18000;
        
        SpawnParticle(107, &pos);
        
        pos.x -= 0x8000;
        
        createActor(0xF1, 0x00200030, &pos, 0, 0, 0);
        pos.x += 0x10000;
        createActor(0xF1, 0x00200030, &pos, 0, 0, 0);
        
        wb->scale.x = 0;
    }
}

bool WoodBowser_beforeDraw(WoodBowser* wb)
{
    return 1;
}

void WoodBowser_draw(WoodBowser* cb)
{
    return;
}
