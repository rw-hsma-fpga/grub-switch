/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the USB task either device/host or both.
//!
//!  The USB task selects the correct USB task (usb_device task or usb_host task
//!  to be executed depending on the current mode available.
//!  According to USB_DEVICE_FEATURE and USB_HOST_FEATURE value (located in conf_usb.h file)
//!  The usb_task can be configured to support USB DEVICE mode or USB Host mode or both
//!  for a dual role device application.
//!  This module also contains the general USB interrupt subroutine. This subroutine is used
//!  to detect asynchronous USB events.
//!  Note:
//!    - The usb_task belongs to the scheduler, the usb_device_task and usb_host do not, they are called
//!      from the general usb_task
//!    - See conf_usb.h file for more details about the configuration of this module
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
#include "usb_task.h"
#include "lib_mcu/usb/usb_drv.h"
#if ((USB_DEVICE_FEATURE == ENABLED))
#include "usb_descriptors.h"
#endif
#include "lib_mcu/power/power_drv.h"
#include "lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/pll/pll_drv.h"

#if ((USB_DEVICE_FEATURE == ENABLED))
   #include "modules/usb/device_chap9/usb_device_task.h"
#endif

#ifndef  USE_USB_PADS_REGULATOR
   #error "USE_USB_PADS_REGULATOR" should be defined as ENABLE or DISABLE in conf_usb.h file
#endif

//_____ M A C R O S ________________________________________________________

#ifndef LOG_STR_CODE
#define LOG_STR_CODE(str)
#else
U8 code log_device_disconnect[]="Device Disconnected";
U8 code log_id_change[]="Pin Id Change";
#endif

//_____ D E F I N I T I O N S ______________________________________________

//!
//! Public : U16 g_usb_event
//! usb_connected is used to store USB events detected upon
//! USB general interrupt subroutine
//! Its value is managed by the following macros (See usb_task.h file)
//! Usb_send_event(x)
//! Usb_ack_event(x)
//! Usb_clear_all_event()
//! Is_usb_event(x)
//! Is_not_usb_event(x)
volatile U16 g_usb_event=0;


#if (USB_DEVICE_FEATURE == ENABLED)
//!
//! Public : (bit) usb_connected
//! usb_connected is set to TRUE when VBUS has been detected
//! usb_connected is set to FALSE otherwise
//! Used with USB_DEVICE_FEATURE == ENABLED only
//!/
extern bit   usb_connected;

//!
//! Public : (U8) usb_configuration_nb
//! Store the number of the USB configuration used by the USB device
//! when its value is different from zero, it means the device mode is enumerated
//! Used with USB_DEVICE_FEATURE == ENABLED only
//!/
extern U8    usb_configuration_nb;

//!
//! Public : (U8) remote_wakeup_feature
//! Store a host request for remote wake up (set feature received)
//!/
extern U8 remote_wakeup_feature;

#ifdef WA_USB_SUSPEND_PERTUBATION
volatile U16 delay_usb;
void usb_delay_ms(U8 ms);
#endif
#endif





//_____ D E C L A R A T I O N S ____________________________________________

/**
 * @brief This function initializes the USB process.
 *
 *  Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
 *  calls the coresponding usb mode initialization function
 *
 *  @param none
 *
 *  @return none
 */
void usb_task_init(void)
{
   #if (USE_USB_PADS_REGULATOR==ENABLE)  // Otherwise assume USB PADs regulator is not used
   Usb_enable_regulator();
   #endif

   usb_device_task_init();
}

/**
 *  @brief Entry point of the USB mamnagement
 *
 *  Depending on the USB mode supported (HOST/DEVICE/DUAL_ROLE) the function
 *  calls the coresponding usb management function.
 *
 *  @param none
 *
 *  @return none
*/
void usb_task(void)
{

// ---- DEVICE ONLY USB MODE ---------------------------------------------------
#if (USB_DEVICE_FEATURE == ENABLED)
   usb_device_task();
#endif
// -----------------------------------------------------------------------------

}

//! @brief USB interrupt subroutine
//!
//! This function is called each time a USB interrupt occurs.
//! The following USB DEVICE events are taken in charge:
//! - VBus On / Off
//! - Start Of Frame
//! - Suspend
//! - Wake-Up
//! - Resume
//! - Reset
//! - Start of frame
//!
//! The following USB HOST events are taken in charge:
//! - Device connection
//! - Device Disconnection
//! - Start Of Frame
//! - ID pin change
//! - SOF (or Keep alive in low speed) sent
//! - Wake up on USB line detected
//!
//! For each event, the user can launch an action by completing
//! the associate define (See conf_usb.h file to add action upon events)
//!
//! Note: Only interrupts events that are enabled are processed
//!
//! @param none
//!
//! @return none
#ifdef __GNUC__
 ISR(USB_GEN_vect)
#else
#pragma vector = USB_General_vect
__interrupt void usb_general_interrupt()
#endif
{
   #if (USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE)
   U8 i;
   U8 save_pipe_nb;
   #endif
// ---------- DEVICE events management -----------------------------------
#if (USB_DEVICE_FEATURE == ENABLED)

   // - Device start of frame received
   if (Is_usb_sof() && Is_sof_interrupt_enabled())
   {
      Usb_ack_sof();
      Usb_sof_action();
   }
#ifdef WA_USB_SUSPEND_PERTUBATION
  // - Device Suspend event (no more USB activity detected)
   if (Is_usb_suspend() && Is_suspend_interrupt_enabled())
   {
      usb_suspended=TRUE;
      Usb_ack_wake_up();                 // clear wake up to detect next event
      Usb_send_event(EVT_USB_SUSPEND);
      Usb_ack_suspend();
      Usb_enable_wake_up_interrupt();
      Usb_disable_resume_interrupt();
      Usb_freeze_clock();
      Stop_pll();
      Usb_suspend_action();
   }
  // - Wake up event (USB activity detected): Used to resume
   if (Is_usb_wake_up() && Is_wake_up_interrupt_enabled())
   {
      if(Is_pll_ready()==FALSE)
      {
         Pll_start_auto();
         Wait_pll_ready();
      }
      Usb_unfreeze_clock();
      Usb_ack_wake_up();
      if(usb_suspended)
      {
         Usb_enable_resume_interrupt();
         Usb_enable_reset_interrupt();
         while(Is_usb_wake_up())
         {
            Usb_ack_wake_up();
         }
         usb_delay_ms(2);
         if(Is_usb_sof() || Is_usb_resume() || Is_usb_reset() )
         {
            Usb_disable_wake_up_interrupt();
            Usb_wake_up_action();
            Usb_send_event(EVT_USB_WAKE_UP);
            Usb_enable_suspend_interrupt();
            Usb_enable_resume_interrupt();
            Usb_enable_reset_interrupt();
            
         }
         else // Workarround to make the USB enter power down mode again (spurious transcient detected on the USB lines)
         {
            Usb_ack_wake_up();                 // clear wake up to detect next event
            Usb_send_event(EVT_USB_SUSPEND);
            Usb_enable_wake_up_interrupt();
            Usb_disable_resume_interrupt();
            Usb_freeze_clock();
            Stop_pll();
            Usb_suspend_action();
         }
      }
   }
  // - Resume state bus detection
   if (Is_usb_resume() && Is_resume_interrupt_enabled())
   {
      usb_suspended = FALSE;
      Usb_disable_wake_up_interrupt();
      Usb_ack_resume();
      Usb_disable_resume_interrupt();
      Usb_resume_action();
      Usb_send_event(EVT_USB_RESUME);
   }
#else
  // - Device Suspend event (no more USB activity detected)
   if (Is_usb_suspend() && Is_suspend_interrupt_enabled())
   {
      // Remote wake-up handler
      if ((remote_wakeup_feature == ENABLED) && (usb_configuration_nb != 0))
      {
        Usb_disable_suspend_interrupt();
        Usb_ack_wake_up();
        Usb_enable_wake_up_interrupt();
        Stop_pll();
        Usb_freeze_clock();
        Usb_suspend_action();
        
        // After that user can execute "Usb_initiate_remote_wake_up()" to initiate a remote wake-up
        // Note that the suspend interrupt flag SUSPI must still be set to enable upstream resume
        // So the SUSPE enable bit must be cleared to avoid redundant interrupt
        // ****************
        // Please note also that is Vbus is lost during an upstream resume (Host disconnection),
        // the RMWKUP bit (used to initiate remote wake up and that is normally cleared by hardware when sent)
        // remains set after the event, so that a good way to handle this feature is :
        //            Usb_unfreeze_clock();
        //            Usb_initiate_remote_wake_up();
        //            while (Is_usb_pending_remote_wake_up())
        //            {
        //              if (Is_usb_vbus_low())
        //              {
        //                // Emergency action (reset macro, etc.) if Vbus lost during resuming
        //                break;
        //              }
        //            }
        //            Usb_ack_remote_wake_up_start();
        // ****************
      }
      else
      {
        // No remote wake-up supported
         Usb_ack_wake_up();                 // clear wake up to detect next event
         Usb_send_event(EVT_USB_SUSPEND);
         Usb_ack_suspend();  // must be executed last (after Usb_suspend_action()) to allow upstream resume
         Usb_enable_wake_up_interrupt();
         Usb_freeze_clock();
         Stop_pll();
         Usb_suspend_action();
      }
   }
  // - Wake up event (USB activity detected): Used to resume
   if (Is_usb_wake_up() && Is_wake_up_interrupt_enabled())
   {
      if(Is_pll_ready()==FALSE)
      {
         Pll_start_auto();
         Wait_pll_ready();
      }
      Usb_unfreeze_clock();
      Usb_ack_wake_up();
      Usb_disable_wake_up_interrupt();
      Usb_wake_up_action();
      Usb_send_event(EVT_USB_WAKE_UP);
      Usb_enable_suspend_interrupt();
   }
  // - Resume state bus detection
   if (Is_usb_resume() && Is_resume_interrupt_enabled())
   {
      Usb_disable_wake_up_interrupt();
      Usb_ack_resume();
      Usb_disable_resume_interrupt();
      Usb_resume_action();
      Usb_send_event(EVT_USB_RESUME);
   }
#endif
  // - USB bus reset detection
   if (Is_usb_reset()&& Is_reset_interrupt_enabled())
   {
      Usb_ack_reset();
      usb_init_device();
      Usb_reset_action();
      Usb_send_event(EVT_USB_RESET);
   }
#endif// End DEVICE FEATURE MODE
}


#if (USB_DEVICE_FEATURE == ENABLED)
#ifdef WA_USB_SUSPEND_PERTUBATION
void usb_delay_ms(U8 ms)
{
   for(;ms;ms--)
   {
      for(delay_usb=0;delay_usb<FOSC/16;delay_usb++);
   }
}
#endif
#endif // USB_DEVICE_FEATURE == ENABLED





