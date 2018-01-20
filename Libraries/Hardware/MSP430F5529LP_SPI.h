
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
 * FileName:      MSP430F5529LP_SPI.h
 *
 * This file provides functions for the Universal Serial Communications
 * Interface (UCA1) in the SPI Master mode for the Texas Instruments
 * MSP430F5529 Launchpad development board.
 *
 * Version 1.0
 *
 * Rev. 1.0, Initial Release
 *
 *                                                                            */
/* ===========================================================================*/

#ifndef __MSP430F5529LP_SPI_H__
#define __MSP430F5529LP_SPI_H__



/******************************************************************************
    PUBLIC DEFINITIONS
******************************************************************************/
   
   typedef enum SPI_CompletionCode
   {
	   SPI_IN_PROGRESS = 0,       // also corresponds to !done
	   SPI_COMPLETED_SUCCESS      // Completed_Success,
   } SPI_CmplCode_t;
   
   
   
/******************************************************************************
    PUBLIC FUNCTION PROTOTYPES
******************************************************************************/

   void MSP430F5529LP_SPI_Initialize(void);

   void SPI_Write(uint8_t * p_TxReg,
                  uint8_t * p_RxReg,
                  uint16_t bytes);
   
   
/******************************************************************************
    PUBLIC VARIABLES (extern)
******************************************************************************/

   
/******************************************************************************
	End MSP430F5529LP_SPI.h
******************************************************************************/

#endif   // __MSP430F5529LP_SPI_H__

