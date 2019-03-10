/* ************************************************************************** */
/** Descriptive File Name
	@ Author Richard Wall
@ Date April 30, 2016
@ Revised December 10, 2016
@Company Digilent Inc.
@File Name hardware.h

@Summary 
Definition of constants and macro routines for the Basys MX3 processor board

@Description
The #define statements and macro C code provide high level access to the
Basys MX3 trainer boards switches, push buttons, and LEDs.

*/
/* ************************************************************************** */

/* Conditional inclusion prevents multiple definition errors */
#ifndef _HARDWARE_H_
#define _HARDWARE_H_
#ifndef _SUPPRESS_PLIB_WARNING /* Suppress plib obsolesce warnings */
#define _SUPPRESS_PLIB_WARNING
#endif
#ifndef _DISABLE_OPENADC10_CONFIGPORT_WARNING
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
#endif

/* Comment out the following define statement when programmer is NOT used to
* allow using BTNL and BTNU as user inputs. */
#define DEBUG_MODE /* Inputs from push buttons BTNL and BTNU are not useable */

/* This included file provides access to the peripheral library functions and
must be installed after the XC32 compiler. See
http://ww1.microchip.com/downloads/en/DeviceDoc/32bitPeripheralLibraryGuide.pdf and
http://www.microchip.com/SWLibraryWeb/product.aspx?product=PIC32%20Peripheral%20Library */

#include <plib.h>

/* The following definitions are for IO assigned on the Digilent Basys MX3
processor board. */
/* The ANSELx register has a default value of 0xFFFF; therefore, all pins that
* share analog functions are analog (not digital) by default. All pins are
* initially set be digital followed be setting A_POT for the ANALOG INPUT
* CONTROL and A_MIC for the microphone input back to being analog input pins.*/
#define ALL_DIGITAL_IO() (ANSELA=0,ANSELB=0,ANSELC=0,ANSELD=0,ANSELE=0,ANSELF=0,ANSELG = 0)
#define SET_MIC_ANALOG() ANSELBbits.ANSB4 = 1
#define SET_POT_ANALOG() ANSELBbits.ANSB2 = 1

/* Macros to configure PIC pins as inputs to sense switch settings */
/* BIT definitions are defined in port.h which is provided with the plib
Library. */
#define SW0_bit BIT_3 /* RF3 - 1<<3 */
#define SW1_bit BIT_5 /* RF5 - 1<<5 */
#define SW2_bit BIT_4 /* RF4 - 1<<4 */
#define SW3_bit BIT_15 /* RD15 - 1<<15 */
#define SW4_bit BIT_14 /* RD14 - 1<<14 */
#define SW5_bit BIT_11 /* RB11 - 1<<11 */
#define SW6_bit BIT_10 /* RB10 - 1<<10 */
#define SW7_bit BIT_9 /* RB9 - 1<<9 */

/* The following macro instructions set switches as inputs. */
#define Set_SW0_in() TRISFbits.TRISF3 = 1
#define Set_SW1_in() TRISFbits.TRISF5 = 1
#define Set_SW2_in() TRISFbits.TRISF4 = 1
#define Set_SW3_in() TRISDbits.TRISD15 = 1
#define Set_SW4_in() TRISDbits.TRISD14 = 1
#define Set_SW5_in() TRISBbits.TRISB11 = 1
#define Set_SW6_in() TRISBbits.TRISB10 = 1
#define Set_SW7_in() TRISBbits.TRISB9 = 1

/* The following macro instruction sets the processor pins for all 8 switch inputs */
#define Set_All_Switches_Input(); { Set_SW0_in(); Set_SW1_in(); Set_SW2_in();\
Set_SW3_in(); Set_SW4_in(); Set_SW5_in();\
Set_SW6_in(); Set_SW7_in(); }

/* The following macro instructions provide for reading the position of the 8 switches. */
#define SW0() PORTFbits.RF3
#define SW1() PORTFbits.RF5
#define SW2() PORTFbits.RF4
#define SW3() PORTDbits.RD15
#define SW4() PORTDbits.RD14
#define SW5() PORTBbits.RB11
#define SW6() PORTBbits.RB10
#define SW7() PORTBbits.RB9

/* Organize the SW bits into a unsigned integer */
/* Macro instructions to define the bit values for push button sensors */
#define BTNL_bit BIT_0 /* RB0 1 << 0 */
#define BTNR_bit BIT_8 /* RB8 1 << 8 */
#define BTNU_bit BIT_1 /* RB1 1 << 1 */
#define BTND_bit BIT_15 /* RA15 1 << 15 */
#define BTNC_bit BIT_0 /* RF0 1 << 1 */

/* See http://umassamherstm5.org/tech-tutorials/pic32-tutorials/pic32mx220-tutorials/internalpull-
updown-resistors */

/* Macro instructions to set the push buttons as inputs */
#define Set_BTNL_in() (TRISBbits.TRISB0 = 1, CNPDBbits.CNPDB0 = 1)

// #define Set_BTNL_in() TRISBbits.TRISB0 = 1
#define Set_BTNR_in() TRISBbits.TRISB8 = 1
#define Set_BTNR_out() TRISBbits.TRISB8 = 0
#define Set_BTNU_in() (TRISBbits.TRISB1 = 1, CNPDBbits.CNPDB1 = 1)

// #define Set_BTNU_in() TRISBbits.TRISB1 = 0
#define Set_BTND_in() TRISAbits.TRISA15 = 1
#define Set_BTND_out() TRISAbits.TRISA15 = 0
#define Set_BTNC_in() TRISFbits.TRISF0 = 1

/* single macro instruction to configure all 5 push buttons */
#ifndef DEBUG_MODE
#define Set_All_PBs_Input() (
Set_BTNL_in(),Set_BTNR_in(),Set_BTNU_in(),Set_BTND_in(),Set_BTNC_in() )
#else
#define Set_All_PBs_Input() ( Set_BTNR_in(),Set_BTND_in(),Set_BTNC_in() )
#endif

/* Macro instructions to read the button position values. 1 = button pressed */
/* Include BTNL and BTNU only if NOT in debug mode */
#ifndef DEBUG_MODE
#define BNTL() PORTBbits.RB0
#define BNTU() PORTBbits.RB1
#endif
#define BNTR() PORTBbits.RB8
#define BNTD() PORTAbits.RA15
#define BNTC() PORTFbits.RF0

/* Macros to define the PIC pin values for the board LEDs */
#define LED0_bit BIT_0 /* RA0 */
#define LED1_bit BIT_1 /* RA1 */
#define LED2_bit BIT_2 /* RA2 */
#define LED3_bit BIT_3 /* RA3 */
#define LED4_bit BIT_4 /* RA4 */
#define LED5_bit BIT_5 /* RA5 */
#define LED6_bit BIT_6 /* RA6 */
#define LED7_bit BIT_7 /* RA7 */
#define All_LED_bits 0xff /* Set all LEDs off

/* Macros to configure PIC pins as outputs for board LEDs */
#define Set_LED0_out() TRISAbits.TRISA0 = 0
#define Set_LED1_out() TRISAbits.TRISA1 = 0
#define Set_LED2_out() TRISAbits.TRISA2 = 0
#define Set_LED3_out() TRISAbits.TRISA3 = 0
#define Set_LED4_out() TRISAbits.TRISA4 = 0
#define Set_LED5_out() TRISAbits.TRISA5 = 0
#define Set_LED6_out() TRISAbits.TRISA6 = 0
#define Set_LED7_out() TRISAbits.TRISA7 = 0

/* Macro instruction to configure all 8 LED pins for outputs */
#define Set_All_LEDs_Output() TRISACLR = All_LED_bits

/* Macros to set board each LED on (1) or off (0) */
#define setLED0(a); {if(a) LATASET = LED0_bit; else LATACLR = LED0_bit;}
#define setLED1(a); {if(a) LATASET = LED1_bit; else LATACLR = LED1_bit;}
#define setLED2(a); {if(a) LATASET = LED2_bit; else LATACLR = LED2_bit;}
#define setLED3(a); {if(a) LATASET = LED3_bit; else LATACLR = LED3_bit;}
#define setLED4(a); {if(a) LATASET = LED4_bit; else LATACLR = LED4_bit;}
#define setLED5(a); {if(a) LATASET = LED5_bit; else LATACLR = LED5_bit;}
#define setLED6(a); {if(a) LATASET = LED6_bit; else LATACLR = LED6_bit;}
#define setLED7(a); {if(a) LATASET = LED7_bit; else LATACLR = LED7_bit;}
#define Set_All_LEDs_On() LATASET = All_LED_bits // Set all LEDs on
#define Set_All_LEDs_Off() LATACLR = All_LED_bits // Set all LEDs off

/* Macros to invert the output to the board LEDs */
#define invLED0() LATAINV = LED0_bit
#define invLED1() LATAINV = LED1_bit
#define invLED2() LATAINV = LED2_bit
#define invLED3() LATAINV = LED3_bit
#define invLED4() LATAINV = LED4_bit
#define invLED5() LATAINV = LED5_bit
#define invLED6() LATAINV = LED6_bit
#define invLED7() LATAINV = LED7_bit

/* Based upon setting in config_bits.h These directly influence timed
* events using the Tick module. They also are used for UART I2C, and SPI
* baud rate generation. */
#define XTAL (8000000UL) /* 8 MHz Xtal on Basys MX3 */
#define GetSystemClock() (80000000UL) /* Instruction frequency */
#define SYSTEM_FREQ (GetSystemClock())
#define GetCoreClock() (GetSystemClock()/2) /* Core clock frequency */
#define GetPeripheralClock() (GetSystemClock()/8) /* PCLK set for 10 MHz */
/* Used in core timer software delay */
#define CORE_MS_TICK_RATE (unsigned int) (GetCoreClock()/1000UL)
#endif /* End of _HARDWARE_H_ */

/* Declare Hardware setup for global access */
void Hardware_Setup(void);
unsigned int switch2Binary(void);