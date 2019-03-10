/* ************************************************************************** */
/** Descriptive File Name
@ Author Richard Wall
@ Date April 30, 2016
@ Revised December 10, 2016
@Company Digilent
@File Name PICmx370.c
 * 
@Summary
Definition of constants and macro routines
 * 
@Description
The #define statements and macro C code provide high level access to the
"Trainer" boards switches, push buttons, and LEDs.
*/
/* ************************************************************************** */
/* This included file provides access to the peripheral library functions and
must be installed after the XC32 compiler. See
http://ww1.microchip.com/downloads/en/DeviceDoc/32bitPeripheralLibraryGuide.pdf
http://www.microchip.com/SWLibraryWeb/product.aspx?product=PIC32%20Peripheral%20Library
*/
#include "hardware.h"
//#include "switches.h"
#include <plib.h>
// *****************************************************************************
/**
@Function 
void Hardware_Setup(void);
 * 
@Summary
Initializes PIC32 pins commonly used for IO on the Trainer processor
board.
 * 
@Description
Initializes PIC32 digital IO pins to provide functionality for the
switches, push buttons, and LEDs
 * 
@Precondition
"config_bits* must be included in the project
 * 
@Parameters
None
 * 
@Returns
None
 * 
@Remarks
* Returned error flag indicates the value of either x or y is out of
* range 0 through 15.
*/
void Hardware_Setup(void)
{
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Statement configure cache, wait states and peripheral bus clock
* Configure the device for maximum performance but do not change the PBDIV
* Given the options, this function will change the flash wait states, RAM
* wait state and enable prefetch cache but will not change the PBDIV.
* The PBDIV value is already set via the pragma FPBDIV option above..
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Allow RA0, RA1, RA4 and RA5 to be used as digital IO
SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
ALL_DIGITAL_IO(); /* Set all IO pins to digital */
SET_MIC_ANALOG(); /* Set microphone input to analog */
SET_POT_ANALOG(); /* Set ANALOG INPUT CONTROL pot input to analog */
Set_All_LEDs_Output(); /* Configure all Basys MX3 LED0 – LED7 as outputs */
Set_All_LEDs_Off(); /* Set all Basys MX3 LED0 – LED7 off */
Set_All_Switches_Input(); /* Configure all Basys MX3 slide switches as inputs */
Set_All_PBs_Input(); /* Configure all Basys MX3 push buttons as inputs */
} /* End of hardware_setup */
/**
@Function
unsigned int Switch2Binary( void )
@Summary
Generates an unsigned integer value from the switch settings using binary
weighting. The Basys MX3 slide switches are initialized as inputs to a
disparate port assignments. This function collects all the switch
settings into a single variable.
*/
unsigned int switch2Binary(void)
{
int value;
value = ((int) SW0()) << 0;
value += ((int) SW1()) << 1;
value += ((int) SW2()) << 2;
value += ((int) SW3()) << 3;
value += ((int) SW4()) << 4;
value += ((int) SW5()) << 5;
value += ((int) SW6()) << 6;
value += ((int) SW7()) << 7;
return value;
}