#ifndef _EXTRASPRITEDATA_H
#define _EXTRASPRITEDATA_H

EnemyClassObject* createActorWithExtraArgs(u32 ClassID, u32 SpriteData, Vec3* Position, S16Vec3* Rotation, int unk2, int unk3, u32 ExtraSpriteDataOffs);
u16 getExtraDataLength(EnemyClassObject* Actor);
u8* getExtraDataPtr(EnemyClassObject* Actor);
u8 getSpriteDataNybble(EnemyClassObject* Actor, u16 Num);
u64 getSpriteDataNybbles(EnemyClassObject* Actor, int StartNybble, int EndNybble);

#endif