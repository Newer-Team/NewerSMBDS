#include "bowserjr.h"

enum BowserJrAnimartions
{
    BowserJrAnimartion_Wait = 0,
    BowserJrAnimartion_Throw1_End = 1,
    BowserJrAnimartion_Throw1_Start = 2,
    BowserJrAnimartion_Throw1_Wait = 3,
    BowserJrAnimartion_Throw2_End = 4,
    BowserJrAnimartion_Throw2_Start = 5,
    BowserJrAnimartion_Throw2_Wait = 6,
    BowserJrAnimartion_Run = 7,
    BowserJrAnimartion_Down = 8,
    BowserJrAnimartion_Hit = 9,
    BowserJrAnimartion_Fire = 10,
    BowserJrAnimartion_Lose = 11,
    BowserJrAnimartion_Lose_1 = 12,
    BowserJrAnimartion_Lose_2 = 13,
    BowserJrAnimartion_Ready = 14,
    BowserJrAnimartion_Jump_Start = 15,
    BowserJrAnimartion_Jump1 = 16,
    BowserJrAnimartion_Jump2 = 17,
    BowserJrAnimartion_Jump_End = 18,
    BowserJrAnimartion_Guard = 19
};

// Returns whether the normal onExecute is run afterwards
bool BowserJr_ExecuteSubstate(BowserJr* jr)
{
    if (!jr->isFinalBoss)
        return true;

    // Idle until Bowser Into finishes
    switch (jr->finalBoss->jrState)
    {

    case 0:     // Wait before Intro
    {
        if (jr->jrStateSub == 0)
        {
            modelAnim3d_init(&jr->model, BowserJrAnimartion_Wait, 0, 0x800, 0);
            jr->jrStateSub = 1;
        }
        anim3d_update(&jr->model.anim);
        return false;
    }

    case 1:     // Intro
    {
        jr->finalBoss->jrState = 2;
        jr->jrStateSub = 0;
        jr->someTimer = 0;
        return true;
    }

    case 2:     // Fight
    case 3:     // Wait while shell cover
        return true;

    case 4:     // Dead. Shouldn't run anyways
        return false;

    default:    // Whoops
        OS_Panic();
        return false;

    }
}
