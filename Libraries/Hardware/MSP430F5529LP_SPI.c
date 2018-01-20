
/* ########################################################################## */
/*
 * This file was created by www.DavesMotleyProjects.com
 *
 * This software is provided under the following conditions:
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub-license, and/or sell copies of the Software, and to
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
 * FileName:      MSP430F5529LP_SPI.c
 *
 * This file provides functions for the Universal Serial Communications
 * Interface (UCB0) in the SPI Master mode for the Texas Instruments
 * MSP430F5529 Launchpad development board.
 *
 * Version 1.0
 *
 * Rev. 1.0, Initial Release
 *
 *                                                                            */
/* ===========================================================================*/

#include "MSP430F5529LP.h"
#include "MSP430F5529LP_CLOCK.h"
#include "MSP430F5529LP_SPI.h"
#include "MSP430F5529LP_TIMERA2.h"


/******************************************************************************
   PUBLIC DEFINITIONS
******************************************************************************/

   
/******************************************************************************
   PUBLIC VARIABLES
******************************************************************************/
  
 
/******************************************************************************
   PRIVATE DEFINITIONS (static const)
******************************************************************************/
   

   
/******************************************************************************
   PRIVATE FUNCTION PROTOTYPES (static)
******************************************************************************/

    static void SpiIsr0Rx(void);
   
    static void SpiIsr0Tx(void);
   
   
/******************************************************************************
   PRIVATE VARIABLES (static)
******************************************************************************/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                  ---- READ ME - DEBUGGING INFO ----
//
//  To view file scope static variables in the CCS debug watch window, the
//  following syntax must be used. 'filename.c'::variableName
//  The filename must be the full filename including the .c extension and
//  must be surrounded by the single quotes, followed by a double-colon.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    static  volatile uint8_t     *pRxData;
    static  volatile uint8_t     *pTxData;

    static  volatile SPI_CmplCode_t  done;

    static  volatile uint16_t   s_SpiErrorCnt;
  
    static  volatile uint16_t   s_RxByteCounter;

   
/******************************************************************************
   Subroutine:    MSP430F5529LP_I2C_Initialize
   Description:   This function initializes the MSP430F5529LP UCB1 comm
                  peripheral for I2C Master mode at 50 KHz.
   Inputs:        None
   Outputs:       None

******************************************************************************/
void MSP430F5529LP_SPI_Initialize(void)
{

    UCB0CTL1_bits.UCSWRST  = 1u;      // Disable SPI (in reset)

    // Reset error collection variables
    s_SpiErrorCnt = 0;

    // Reset both control registers to default values (just in case)
    UCB0CTL0 = 1u;
    UCB0CTL1 = 1u;

    // Configure SPI in master mode
    UCB0CTL0_bits.UCMODEx = 0u;      // Select 3-pin SPI mode
    UCB0CTL0_bits.UCSYNC = 1u;       // Synchronous mode
    UCB0CTL0_bits.UCMST = 1u;        // Select Master Mode
    UCB0CTL1_bits.UCSSELx = 3u;      // Select SMCLK Source

    UCB0CTL0_bits.UCCKPH = 0u;
    UCB0CTL0_bits.UCCKPL = 1u;
    UCB0CTL0_bits.UCMSB = 1u;
    UCB0CTL0_bits.UC7BIT = 0u;


    // Set SPI clock speed to 50KHz = 24MHz/480
    // Prescaler is determined by (UCxxBR0 + (UCxxBR1 × 256))
    UCB0BR1 = 1u;
    UCB0BR0 = 224u;

    // Select the special function mapping for UCA1 SPI Mode pins
    P3DIR_bits.P3DIR0 = 1;      // set P3.0 as Output
    P3SEL_bits.P3SEL0 = 1;      // select P3.0 as (SPI) MOSI
    P3DIR_bits.P3DIR1 = 0;      // set P3.1 as Input
    P3SEL_bits.P3SEL1 = 1;      // select P3.1 as (SPI) MISO
    P3DIR_bits.P3DIR2 = 1;      // set P3.1 as Output
    P3SEL_bits.P3SEL2 = 1;      // select P3.2 as (SPI) CLK

    UCB0IE = 0u;

    UCB0CTL1_bits.UCSWRST  = 0u;      // Enable SPI (not reset)
}



/******************************************************************************
   Subroutine:    SPI_Write
   Description:   
   Inputs:
   Outputs:

******************************************************************************/
void SPI_Write(uint8_t * p_TxReg, uint8_t * p_RxReg, uint16_t bytes)
{
    while(UCB0STAT_bits.UCBBUSY) {};
     
    UCB0CTL1_bits.UCSWRST  = 1u;    

    pTxData = p_TxReg;
    pRxData = p_RxReg;
    s_RxByteCounter = bytes;

    UCB0CTL1_bits.UCSWRST  = 0u;      

    UCB0IE_bits.UCRXIE = 1u;

    UCB0TXBUF = *pTxData++;

    done = SPI_IN_PROGRESS;
    while (SPI_IN_PROGRESS == done) 
	{
		asm("NOP");
	}

    return;
}



/******************************************************************************
   Subroutine:    USCIB0_SPI_ISR
   Description:   
   Inputs:
   Outputs:

******************************************************************************/
__attribute__((interrupt(USCI_B0_VECTOR)))
void USCIB0_SPI_ISR(void)
{ 
    static uint16_t UCB0IVector;

    UCB0IVector = UCB0IV;

    switch (UCB0IVector)
    {
        case (UCBxIV_SPI_UCRXIFG):
        {
            SpiIsr0Rx();
            break;
        }
        case (UCBxIV_SPI_UCTXIFG):
        {
            SpiIsr0Tx();
            break;
        }
        default:
        {
            // No Interrupt
        }
    }
}



/******************************************************************************
   Subroutine:    SpiIsr0Rx
   Description:   
   Inputs:
   Outputs:

******************************************************************************/
static void SpiIsr0Rx(void)
{  
    // Read the received byte into the buffer using the pRxData pointer.
    // Increment the pointer to be ready for the next byte. The interrupt flag
    // is cleared automatically when UCB0RXBUF is read.
	*pRxData++ = UCB0RXBUF;

    s_RxByteCounter--;
    if (s_RxByteCounter)
    {
        UCB0TXBUF = *pTxData++;
    }
    else
    {
        UCB0IE_bits.UCRXIE = 0u;
        done = SPI_COMPLETED_SUCCESS;
    }
}



/******************************************************************************
   Subroutine:    SpiIsr0Tx
   Description:   
   Inputs:
   Outputs:

******************************************************************************/
static void SpiIsr0Tx(void)
{  
	// Write the next value to the SPI transmit buffer. 
	UCB0TXBUF = *pTxData++;
}



/******************************************************************************
	End of File: MSP430F5529LP_SPI.c
******************************************************************************/
