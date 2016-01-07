
/* ########################################################################## */
/*
 * This file was created by www.DavesMotleyProjects.com
 *
 * This software is provided under the following conditions:
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *                                                                            */
/* ########################################################################## */


/* ===========================================================================*/
/*
 * FileName:      MSP430F5529LP_WDT.c
 *
 * This file provides functions for the Watchdog module in the
 * Texas Instruments MSP430F5529 Launchpad development board.
 *
 * Version 1.0
 *
 * Rev. 1.0, Initial Release
 *
 *                                                                            */
/* ===========================================================================*/

#include "MSP430F5529LP.h"
#include "MSP430F5529LP_WDT.h"


/******************************************************************************
   PUBLIC DEFINITIONS
******************************************************************************/


/******************************************************************************
   PUBLIC VARIABLES
******************************************************************************/


/******************************************************************************
   PRIVATE DEFINITIONS (static const)
******************************************************************************/

    // This defines the number of WDT timer callback functions that can be
    // registered at the same time. This value can be increased to accomodate
    // more timers as needed.

    #define MAX_WDT_TIMERS  3


/******************************************************************************
   PRIVATE FUNCTION PROTOTYPES (static)
******************************************************************************/


/******************************************************************************
   PRIVATE VARIABLES (static)
******************************************************************************/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  ---- READ ME - DEBUGGING INFO ----
//
//  There is a bug in CCS ver 6, where static (file scope) variables can not
//  be viewed in the debug watch window. It is bad programming style to make
//  these variables permanently global, so here is a middle-of-the-road
//  solution. If it is necesary to debug variables in this file, comment the
//  line "#define STATIC static", and uncomment the line "#define STATIC".
//  When you are done debugging, put it back the way it was.
//  #define STATIC
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define STATIC static


     STATIC WDT_Callback_t  WDT_timers[MAX_WDT_TIMERS];


/******************************************************************************
    Subroutine:     MSP430F5529LP_WDT_Initialize
    Description:    Initializes the watchdog timer in interval mode with a
                    1 second timeout interval.
    Inputs:         None
    Outputs:        None

******************************************************************************/
void MSP430F5529LP_WDT_Initialize(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // Restart the watchdog in the interval timer mode, running from ACLK at
    // 32.768 KHz, with a timeout interval of 1 second.
    WDTCTL = WDTPW + WDTSSEL0 + WDTTMSEL + WDTIS2;

    // Enable interrupts from the WDT interval timer
    SFRIE1_bits.WDTIE = 1;
}


/******************************************************************************
    Subroutine:     WDT_ISR
    Description:    Interrupt service routine for the watchdog interval timer.
                    If there are active WDT timers registered, the values will
                    be decremented, and upon expiration, the registered
                    callback function will be called. After a timer has
                    expired, it must be activated again with Set_WDT_Timer().
    Inputs:         None
    Outputs:        None

******************************************************************************/
void __attribute__((__interrupt__(WDT_VECTOR))) WDT_ISR(void)
{
    uint16_t    x;

    // Loop through all of the WDT timers
    for (x=0u; x<MAX_WDT_TIMERS; x++)
    {
        // If the current timer is active (not 0)...
        if (0u < WDT_timers[x].timeout)
        {
            // Decrement the timer
            WDT_timers[x].timeout--;

            // If the timer expired (just transitioned to 0)...
            if (0u == WDT_timers[x].timeout)
            {
                // Call the registered callback function
                WDT_timers[x].callback();
            }
        }
    }
}


/******************************************************************************
    Subroutine:     Set_WDT_Timer
    Description:    Use this function to create a watchdog timer, and register
                    the callback function to be executed when it expires.
    Inputs:         Index: The index of the WDT_timers[] array to place the
                    new WDT timer being registered. Valid values are from
                    zero to MAX_WDT_TIMERS-1.
                    timeout: The timeout value of the new timer in seconds.
                    For example, a value of 10, will expire after 10 seconds.
                    callback: The name of the function that will be called
                    when the registered timer expires.
    Outputs:        None

******************************************************************************/
void Set_WDT_Timer(uint16_t index, uint16_t timeout, WDT_Callback callback)
{
    // If the index provided exceeds the size of WDT_timers, just exit.
    if (MAX_WDT_TIMERS <= index)    {return;}

    // Otherwise, register the new timer at the index position provided.
    WDT_timers[index].timeout = timeout;
    WDT_timers[index].callback = callback;

}


/******************************************************************************
	End of File: MSP430F5529LP_WDT.c
******************************************************************************/