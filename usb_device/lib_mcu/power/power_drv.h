/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the Power Management low level driver definition
//!
//!  This module allows to configure the different power mode of the AVR core and
//!  also to setup the the internal clock prescaler
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


#ifndef _POWER_DRV_H_
#define _POWER_DRV_H_

#ifdef  __GNUC__
   #include <avr/power.h>
#endif
//! @defgroup powermode Power management drivers
//!
//! @{
   
//_____ M A C R O S ________________________________________________________

#define Setup_idle_mode()                       (SMCR=0,SMCR |= (1<<SE))
#define Setup_power_down_mode()                 (SMCR=0,SMCR |= (1<<SE)+(1<<SM1))
#define Setup_adc_noise_reduction_mode()        (SMCR=0,SMCR |= (1<<SE)+(1<<SM0))
#define Setup_power_save_mode()                 (SMCR=0,SMCR |= (1<<SE)+(1<<SM1)+(1<<SM0))
#define Setup_standby_mode()                    (SMCR=0,SMCR |= (1<<SE)+(1<<SM2)+(1<<SM1))
#define Setup_ext_standby_mode()                (SMCR=0,SMCR |= (1<<SE)+(1<<SM2)+(1<<SM1)+(1<<SM0))

//! This function reset the internal CPU core clock prescaler
//!
#ifdef  __GNUC__
   #define Clear_prescaler()                       (clock_prescale_set(0))
#else
   #define Clear_prescaler()                       (Set_cpu_prescaler(0))
#endif

//! This function configure the internal CPU core clock prescaler value
//!
//! @param x: prescaler new value
//!
#ifdef  __GNUC__
   #define Set_cpu_prescaler(x)                        (clock_prescale_set(x))
#else
   extern void Set_cpu_prescaler(U8 x);
#endif


#define Sleep_instruction()      {asm("SLEEP");}

//Backward compatibility
#define Set_power_down_mode()      set_power_down_mode()
#define Set_idle_mode()            set_idle_mode()

//_____ D E C L A R A T I O N ______________________________________________

void set_idle_mode(void);
void set_power_down_mode(void);
void set_adc_noise_reduction_mode(void);
void set_power_save_mode(void);
void set_standby_mode(void);
void set_ext_standby_mode(void);

//! This function makes the AVR core enter idle mode.
//!
#define Enter_idle_mode()                 (set_idle_mode())

//! This function makes the AVR core enter power down mode.
//!
#define Enter_power_down_mode()           (set_power_down_mode())

//! This function makes the AVR core enter adc noise reduction mode.
//!
#define Enter_adc_noise_reduction_mode()  (set_adc_noise_reduction_mode())

//! This function makes the AVR core enter power save mode.
//!
#define Enter_power_save_mode()           (set_power_save_mode())

//! This function makes the AVR core enter standby mode.
//!

#define Enter_standby_mode()              (set_standby_mode())

//! This function makes the AVR core enter extended standby mode.
//!
#define Enter_ext_standby_mode()          (set_ext_standby_mode())


//! @}

#endif  // _POWER_DRV_H_

