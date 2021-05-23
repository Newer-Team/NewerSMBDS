#include <nds.h>
#include "actorSpawner.h"
#include "game.h"
#include "VariousFunctions.h"

void ActorSpawner_doSpawn(ActorSpawner* as)
{
    Vec3 pos = as->position;
    pos.y -= 0x10000;       // Origin pos: Bottom-left of sprite

    pos.x += (s8)as->settings->offsetX * 0x1000;
    pos.y += (s8)as->settings->offsetY * 0x1000;
    
    if (as->sActor) 
    {
        EnemyClassObject* ptr = (EnemyClassObject*)createActor(as->settings->classID & 0x1FF, as->settings->spriteData, &pos, 0, 0, 0);
        
        if (ptr == 0) 
        {
            Base__deleteIt(as);
            return;
        }
        
        ptr->EventFieldID1 = as->settings->event1;
        ptr->EventFieldID2 = as->settings->event2;
        
        ptr->EventField2ID1 = as->settings->event1;
        ptr->EventField2ID2 = as->settings->event2;
        
        int dist;
        
        if ((as->settings->classID >> 12) == 0xF)
            dist = 0x5000;
        else
            dist = 1000 + ((as->settings->classID >> 12) * 100);
       
        if (dist != 0)
        {  
            ptr->ExistDist.x = dist;
            ptr->ExistDist.y = dist;
            ptr->DrawDist.x = dist;
            ptr->DrawDist.y = dist;
        }
    }
    
    if (((as->settings->classID) & 0x1FF) == 0x61) asm("STR     R0, [R0,#0x3FC]");
        
        
    pos.x += (s8)as->settings->particleOffsetX * 0x1000;
    pos.y += (s8)as->settings->particleOffsetY * 0x1000;
        
    if (as->sParticles)
        
        SpawnParticle(as->settings->particleID, &pos);
    if (as->sSFX)
        PlaySNDEffect(as->settings->sfxID, &as->position);
        
    as->actorSpawned = true;
}

void ActorSpawner_setup(ActorSpawner* as)
{   
    as->actorSpawned = false;
    
    as->ExistDist.x = 5000;
    as->ExistDist.y = 5000;
    
    u16 assID = (as->spriteData & 0XFF0000) >> 16;
    u32* pointerToBlock14 = (u32*)0x0208B19C;
    as->settings = (ActorSpawnerSettigs*)(*pointerToBlock14 + 16 + assID*16);
    
    as->sActor = (as->spriteData & 0x1000) == 0;
    as->sParticles = (as->spriteData & 0x2000) == 0;
    as->sSFX = (as->spriteData & 0x4000) == 0;
    as->sFirstTick = (as->spriteData & 0x8000) == 0;
    
    as->event = as->spriteData>>24;
    as->eventWasActive = false;
    as->spawnDelay = as->spriteData & 0xFFF;
    as->timer = 0;
}

void ActorSpawner_update_state0(ActorSpawner* as)
{
    if (as->event == 0 && as->spawnDelay == 0)
        return;
    
    if (as->spawnDelay == 0)
    {
        if (eventIsActivated(as->event) && as->actorSpawned)
            return;
        
        if (!eventIsActivated(as->event) && as->actorSpawned)
            as->actorSpawned = false;
        
        if (eventIsActivated(as->event) && !as->actorSpawned)
            ActorSpawner_doSpawn(as);
    }
    else
    {
        bool eventActive;
        if (as->event != 0)
            eventActive = eventIsActivated(as->event);
        else
            eventActive = true;
            
        if (as->eventWasActive && !eventActive)
            as->timer = 0;
            
        
        if (eventActive)
        {
            if (as->sFirstTick && !as->eventWasActive)
                ActorSpawner_doSpawn(as);
                
            if (as->timer > as->spawnDelay)
            {
                ActorSpawner_doSpawn(as);
                as->timer = 0;
            }   
            as->timer++;
        }
            
        as->eventWasActive = eventActive;   
    }
}