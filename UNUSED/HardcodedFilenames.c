#include <nds.h>
#include "game.h"
#include "auto_consts_files.h"

// Nintendo, you are a funny bunch.

void nsub_02004DFC() {}

void hook_02004E60()
{
    u16* filesCount = (u16*)0x2085D00;
    u16* overlaysCount = (u16*)0x2085D04;

    *filesCount = NUM_FILES_ADJ;
    *overlaysCount = 131;
}

void nsub_02012744()
{
    // loadFileByExtId_Dest_WithMax(132 - 0x83 /*FILEADJ_0132_BUILDTIME*/, (u32*)0x02088BB4, 0x20);
}

/*

void hksf_0205F108(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205F108, R0 was 0");
}

void hksf_0205F128(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205F128, R0 was 0");
}




void hksf_0205EE6C(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205EE6C, R0 was 0");
}

u32 EXPECTED_FSFILE[] = {
    0x00000000,
    0x00000000,
    0x02096114,
    0x00000010,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000086,
    0x00F29000,
    0x0113B1C0,
    0x00F29030,
    0x0208FC28,
    0x00000030,
    0x00000030,
    0x00000000,
    0x00000000,
    0x00000000
};

void hkxsf_0205EE8C(int compareToHexThirty, u32* r6)
{

    u32* fsFilePtr = r6 + (0x34 / 4);

    for (int i = 0; i < (0x48 / 4); i++) {
        OS_REPORT_HEXNUM("fsfile ", fsFilePtr[i]);
        if (fsFilePtr[i] != EXPECTED_FSFILE[i])
            OS_PANIC_WITH_REASON("FSFile is wrong somewhere");
    }

    for(;;);

    if (compareToHexThirty == 0)
        OS_PANIC_WITH_REASON("At 0205EE8C, R0 was not 0x30 (it was 0)");
    if (compareToHexThirty != 0x30)
        OS_PANIC_WITH_REASON("At 0205EE8C, R0 was not 0x30");
}

void hksf_0205EEC8(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At hksf_0205EEC8, R0 was 0");
}

void hksf_0205EEE8(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205EEE8, R0 was 0");
}

void hksf_0205EF0C(int compare1, int compare2)
{
    if (compare1 != compare2)
        OS_PANIC_WITH_REASON("At 0205EF0C, R0 was not R1");
}

void hksf_0205EF40(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205EF40, R0 was 0");
}

void hksf_0205EF60(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205EF60, R0 was 0");
}

void hksf_0205EF84(int compare1, int compare2)
{
    if (compare1 != compare2)
        OS_PANIC_WITH_REASON("At 0205EF84, R0 was not R1");
}

void hksf_0205EFC8(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205EFC8, R0 was 0");
}

void hksf_0205EFE8(int compareToZero)
{
    if (compareToZero == 0)
        OS_PANIC_WITH_REASON("At 0205EFE8, R0 was 0");
}

void hksf_0205F00C(int compare1, int compare2)
{
    if (compare1 != compare2)
        OS_PANIC_WITH_REASON("At 0205F00C, R0 was not R1");
}
*/