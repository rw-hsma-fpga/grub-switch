/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief Main for USB application.
//!
//! - Compiler:           GNU GCC for AVR
//! - Supported devices:  AT90USB1286
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************
//!
//! @mainpage AT90USBxxx USB Mass storage
//!
//! @section intro License
//! Use of this program is subject to Atmel's End User License Agreement.
//!
//! Please read file  \ref lic_page for copyright notice.
//!
//! @section install Description
//! This embedded application source code illustrates how to implement a USB mass storage application
//! over the AT90USBxxx controller.
//!
//! @section sample About the sample application
//! By default the sample code is delivered configured for AT90USBKey, but
//! this sample application can be configured for both STK525 or AT90USBKey hardware, see #TARGET_BOARD
//! define value in config.h file.
//!
//! @section Revision Revision
//!
//! @par V2.0.3
//!
//! Update license 
//!
//! @par V2.0.2
//!
//! USB Device Stack :
//! - Improve USB device detach behavior in mode self power
//! - Fix bug about back drive voltage on D+ after VBus disconnect (USB Certification)
//! - Remove VBus interrupt (bug IT VBUS, see errata) and manage VBus state by pooling
//! - Fix bug during attach (the interrupt must be disable during attach to don't freeze clock)
//! - Class MSC :
//!   - Improve MSC compliance in USB device mode
//!   - Add MSC compliance with Linux 2.4 kernel
//!   - Fix bug in sense command to support Vista and Windows Seven
//!   - Add write Protect Management under MAC OS
//!
//! @par V2.0.1 and before
//!
//! First releases
//!
//! @section news New in 1.0.4 delivery from 1.0.3
//! New data flash driver that improves write access time (double speed).
//! Dataflash content written with the previous data flash driver should be previously backup
//! before using this new firmware. The new driver version needs to reformat the data flash memory.
//!
//! @section src_code About the source code
//! This source code is usable with the following compilers:
//! - IAR Embedded Workbench (5.11A and higher)
//! - AVRGCC (WinAVR 20080411 and higher).
//!
//! Support for other compilers may required modifications or attention for:
//! - compiler.h file 
//! - special registers declaration file
//! - interrupt subroutines declarations
//!
//! @section arch Architecture
//! As illustrated in the figure bellow, the application entry point is located is the main.c file.
//! The main function first performs the initialization of a scheduler module and then runs it in an infinite loop.
//! The scheduler is a simple infinite loop calling all its tasks defined in the conf_scheduler.h file.
//! No real time schedule is performed, when a task ends, the scheduler calls the next task defined in
//! the configuration file (conf_scheduler.h).
//!
//! The sample dual role application is based on two different tasks:
//! - The usb_task  (usb_task.c associated source file), is the task performing the USB low level
//! enumeration process in device mode.
//! - The storage task performs SCSI bulk only protocol decoding and performs flash memory access.
//!
//! \image html arch_full.gif
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
 
//_____  I N C L U D E S ___________________________________________________

#include "config.h"
#include "modules/scheduler/scheduler.h"
#include "lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/power/power_drv.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________



int main(void)
{
  // watchdog disable
  wdtdrv_disable();

  // clear timing
  Clear_prescaler();

  // deactive JTAG (twice inside 4 cycles)
  U8 mcucr_buf = (MCUCR | (1 << JTD));
  MCUCR = mcucr_buf;
  MCUCR = mcucr_buf;

  // start tasks
  scheduler();

  return 0;
}

//! \name Procedure to speed up the startup code
//! This one increment the CPU clock before RAM initialisation
//! @{
#ifdef  __GNUC__
// Locate low level init function before RAM init (init3 section)
// and remove std prologue/epilogue
char __low_level_init(void) __attribute__ ((section (".init3"),naked));
#endif

#ifdef __cplusplus
extern "C" {
#endif
char __low_level_init()
{
  Clear_prescaler();
  return 1;
}
#ifdef __cplusplus
}
#endif
//! @}

