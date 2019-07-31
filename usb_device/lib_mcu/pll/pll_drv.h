/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the low level macros and definition for the USB PLL
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  AT90USB1287, AT90USB1286, AT90USB647, AT90USB646
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PLL_DRV_H
#define PLL_DRV_H

//_____ I N C L U D E S ____________________________________________________

//! @defgroup PLL PLL driver
//! PLL Module
//! @{
//_____ M A C R O S ________________________________________________________

   //! @defgroup PLL_macros PLL Macros
   //! These functions allow to control the PLL
   //! @{
#define PLLx06          ( (0<<PLLP2) | (1<<PLLP1) | (1<<PLLP0) )

#ifdef __ICCAVR__
   #if (defined(__AT90USB1287__) || defined(__AT90USB1286__)) 
      #define PLLx03          ( (1<<PLLP2) | (0<<PLLP1) | (1<<PLLP0) )
   #elif (defined(__AT90USB647__) || defined(__AT90USB646__) || defined(__ATmega32U6__))
      #define PLLx03          ( (1<<PLLP2) | (1<<PLLP1) | (0<<PLLP0) )
   #else
      #error TARGET should be defined 
   #endif
#elif defined __GNUC__
   #if (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB1286__)) 
      #define PLLx03          ( (1<<PLLP2) | (0<<PLLP1) | (1<<PLLP0) )
   #elif (defined(__AVR_AT90USB647__) || defined(__AVR_AT90USB646__) || defined(__AVR_ATmega32U6__))
      #define PLLx03          ( (1<<PLLP2) | (1<<PLLP1) | (0<<PLLP0) )
   #elif (defined(__AVR_ATmega32U4__))
      #define PLLx03          ( (1<<PINDIV) )
   #endif
#else // Other compiler
   #error Compiler unknow
#endif


      //! @brief Start the PLL at only 48 MHz, regarding CPU frequency
      //! Start the USB PLL with correct clockfactor
#define Start_pll(clockfactor)   (PLLCSR = ( clockfactor  | (1<<PLLE)  ))
      //! return 1 when PLL locked
#define Is_pll_ready()           (PLLCSR & (1<<PLOCK) )
      //! Test PLL lock bit and wait until lock is set
#define Wait_pll_ready()         while (!(PLLCSR & (1<<PLOCK)))
      //! Stop the PLL
#define Stop_pll()               (PLLCSR  &= (~(1<<PLLE)),PLLCSR=0 ) 
#ifdef CUSTOM_PLL_START_AUTO
   #define    Pll_start_auto()   CUSTOM_PLL_START_AUTO
#else

      // Start the PLL in autofactor mode
      // regarding FOSC define
#if   (FOSC==8000)
   #define Pll_start_auto()   Start_pll(PLLx06)
#elif (FOSC==16000)
   #define Pll_start_auto()   Start_pll(PLLx03)
#else
   #error   "FOSC should be defined with 8000KHz or 16000KHz in config.h"
#endif
#endif

   //! @}

//! @}
#endif  // PLL_DRV_H


