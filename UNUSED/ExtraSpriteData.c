#include "game.h"
#include "Enemy.h"
#include "ExtraSpriteData.h"
#include "OSReport.h"
#include "OSPanic.h"


EnemyClassObject* createActorWithExtraArgs(u32 ClassID, u32 SpriteData, 
                                           Vec3* Position, S16Vec3* Rotation, int unk2, int unk3, u32 ExtraSpriteDataOffs)
{
    EnemyClassObject* Actor = (EnemyClassObject*)createActor(ClassID, SpriteData, Position, Rotation, unk2, unk3);
    
    if (ExtraSpriteDataOffs != 0)
    {    

        u32* PtrToBlock6 = (u32*)0x208B180;
        u32 ExtraSpriteDataPtr = ExtraSpriteDataOffs + *PtrToBlock6;
        Actor->rotation.padding = ExtraSpriteDataPtr >> 16;
        Actor->unkv5.padding = ExtraSpriteDataPtr & 0xFFFF;
    }

    return Actor;
}


u8* getExtraDataPtr(EnemyClassObject* Actor)
{
    u32 top = Actor->rotation.padding << 16;
    u32 bottom = Actor->unkv5.padding;
    u32 PointerToData = top | bottom;
    
    return ((u8*)PointerToData);   
}


u16 getExtraDataLength(EnemyClassObject* Actor)
{
    u16* extraDataPtr = (u16*)getExtraDataPtr(Actor)
    if (extraDataPtr == NULL)
        return 0;
    else
        return *extraDataPtr;
}


u8 getSpriteDataNybble(EnemyClassObject* Actor, u16 Num)
{
    Num -= 1;
    
    if (Num == 0)
        return Actor->EventFieldID1;
    if (Num == 1)
        return Actor->EventFieldID2;
    if (Num == 2)
        return Actor->EventField2ID1;    
    if (Num == 3)
        return Actor->EventField2ID2;    
    if (Num < 12)
        return (Actor->spriteData >> ((11 - Num) * 4)) & 0xF;

    u8* PointerToData = getExtraDataPtr(Actor);
    
    if (PointerToData == NULL)
        return 0;
    
    u16 Len = *((u16*)PointerToData);
    
    if (Len < Num - 11)
        return 0;

    PointerToData += ((Num - 11) / 2) + 2;
    
    u8 Byte = *PointerToData;
    
    if (Num % 2)
        return Byte >> 4;
    else
        return Byte & 0xF;
}


u64 getSpriteDataNybbles(EnemyClassObject* Actor, int StartNybble, int EndNybble)
{
    int NumOfNybbles = StartNybble - EndNybble + 1;

    u64 result = 0;
    
    if (NumOfNybbles > 16)
        NumOfNybbles = 16;
    if (NumOfNybbles < 0)
        NumOfNybbles = 0;
    
    for (int i = 0; i < NumOfNybbles; i++)
        result |= getSpriteDataNybble(Actor, StartNybble + i) << (i * 4);
        
    return result;
}