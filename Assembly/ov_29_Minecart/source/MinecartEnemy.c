#include "game.h"
#include "registers.h"
#include "MinecartEnemy.h"
#include "Minecart.h" // for collision


int MinecartEnemy_loadFiles()
    {
        loadFileByExtId_3dModel(2693-0x83, 0); 
        return 1;
    }


void MinecartEnemy_setup(MinecartEnemy* cart)
    {

        u8* texture = (u8*) NNS_G3dGetTex((u32*)getPtrToLoadedFile(2693-0x83));

        getTextureParams(texture, 0, &cart->info.cartTex.texparam);
        getTexturePalBase(texture, 0, &cart->info.cartTex.palbase);

        getTextureParams(texture, 1, &cart->info.wheelTex.texparam);
        getTexturePalBase(texture, 1, &cart->info.wheelTex.palbase);

        cart->position.y += 0x8000;
        cart->position.z = 0;
        cart->info.intendedX = cart->position.x;
        cart->info.intendedY = cart->position.y;

        MinecartCommon_Setup(&cart->info, cart->spriteData);

        u32 MinecartEnemy_physics[] =
            {
                0x18000,				
                0xA000,					
                0x13000,
                0xC000,
                0x00D30004,
                0x0400,
                (u32)&MinecartEnemy_Collision,
            };

        initActivePhysics(&cart->activephysics, cart, MinecartEnemy_physics, 0);
        registerCollider(&cart->activephysics);
		
		cart->info.isSilent = 0;
    }


void MinecartEnemy_StateIdle(MinecartEnemy* cart)                     // State 0
    {
        if (!MinecartCommon_MoveOnGround(&cart->info, &cart->position, &cart->velocity, &cart->rotation))
            {
                MinecartCommon_InitiateJump(&cart->info, &cart->position, &cart->velocity, false, true);
                cart->state = 1;
            }
    }


void MinecartEnemy_StateFalling(MinecartEnemy* cart)                  // State 1
    {
        // This becomes true immediately for some reason...
        // and apparently it sets garbage path data, which'd be why it disappears immediately
        if (MinecartCommon_MoveDuringJump(&cart->info, &cart->position, &cart->velocity, &cart->rotation, false))
            cart->state = 2;
    }


void MinecartEnemy_StateLanding(MinecartEnemy* cart)                  // State 2
    {
        if (cart->info.landingBounceCounter == -1)
            {
                PlaySNDEffect(367, &cart->position);
                cart->info.doneFirstBounce = 0;
                cart->info.doneSecondBounce = 0;
                cart->info.isLanding = false;
                cart->state = 0;
            }

        MinecartEnemy_StateIdle(cart);
    }


void MinecartEnemy_StateDead(MinecartEnemy* cart)                     // State 3
    {
        cart->position.y -= 0x4000;
        cart->rotation.z += 0x200;
    }


void MinecartEnemy_JumpedOn(MinecartEnemy* cart)
    {
        cart->info.isDead = true;
        cart->state = 3;
    }


void MinecartEnemy_Collision(ActivePhysics* a, ActivePhysics* b)
    {
        MinecartEnemy* evilCart = (MinecartEnemy*)a->owner;
        Minecart* playerCart = (Minecart*)b->owner;

        if (playerCart->ClassID == 0x137 && (b == &playerCart->activephysics2))
            MinecartEnemy_JumpedOn(evilCart);
        else
            EnemyCollisionCallback(a, b);
    }


bool MinecartEnemy_beforeDraw(MinecartEnemy* cart)
    {
        return 1;
    }


void MinecartEnemy_draw(MinecartEnemy* cart)
    {
        Vec3 Scale;
        Vec3 Translate1;
        Vec3 Translate2;
        Vec3 Offset;

        Scale.x = 0x30000;
        Scale.y = 0x1D41D;
        Scale.z = 0;

        Translate1.x = 0x18000;
        Translate1.y = 0x1141D;

        Scale.z = Translate1.z = Translate2.x = Translate2.y = Translate2.z = 0;
        Offset = Translate2;

        DrawGenericMinecartPoly(cart->info.cartTex, cart->position, 0x800, 0x800, Scale,
                         Translate1, Translate2, Offset, cart->rotation.z, 0, cart->info.goingBackwards, false);

        Scale.x = Scale.y = 0xD000;
        Translate2.x = Translate2.y = 0x6800;
        Translate2.z = -0x1000;
        Offset.x = 0x7000;
        Offset.y = -0x7000;

        DrawGenericMinecartPoly(cart->info.wheelTex, cart->position, 0x200, 0x200, Scale,
                        Translate1, Translate2, Offset, cart->rotation.z,
                        cart->info.wheelRot, false, false);

        Offset.x = 0x1B000;

        DrawGenericMinecartPoly(cart->info.wheelTex, cart->position, 0x200, 0x200, Scale,
                        Translate1, Translate2, Offset, cart->rotation.z,
                        cart->info.wheelRot + cart->info.secondWheelRotAddition, false, false);
    }


void* MinecartEnemy_dtor(MinecartEnemy* cart)
    {
        cart->vtable = (u32*)0x20C4F04;

        Vec3 dtor = {0, 0, 0};
        Vec2 dtor2 = {0, 0};

        cart->Unk4Vec2 = dtor2;
        cart->Unk3Vec2 = dtor2;
        cart->DeleteDist = dtor2;
        cart->DrawDist = dtor2;
        cart->ExistDist = dtor2;
        cart->unkv13 = dtor;
        cart->unkv12 = dtor;
        cart->unkv11 = dtor;

		UnregisterActivePhysics(&cart->activephysics);

        return cart;
    }


void* MinecartEnemy_dtorFree(MinecartEnemy* cart)
    {
        MinecartEnemy_dtor(cart);	
        freeToGameHeap(cart);
        return cart;
    }