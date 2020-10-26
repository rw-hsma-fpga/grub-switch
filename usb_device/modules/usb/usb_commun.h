/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the usb definition constant parameters from USB V2.0
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
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

#ifndef _USB_COMMUN_H_
#define _USB_COMMUN_H_

//! \name Vendor Identifiant according by USB org to ATMEL
#define  VID_PIDCODES                           0x1209


//! \name Product Identifiant according by ATMEL
//! @{
#define  PID_GrubSwitch                         0x2015 // register with pidcodes.github.com
#define  PID_MegaHIDGeneric                     0x2013
#define  PID_MegaHIDKeyboard                    0x2017
#define  PID_MegaCDC                            0x2018
#define  PID_MegaAUDIO_IN                       0x2019
#define  PID_MegaMS                             0x201A
#define  PID_MegaAUDIO_IN_OUT                   0x201B
#define  PID_MegaHIDMouse                       0x201C
#define  PID_MegaHIDMouse_certif_U4             0x201D
#define  PID_MegaCDC_multi                      0x201E
#define  PID_AT90USB128_64_MS_HIDMS_HID_USBKEY  0x2022
#define  PID_AT90USB128_64_MS_HIDMS_HID_STK525  0x2023
#define  PID_AT90USB128_64_MS                   0x2029
#define  PID_Mega_MS_HIDMS                      0x202A
#define  PID_MegaMS_2                           0x2032
#define  PID_MegaLibUsb                         0x2050
#define  PID_ATMega8U2_DFU                      0x2FEE
#define  PID_ATMega16U2_DFU                     0x2FEF
#define  PID_ATMega32U2_DFU                     0x2FF0
#define  PID_ATMega32U6_DFU                     0x2FF2
#define  PID_ATMega16U4_DFU                     0x2FF3
#define  PID_ATMega32U4_DFU                     0x2FF4
#define  PID_AT90USB82_DFU                      0x2FF7
#define  PID_AT90USB64_DFU                      0x2FF9
#define  PID_AT90USB162_DFU                     0x2FFA
#define  PID_AT90USB128_DFU                     0x2FFB
//! @}


//! \name Global Class, SubClass & Protocol constants
//! @{
#define  CLASS_APPLICATION                   0xFE  //!< Use to declare a specific interface link at VID-PID
#define  CLASS_VENDOR                        0xFF  //!< Use to declare a specific interface link at VID-PID
#define  NO_CLASS                            0x00
#define  NO_SUBCLASS                         0x00
#define  NO_PROTOCOL                         0x00
//! @}

//! \name IAD Interface Association Descriptor constants
//! @{
#define  CLASS_IAD                       0xEF
#define  SUB_CLASS_IAD                   0x02
#define  PROTOCOL_IAD                    0x01

//! @}

//! \name Status constant of device
//! Bit 0 Self Powered
//! Bit 1 Remote Wakeup
//! Bit 2 Battery Powered
//! @{
#define  USB_DEVICE_STATUS_BUS_POWERED       0x00
#define  USB_DEVICE_STATUS_SELF_POWERED      0x01
#define  USB_DEVICE_STATUS_REMOTEWAKEUP      0x02
#define  USB_DEVICE_STATUS_BATTERYPOWERED    0x04
//! @}


//! \name Attribut constant of status device
//! @{
#define USB_CONFIG_ATTRIBUTES_RESERVED       0x80
#define USB_CONFIG_ATTRIBUTES_REMOTEWAKEUP   0x20
#define USB_CONFIG_ATTRIBUTES_SELFPOWERED    0x40
#define USB_CONFIG_BUSPOWERED                (USB_CONFIG_ATTRIBUTES_RESERVED)
#define USB_CONFIG_REMOTEWAKEUP              (USB_CONFIG_ATTRIBUTES_RESERVED | USB_CONFIG_ATTRIBUTES_REMOTEWAKEUP)
#define USB_CONFIG_SELFPOWERED               (USB_CONFIG_ATTRIBUTES_RESERVED | USB_CONFIG_ATTRIBUTES_SELFPOWERED)
//! @}


//! \name Constants used in Endpoint Descriptor
//! @{
#define  USB_ENDPOINT_BULK                   0x02
#define  USB_ENDPOINT_INTERRUPT              0x03
#define  USB_ENDPOINT_OUT                    0x00
#define  USB_ENDPOINT_IN                     0x80
#define  USB_ENDPOINT_DIR_MASK               0x80
#define  USB_ENDPOINT_NUM_MASK               (~USB_ENDPOINT_DIR_MASK)
//! @}


//! \name Constants used in setup requests
//! @{

   //! \name Requests type (bmRequestTypes)
   //! @{

      //! \name Data transfer direction
      //! bit 7,
      //! 0 = Host to device
      //! 1 = Device to host
      //! @{
#define  USB_SETUP_DIR_HOST_TO_DEVICE        (0<<7)
#define  USB_SETUP_DIR_DEVICE_TO_HOST        (1<<7)
      //! @}

      //! \name Type
      //! bit 6 to 5,
      //! 0 = Standard
      //! 1 = Class
      //! 2 = Vendor
      //! 3 = Reserved
      //! @{
#define  USB_SETUP_TYPE_STANDARD             (0<<5)
#define  USB_SETUP_TYPE_CLASS                (1<<5)
#define  USB_SETUP_TYPE_VENDOR               (2<<5)
      //! @}

      //! \name Recipient
      //! bit 4 to 0,
      //! 0 = device
      //! 1 = Interface
      //! 2 = Endpoint
      //! 3 = Other
      //! 4...31 = Reserved
      //! @{
#define  USB_SETUP_RECIPIENT_DEVICE          (0)
#define  USB_SETUP_RECIPIENT_INTERFACE       (1)
#define  USB_SETUP_RECIPIENT_ENDPOINT        (2)
#define  USB_SETUP_RECIPIENT_OTHER           (3)
      //! @}

      //! \name Request type used by standard setup request
      //! @{
#define  USB_SETUP_SET_STAND_DEVICE          (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_STANDARD |USB_SETUP_RECIPIENT_DEVICE)    // 0x00
#define  USB_SETUP_GET_STAND_DEVICE          (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_STANDARD |USB_SETUP_RECIPIENT_DEVICE)    // 0x80
#define  USB_SETUP_SET_STAND_INTERFACE       (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_STANDARD |USB_SETUP_RECIPIENT_INTERFACE) // 0x01
#define  USB_SETUP_GET_STAND_INTERFACE       (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_STANDARD |USB_SETUP_RECIPIENT_INTERFACE) // 0x81
#define  USB_SETUP_SET_STAND_ENDPOINT        (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_STANDARD |USB_SETUP_RECIPIENT_ENDPOINT)  // 0x02
#define  USB_SETUP_GET_STAND_ENDPOINT        (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_STANDARD |USB_SETUP_RECIPIENT_ENDPOINT)  // 0x82
      //! @}

      //! \name Request type used by specific setup request from class driver
      //! @{
#define  USB_SETUP_SET_CLASS_DEVICE          (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_DEVICE)       // 0x20
#define  USB_SETUP_GET_CLASS_DEVICE          (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_DEVICE)       // 0xA0
#define  USB_SETUP_SET_CLASS_INTER           (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_INTERFACE)    // 0x21
#define  USB_SETUP_GET_CLASS_INTER           (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_INTERFACE)    // 0xA1
#define  USB_SETUP_SET_CLASS_ENDPOINT        (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_ENDPOINT)     // 0x22
#define  USB_SETUP_GET_CLASS_ENDPOINT        (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_ENDPOINT)     // 0xA2
#define  USB_SETUP_SET_CLASS_OTHER           (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_OTHER)        // 0x23
#define  USB_SETUP_GET_CLASS_OTHER           (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_CLASS |USB_SETUP_RECIPIENT_OTHER)        // 0xA3
#define  USB_SETUP_SET_VENDOR_DEVICE         (USB_SETUP_DIR_HOST_TO_DEVICE |USB_SETUP_TYPE_VENDOR |USB_SETUP_RECIPIENT_DEVICE)      // 0x40
#define  USB_SETUP_GET_VENDOR_DEVICE         (USB_SETUP_DIR_DEVICE_TO_HOST |USB_SETUP_TYPE_VENDOR |USB_SETUP_RECIPIENT_DEVICE)      // 0xC0
      //! @}
   //! @}

   //! \name Standard Requests (bRequest)
   //! @{
#define  SETUP_GET_STATUS                    0x00
#define  SETUP_GET_DEVICE                    0x01
#define  SETUP_CLEAR_FEATURE                 0x01
#define  SETUP_GET_STRING                    0x03
#define  SETUP_SET_FEATURE                   0x03
#define  SETUP_SET_ADDRESS                   0x05
#define  SETUP_GET_DESCRIPTOR                0x06
#define  SETUP_SET_DESCRIPTOR                0x07
#define  SETUP_GET_CONFIGURATION             0x08
#define  SETUP_SET_CONFIGURATION             0x09
#define  SETUP_GET_INTERFACE                 0x0A
#define  SETUP_SET_INTERFACE                 0x0B
#define  SETUP_SYNCH_FRAME                   0x0C
   //! @}

   //! \name Descriptor types used in several setup requests
   //! @{
#define  DESCRIPTOR_DEVICE                   0x01
#define  DESCRIPTOR_CONFIGURATION            0x02
#define  DESCRIPTOR_STRING                   0x03
#define  DESCRIPTOR_INTERFACE                0x04
#define  DESCRIPTOR_ENDPOINT                 0x05
#define  DESCRIPTOR_DEVICE_QUALIFIER         0x06
#define  DESCRIPTOR_CONF_OTHER_SPEED         0x07
#define  DESCRIPTOR_OTG                      0x09
#define  DESCRIPTOR_IAD                      0x0B
   //! @}

   //! \name Feature types for SETUP_X_FEATURE standard request
   //! @{
#define  FEATURE_DEVICE_REMOTE_WAKEUP        0x01
#define  FEATURE_DEVICE_TEST                 0x02
#define  FEATURE_DEVICE_OTG_B_HNP_ENABLE     0x03
#define  FEATURE_DEVICE_OTG_A_HNP_SUPPORT    0x04
#define  FEATURE_DEVICE_OTG_A_ALT_HNP_SUPPORT 0x05
#define  FEATURE_ENDPOINT_HALT               0x00
   //! @}

   //! \name Feature types for SETUP_X_FEATURE standard test request
   //! @{
#define  FEATURE_DEVICE_TEST_J               0x01
#define  FEATURE_DEVICE_TEST_K               0x02
#define  FEATURE_DEVICE_TEST_SEO_NAK         0x03
#define  FEATURE_DEVICE_TEST_PACKET          0x04
#define  FEATURE_DEVICE_TEST_FORCE_ENABLE    0x05
   //! @}
//! @}

//! \name OTG descriptor (see OTG_BMATTRIBUTES)
//! @{
#define  DESCRIPTOR_OTG_bLength              0x03
#define  HNP_SUPPORT                         0x02
#define  SRP_SUPPORT                         0x01
//! @}


#endif   // _USB_COMMUN_H_

