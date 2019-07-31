/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the USB device controller.
//!
//!  The USB task checks the income of new requests from the USB Host.
//!  When a Setup request occurs, this task will launch the processing
//!  of this setup contained in the usb_standard_request.c file.
//!  Other class specific requests are also processed in this file.
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

//_____  I N C L U D E S ___________________________________________________

#include "config.h"
#include "conf_usb.h"
#include "usb_device_task.h"
#include "modules/usb/usb_task.h"
#include "lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "lib_mcu/pll/pll_drv.h"

//_____ M A C R O S ________________________________________________________




//_____ D E F I N I T I O N S ______________________________________________

//!
//! Public : (bit) usb_connected
//! usb_connected is set to TRUE when VBUS has been detected
//! usb_connected is set to FALSE otherwise
//!/
bit   usb_connected=FALSE;

//!
//! Public : (bit) usb_suspended
//! usb_suspended is set to TRUE when USB is in suspend mode
//! usb_suspended is set to FALSE otherwise
//!/
bit   usb_suspended=FALSE;




//!
//! Public : (U8) usb_configuration_nb
//! Store the number of the USB configuration used by the USB device
//! when its value is different from zero, it means the device mode is enumerated
//! Used with USB_DEVICE_FEATURE == ENABLED only
//!/
extern U8  usb_configuration_nb;




//_____ D E C L A R A T I O N S ____________________________________________

//!
//! @brief This function initializes the USB device controller and system interrupt
//!
//! This function enables the USB controller and init the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//!
//! @param none
//!
//! @return none
//!
//!/
void usb_device_task_init(void)
{
   Enable_interrupt();
   Usb_disable();
   Usb_enable();
   // Usb_select_device();
#if (USB_LOW_SPEED_DEVICE==ENABLE)
   Usb_low_speed_mode();
#endif
   Enable_interrupt();
}

//!
//! @brief This function initializes the USB device controller
//!
//! This function enables the USB controller and init the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//! Start device function is executed once VBUS connection has been detected
//! either by the VBUS change interrupt either by the VBUS high level
//!
//! @param none
//!
//! @return none
//!
void usb_start_device (void)
{
   Pll_start_auto();
   Wait_pll_ready();

   Disable_interrupt();
   Usb_unfreeze_clock();
   usb_init_device();         // configure the USB controller EP0
   Usb_attach();
   Enable_interrupt();
   
   Usb_enable_suspend_interrupt();
   Usb_enable_reset_interrupt();
}

//! @brief Entry point of the USB device mamagement
//!
//! This function is the entry point of the USB management. Each USB
//! event is checked here in order to launch the appropriate action.
//! If a Setup request occurs on the Default Control Endpoint,
//! the usb_process_request() function is call in the usb_standard_request.c file
//!
//! @param none
//!
//! @return none
void usb_device_task(void)
{
  
   // Non-OTG exclusives Device operations

   // VBUS state detection
   if (Is_usb_vbus_high()&& (usb_connected==FALSE))
   {
      usb_connected = TRUE;
      remote_wakeup_feature = DISABLED;
      Usb_vbus_on_action();
      Usb_send_event(EVT_USB_POWERED);
      usb_start_device();
   }
   if (Is_usb_vbus_low()&& (usb_connected==TRUE))
   {
      usb_connected = FALSE;
      usb_configuration_nb = 0;
      Usb_send_event(EVT_USB_UNPOWERED);
      Usb_detach();
      Usb_freeze_clock();
      Usb_vbus_off_action();
   }

   if(Is_usb_event(EVT_USB_RESET))
   {
      Usb_ack_event(EVT_USB_RESET);
      Usb_reset_endpoint(0);
      usb_configuration_nb=0;
   }

   
   
   // =======================================
   // Common Standard Device Control Requests
   // =======================================
   //   - device enumeration process
   //   - device control commands and features
   Usb_select_endpoint(EP_CONTROL);
   if (Is_usb_receive_setup())
   {
      usb_process_request();
   }
}
