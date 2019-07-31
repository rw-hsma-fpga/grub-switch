/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief user call-back functions
//!
//!  This file contains the user call-back functions corresponding to the
//!  application:
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


//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "conf_usb.h"
#include "lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "usb_specific_request.h"
#include "modules/control_access/ctrl_access.h"
#if ((USB_DEVICE_SN_USE==ENABLE) && (USE_DEVICE_SN_UNIQUE==ENABLE))
#include "lib_mcu/flash/flash_drv.h"
#endif

//_____ D E F I N I T I O N ________________________________________________

bit  ms_multiple_drive;

#ifdef __GNUC__
extern PGM_VOID_P pbuffer;
#else
extern U8   code *pbuffer;
#endif
extern U8   data_to_transfer;


//_____ D E C L A R A T I O N ______________________________________________

//! @breif This function checks the specific request and if known then processes it
//!
//! @param type      corresponding at bmRequestType (see USB specification)
//! @param request   corresponding at bRequest (see USB specification)
//!
//! @return TRUE,  when the request is processed
//! @return FALSE, if the request is'nt know (STALL handshake is managed by the main standard request function).
//!
Bool usb_user_read_request(U8 type, U8 request)
{
   U16   wInterface;
   U8    wValue_msb;
   U8    wValue_lsb;

   // Read wValue
   wValue_lsb = Usb_read_byte();
   wValue_msb = Usb_read_byte();

   //** Specific request from Class MassStorage
   if( USB_SETUP_SET_CLASS_INTER == type )
   {
      switch( request )
      {
         case SETUP_MASS_STORAGE_RESET:
         // wValue must be 0
         // wIndex = Interface
         if( (0!=wValue_lsb) || (0!=wValue_msb) )
            break;
         LSB(wInterface)=Usb_read_byte();
         MSB(wInterface)=Usb_read_byte();
         if( INTERFACE_NB != wInterface )
            break;
         Usb_ack_receive_setup();
         Usb_send_control_in();
         while(!Is_usb_in_ready());
         return TRUE;
         break;
      }
   }
   if( USB_SETUP_GET_CLASS_INTER == type )
   {
      switch( request )
      {
         case SETUP_MASS_STORAGE_GET_MAX_LUN:
         // wValue must be 0
         // wIndex = Interface
         if( (0!=wValue_lsb) || (0!=wValue_msb) )
            break;
         LSB(wInterface)=Usb_read_byte();
         MSB(wInterface)=Usb_read_byte();
         if( INTERFACE_NB != wInterface )
            break;
         Usb_ack_receive_setup();
         Usb_write_byte( 0 ); // max LUN number is 0 (single device)
         Usb_send_control_in();
         while(!Is_usb_in_ready());
         while( !Is_usb_receive_out() );
         Usb_ack_receive_out();
         ms_multiple_drive = 1;
         return TRUE;
         break;
      }
   }
   
   return FALSE;  // No supported request
}


//! @brief This function configures the endpoints
//!
//! @param conf_nb configuration number choosed by USB host
//!
void usb_user_endpoint_init(U8 conf_nb)
{
   usb_configure_endpoint(EP_MS_IN,      \
                          TYPE_BULK,     \
                          DIRECTION_IN,  \
                          SIZE_64,       \
                          TWO_BANKS,     \
                          NYET_ENABLED);
   usb_configure_endpoint(EP_MS_OUT,     \
                          TYPE_BULK,     \
                          DIRECTION_OUT, \
                          SIZE_64,       \
                          TWO_BANKS,     \
                          NYET_ENABLED);
}


//! @brief This function returns the interface alternate setting
//!
//! @param wInterface         Interface selected
//!
//! @return alternate setting configurated
//!
U8   usb_user_interface_get( U16 wInterface )
{
   return 0;  // Only one alternate setting possible for all interface
}


//! @brief This function selects (and resets) the interface alternate setting
//!
//! @param wInterface         Interface selected
//! @param alternate_setting  alternate setting selected
//!
void usb_user_interface_reset(U16 wInterface, U8 alternate_setting)
{  
   // default setting selected = reset data toggle
   if( INTERFACE_NB == wInterface )
   {
      // Interface Mass Storage
      Usb_select_endpoint(EP_MS_IN);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(EP_MS_IN);
      Usb_reset_data_toggle();
      Usb_select_endpoint(EP_MS_OUT);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(EP_MS_OUT);
      Usb_reset_data_toggle();
   }
}

   
//! This function fills the global descriptor
//!
//! @param type      corresponding at MSB of wValue (see USB specification)
//! @param string    corresponding at LSB of wValue (see USB specification)
//!
//! @return FALSE, if the global descriptor no filled
//!
Bool usb_user_get_descriptor(U8 type, U8 string)
{
   switch(type)
   {
      case DESCRIPTOR_STRING:
      switch (string)
      {
         case LANG_ID:
         data_to_transfer = sizeof (usb_user_language_id);
         pbuffer = &(usb_user_language_id.bLength);
         return TRUE;
         break;
        
         case MAN_INDEX:
         data_to_transfer = sizeof (usb_user_manufacturer_string_descriptor);
         pbuffer = &(usb_user_manufacturer_string_descriptor.bLength);
         return TRUE;
         break;
        
         case PROD_INDEX:
         data_to_transfer = sizeof (usb_user_product_string_descriptor);
         pbuffer = &(usb_user_product_string_descriptor.bLength);
         return TRUE;
         break;
           
#if (USB_DEVICE_SN_USE==ENABLE)              
         case SN_INDEX:
         data_to_transfer = sizeof (usb_user_serial_number);
         pbuffer = &(usb_user_serial_number.bLength);
#if (USE_DEVICE_SN_UNIQUE==ENABLE)
         f_get_serial_string=TRUE;
         data_to_transfer += (SN_LENGTH*4);
#endif
         return TRUE;
         break;
#endif
      }
      break;
   }
   return FALSE;
}

