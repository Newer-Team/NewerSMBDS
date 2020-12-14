#include "game.h"
#include "keys.h"
#include "auto_consts_version.h"

extern "C" {
    void nds_printf(u16 something[2], u16* screenPtr, const char* str, ...);
    void showDebugScreen();
    void showDebugScreenSub();
}

#define BLACK 0x0000
#define WHITE 0xD000
#define GREEN 0xE000
#define RED 0xF000

int currentScene = 0;
u8* ptr = (u8*)0x02000000;
u32* crashreason = (u32*)0x2088F38;
int pos = 4;

char CrashReasonString[64] = "Crashed without reason set.";

void debugMenu()
{
    u16** topScreenPtr = (u16**) 0x020859EC;
    u16** subScreenPtr = (u16**) 0x020859E8;

    u16 something[2];

    something[0] = WHITE;
    something[1] = WHITE;

    myScanKeys();
    int keysDw = myKeysDown();

    u32* cardPulled = (u32*)0x02096B40;

    // Don't show Debug Menu if the Game Card disconnected
    if (*cardPulled != 0)
        currentScene = 3;

    // Go back to Debug Menu
    if (currentScene > 0 && currentScene != 3 && keysDw & KEY_B)
    {
        currentScene = 0;
        return;
    }

    // Debug Menu
    if (currentScene == 0)
    {
        nds_printf(something, *topScreenPtr+64,  "Whoops! NewerSML " PROJECT_VERSION "-EN crashed");

        nds_printf(something, *topScreenPtr+128, "   Please press A and take a    ");
        nds_printf(something, *topScreenPtr+160, "  screenshot of the crash dump  ");
        nds_printf(something, *topScreenPtr+192, "       shown. Send it to        ");
        nds_printf(something, *topScreenPtr+224, "       admin@newerteam.com      ");
        nds_printf(something, *topScreenPtr+256, "  along with a description of   ");
        nds_printf(something, *topScreenPtr+288, "    what you were doing when    ");
        nds_printf(something, *topScreenPtr+320, "       the game crashed.        ");
        nds_printf(something, *topScreenPtr+384, "           Thank you!           ");

        nds_printf(something, *topScreenPtr+672, " (A) Crash Dump  (B) RAM Viewer ");


        char* buildtimeStr = (char*) 0x02088BB4;
        nds_printf(something, *subScreenPtr, buildtimeStr);

        if (keysDw & KEY_A)
            currentScene = 1;
        else if (keysDw & KEY_B)
            currentScene = 2;
    }

    // Crash Dump
    else if (currentScene == 1)
    {
        showDebugScreen();
        nds_printf(something, *topScreenPtr+704, CrashReasonString);
        nds_printf(something, *topScreenPtr+672, " (B) Back");
        showDebugScreenSub();
    }

    // RAM Viewer
    else if (currentScene == 2)
    {
        u8* newptr = ptr;
        if(keysDw & KEY_UP) newptr+=1<<(pos*4);
        if(keysDw & KEY_DOWN) newptr-=1<<(pos*4);
        if(keysDw & KEY_LEFT) pos++;
        if(keysDw & KEY_RIGHT) pos--;
        if(keysDw & START) newptr = (u8*)0x9000000;
        if(pos > 7) pos = 0;
        if(pos < 0) pos = 7;

        nds_printf(something, *topScreenPtr + 33, "NewerSML " PROJECT_VERSION " RAM Viewer", ptr);
        nds_printf(something, *topScreenPtr + 65, "Current Address: 0x%08x", ptr);
        nds_printf(something, *topScreenPtr + 123+-pos, "^");

        nds_printf(something, *subScreenPtr+32, " (B) Back");

        ptr = newptr;

        for(int i = 0; i < 16*20; i++)
        {
            if(something[0] == GREEN)
                something[0] = something[1] = WHITE;
            else
                something[0] = something[1] = GREEN;

            nds_printf(something, *topScreenPtr + i*2+4*32, "%02x", *newptr++);
        }
    }

    // Card disconnected. No debug features.
    else if (currentScene == 3)
    {
        nds_printf(something, *topScreenPtr+288, "      NewerSML " PROJECT_VERSION " crashed     ");
        nds_printf(something, *topScreenPtr+320, " because the Game Card/SD Card  ");
        nds_printf(something, *topScreenPtr+352, "       was disconnected.        ");

        nds_printf(something, *topScreenPtr+416, " Reinsert the Game Card/SD Card ");
        nds_printf(something, *topScreenPtr+448, "     and reboot the system.     ");
    }
}
