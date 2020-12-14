#ifndef OSREPORT_H
#define OSREPORT_H

void OS_REPORT_NUM(const char* Message, u32 Number);
void OS_REPORT_HEXNUM(const char* Message, u32 Number);
void OS_REPORT_ADDRESS_32(const char* Message, u32* Address);
void OS_REPORT_ADDRESS_16(const char* Message, u16* Address);
void OS_REPORT_ADDRESS_8(const char* Message, u8* Address);
void OS_REPORT_NONE(const char* Message);
void OS_PANIC_WITH_REASON(const char* Message);
#endif