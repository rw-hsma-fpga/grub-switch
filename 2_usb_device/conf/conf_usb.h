/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the possible external configuration of the USB
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
#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "modules/usb/usb_commun.h"
#include "modules/usb/usb_commun_ms.h"


//! @defgroup usb_general_conf USB application configuration
//!
//! @{


   // _________________ USB MODE CONFIGURATION ____________________________
   //
   //! @defgroup USB_op_mode USB operating modes configuration
   //! defines to enable device or host usb operating modes
   //! supported by the application
   //! @{

      //! @brief ENABLE to activate the host software library support
      //!
      //! Possible values ENABLE or DISABLE
      #define USB_HOST_FEATURE            DISABLED

      //! @brief ENABLE to activate the device software library support
      //!
      //! Possible values ENABLE or DISABLE
      #define USB_DEVICE_FEATURE          ENABLED

   //! @}

   // _________________ USB REGULATOR CONFIGURATION _______________________
   //
   //! @defgroup USB_reg_mode USB regulator configuration
   //! @{

   //! @brief Enable the internal regulator for USB pads
   //!
   //! When the application voltage is lower than 3.5V, to optimize power consumption
   //! the internal USB pads regulatr can be disabled.
#ifndef USE_USB_PADS_REGULATOR
   #define USE_USB_PADS_REGULATOR   ENABLE      // Possible values ENABLE or DISABLE
#endif
   //! @}

   // _________________ HOST MODE CONFIGURATION ____________________________
   //
   //! @defgroup USB_host_mode_cfg USB host operating mode configuration
   //!
   //! @{

   //!   @brief VID/PID supported table list
   //!
   //!   This table contains the VID/PID that are supported by the reduced host application
   //!   VID_PID_TABLE format definition:
   //!
   //!   #define VID_PID_TABLE      {VID1, number_of_pid_for_this_VID1, PID11_value,..., PID1X_Value \n
   //!                              ...\n
   //!                              ,VIDz, number_of_pid_for_this_VIDz, PIDz1_value,..., PIDzX_Value}
   #define VID_PID_TABLE            {VID_PIDCODES, 2, PID_GrubSwitch, 0x2014 \
                                    ,0x0123, 3, 0x2000, 0x2100, 0x1258}

   //!   @brief CLASS/SUBCLASS_PROTOCOL supported table list
   //!
   //!   This table contains the CLASS/SUBCLASS/PROTOCOL that is supported by the reduced host application
   //!   This table definition allows to extended the reduced application device support to an entire Class/
   //!   /subclass/protocol instead of a simple VID/PID table list.
   //!
   //!   CLASS_SUBCLASS_PROTOCOL format definition: \n
   //!   #define CLASS_SUBCLASS_PROTOCOL  {CLASS1, SUB_CLASS1,PROTOCOL1, \n
   //!                                     ...\n
   //!                                     CLASSz, SUB_CLASSz,PROTOCOLz}
   #define CLASS_SUBCLASS_PROTOCOL     {0x0A, 0x00, 0x00, \
                                       0x00, 0x00, 0x00,\
                                       0xFF,0x00,0x00}

   //! The size of RAM buffer reserved of descriptors manipulation
   #define SIZEOF_DATA_STAGE        250

   //! The address that will be assigned to the connected device
   #define DEVICE_ADDRESS           0x05

   //! The host controller will be limited to the strict VID/PID list.
   //! When enabled, if the device PID/VID does not belongs  to the supported list,
   //! the host controller library will not go to deeper configuration, but to error state.
   #define HOST_STRICT_VID_PID_TABLE      DISABLE

   //! Try to configure the host pipe according to the device descriptors received
   #define HOST_AUTO_CFG_ENDPOINT         ENABLE

   //! Host start of frame interrupt always enable
   #define HOST_CONTINUOUS_SOF_INTERRUPT  DISABLE

   //! When Host error state detected, goto unattached state
   #define HOST_ERROR_RESTART             ENABLE

   //! Force WDT reset upon ID pin change
   #define ID_PIN_CHANGE_GENERATE_RESET   DISABLE

   //! NAK handshake in 1/4sec (250ms) before timeout
   #define NAK_TIMEOUT_DELAY              1

   #if (HOST_AUTO_CFG_ENDPOINT==FALSE)
      //! If no auto configuration of EP, map here user function
      #define        User_configure_endpoint()
   #endif

   //! @defgroup host_cst_actions USB host custom actions
   //!
   //! @{
   // write here the action to associate to each USB host event
   // be carefull not to waste time in order not disturbing the functions
   #define Usb_id_transition_action()
   #define Host_device_disconnection_action()
   #define Host_device_connection_action()
   #define Host_sof_action()
   #define Host_suspend_action()                      host_suspend_action();
   #define Host_hwup_action()
   #define Host_device_not_supported_action()
   #define Host_device_supported_action()
   #define Host_device_error_action()
   //! @}

extern void host_suspend_action(void);
   //! @}


// _________________ DEVICE MODE CONFIGURATION __________________________

   //! @defgroup USB_device_mode_cfg USB device operating mode configuration
   //!
   //! @{

#define USB_DEVICE_SN_USE          ENABLE             // Must be enable to see all disks under Windows in USB device MS mode
#define USE_DEVICE_SN_UNIQUE       DISABLE            // ignore if USB_DEVICE_SN_USE = DISABLE

#define EP_MS_IN              1
#define EP_MS_OUT             2

#define USB_REMOTE_WAKEUP_FEATURE   DISABLE

#define Usb_unicode(a)         ((U16)(a))

   //! @defgroup device_cst_actions USB device custom actions
   //!
   //! @{
   // write here the action to associate to each USB event
   // be carefull not to waste time in order not disturbing the functions
#define Usb_sof_action()         sof_action();
#define Usb_wake_up_action()
#define Usb_resume_action()
#define Usb_suspend_action()
#define Usb_reset_action()
#define Usb_vbus_on_action()
#define Usb_vbus_off_action()
#define Usb_set_configuration_action()
   //! @}

extern void sof_action(void);
   //! @}

   //! @defgroup device_cst_actions USB device custom actions
   //!
   //! @{
   // write here the action to associate to each SCSI commands
   // be carefull not to waste time in order not disturbing the functions
#define Scsi_start_read_action()    // Led_on()
#define Scsi_stop_read_action()     // Led_off()
#define Scsi_start_write_action()   // Led_on()
#define Scsi_stop_write_action()    // Led_off()
   //! @}

//! @}

#endif // _CONF_USB_H_
