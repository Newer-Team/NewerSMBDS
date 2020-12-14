#include <nds.h>
#include "game.h"
#include "VariousFunctions.h"
#include "OSReport.h"

void nds_printf(u16 Color[2], u16* screenPtr, const char* str, ...);
void GX_SetBankForSubBG(u32 Values);
void clearDebugScreen();
void SetupScreen();
void SetupGraphicsforOSReports();

int  ScreenOffs = 0;
bool StoppedPrinting;

#define BLACK 0x0000
#define WHITE 0xD000
#define GREEN 0xE000
#define RED 0xF000

u16** subScreenPtr = (u16**) 0x020859E8;
u16 MessageColor[2];
u16 ValueColor[2];
extern char CrashReasonString;


void hook_02013364()
	{
		SetupScreen(); 
	}
    
void SetupScreen()
	{
		SetupGraphicsforOSReports();
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
		
		MessageColor[0] = WHITE;
		MessageColor[1] = WHITE;
		ValueColor[0] = GREEN;
		ValueColor[1] = GREEN;
		StoppedPrinting = false;
		ScreenOffs = 0;
		
		OS_REPORT_NONE("Ready...");
	}
	
void hook_02004F24_main()
	{
		if ((*ButtonsPressed & SELECT) && StoppedPrinting)
		{
			StoppedPrinting = false;
			return;
		}
		
		if ((*ButtonsPressed & SELECT) && !StoppedPrinting)
		{
			StoppedPrinting = true;
			return;
		}
	}
	
void OS_REPORT_NUM(const char* Message, u32 Number)
	{
		if (StoppedPrinting) return;
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		
		char _Message[24];
		strncpy(_Message, Message, 24);
		
		if (ScreenOffs < 736)
		{
			nds_printf(MessageColor, *subScreenPtr + ScreenOffs, "%s", _Message);
			nds_printf(ValueColor, *subScreenPtr + ScreenOffs + strlen(_Message) + 1, "%d", Number);
			ScreenOffs += 32;
		}
		else
		{
			MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
			nds_printf(MessageColor, *subScreenPtr, "%s", _Message);
			nds_printf(ValueColor, *subScreenPtr + strlen(_Message) + 1, "%d", Number);
			ScreenOffs = 32;	
		}
		
		nocashPrint(_Message);
		nocashPrint1(" %r0%\n", Number);
	}
	
void OS_REPORT_HEXNUM(const char* Message, u32 Number)
	{
		if (StoppedPrinting) return;
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		
		char _Message[24];
		strncpy(_Message, Message, 24);
		
		if (ScreenOffs < 736)
		{
			nds_printf(MessageColor, *subScreenPtr + ScreenOffs, "%s", _Message);
			nds_printf(ValueColor, *subScreenPtr + ScreenOffs + strlen(_Message) + 1, "%x", Number);
			ScreenOffs += 32;
		}
		else
		{
			MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
			nds_printf(MessageColor, *subScreenPtr, "%s", _Message);
			nds_printf(ValueColor, *subScreenPtr + strlen(_Message) + 1, "%x", Number);
			ScreenOffs = 32;	
		}
		
		nocashPrint(_Message);
		nocashPrint1(" %r0%\n", Number);
	}

void OS_REPORT_ADDRESS_32(const char* Message, u32* Address)
	{
		if (StoppedPrinting) return;
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		
		char _Message[24];
		strncpy(_Message, Message, 24);
		
		if (ScreenOffs < 736)
		{
			nds_printf(ValueColor, *subScreenPtr + ScreenOffs + strlen(_Message) + 1, "0x%08x", *Address);
			ScreenOffs += 32;
		}
		else
		{
			MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
			nds_printf(ValueColor, *subScreenPtr + strlen(_Message) + 1, "0x%08x", *Address);
			ScreenOffs = 32;	
		}
		
		nocashPrint(_Message);
		nocashPrint1(" %r0%\n", *Address);
	}

void OS_REPORT_ADDRESS_16(const char* Message, u16* Address)
	{
		if (StoppedPrinting) return;
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		
		char _Message[24];
		strncpy(_Message, Message, 24);
		
		if (ScreenOffs < 736)
		{
			nds_printf(ValueColor, *subScreenPtr + ScreenOffs + strlen(_Message) + 1, "0x%04x", *Address);
			ScreenOffs += 32;
		}
		else
		{
			MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
			nds_printf(ValueColor, *subScreenPtr + strlen(_Message) + 1, "0x%04x", *Address);
			ScreenOffs = 32;	
		}
		
		nocashPrint(_Message);
		nocashPrint1(" %r0%\n", *Address);
	}

void OS_REPORT_ADDRESS_8(const char* Message, u8* Address)
	{
		if (StoppedPrinting) return;
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		
		char _Message[24];
		strncpy(_Message, Message, 24);
		
		if (ScreenOffs < 736)
		{
			nds_printf(ValueColor, *subScreenPtr + ScreenOffs + strlen(_Message) + 1, "0x%02x", *Address);
			ScreenOffs += 32;
		}
		else
		{
			MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
			nds_printf(ValueColor, *subScreenPtr + strlen(_Message) + 1, "0x%02x", *Address);
			ScreenOffs = 32;	
		}
		
		nocashPrint(_Message);
		nocashPrint1(" %r0%\n", *Address);
	}

void OS_REPORT_NONE(const char* Message)
	{
		if (StoppedPrinting) return;
		
		*subScreenPtr = (u16*)G2S_GetBG1ScrPtr();
		
		char _Message[24];
		strncpy(_Message, Message, 24);
		
		if (ScreenOffs < 736)
		{
			nds_printf(MessageColor, *subScreenPtr + ScreenOffs, "%s", _Message);
			ScreenOffs += 32;
		}
		else
		{
			MIi_CpuClear16(0x007F, *subScreenPtr, 0x800);
			nds_printf(MessageColor, *subScreenPtr, "%s", _Message);
			ScreenOffs = 32;	
		}

		nocashPrint(_Message);
		nocashPrint(" \n");
	}
    
void OS_PANIC_WITH_REASON(const char* Message)
    {
        strncpy(&CrashReasonString, Message, 64);
        OS_Panic();
    }
    
void SET_CRASH_REASON(const char* Message)
    {
        strncpy(&CrashReasonString, Message, 64);
    }