/* ************************************************************************** */
/** Descriptive File Name
Company: DigilentInc
File Name: config_bits.h
Summary: Set configuration bits for the PIC32MX370F512L processor.
Description: The configurations settings set the processor to have a core frequency of
80MHz and a peripheral clock speed of 10MHz.
*/
/* ************************************************************************** */
#ifndef _CONFIG_BIYS_H_ /* Guard against multiple inclusion */
#define _CONFIG_BIYS_H_
// PIC32MX370F512L Configuration Bit Settings
// 'C' source line config statements
//#include <xc.h>
// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7 // Shadow Register Set Priority Select
//(SRS Priority 7)
#pragma config PMDL1WAY = OFF // Peripheral Module Disable
// Configuration (Allow multiple
// reconfigurations)
#pragma config IOL1WAY = OFF // Peripheral Pin Select Configuration
// (Allow multiple reconfigurations)
// DEVCFG2
#pragma config FPLLIDIV = DIV_2 // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20 // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1 // System PLL Output Clock Divider
// (PLL Divide by 1)
// DEVCFG1
#pragma config FNOSC = PRIPLL // Oscillator Selection Bits (Primary
// Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF // Secondary Oscillator Enable
// (Disabled)
#pragma config IESO = OFF // Internal/External Switch Over
// (Disabled)
#pragma config POSCMOD = XT // Primary Oscillator Configuration
// (XT osc mode)
#pragma config OSCIOFNC = OFF // CLKO Output Signal Active on the
// OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_8 // Peripheral Clock Divisor (Pb_Clk is
// Sys_Clk/8)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor Selection
#pragma config WDTPS = PS1048576 // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF // Watchdog Timer Window Enable
// (Watchdog Timer in Non-Window Mode)
#pragma config FWDTEN = OFF // Watchdog Timer Enable (WDT Disabled
// (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WINSZ_25 // Watchdog Timer Window Size (Window
// Size is 25%)
// DEVCFG0
#pragma config DEBUG = OFF // Background Debugger Enable (Debugger
// is Disabled)
#pragma config JTAGEN = OFF // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1 // ICE/ICD Comm Channel Select
// (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF // Program Flash Write Protect (Disable)
#pragma config BWP = OFF // Boot Flash Write Protect bit
//(Protection Disabled)
#pragma config CP = OFF // Code Protect (Protection Disabled)
#endif /* _CONFIG_BIYS_H_ */
/* *********************************** End of File ******************************/