/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief Process USB device enumeration requests.
//!
//!  This file contains the USB endpoint 0 management routines corresponding to
//!  the standard enumeration process (refer to chapter 9 of the USB
//!  specification.
//!  This file calls routines of the usb_specific_request.c file for non-standard
//!  request management.
//!  The enumeration parameters (descriptor tables) are contained in the
//!  usb_descriptors.c file.
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
#include "lib_mcu/pll/pll_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "usb_specific_request.h"

#if ((USB_DEVICE_SN_USE==ENABLE) && (USE_DEVICE_SN_UNIQUE==ENABLE))
#include "lib_mcu/flash/flash_drv.h"
static  U8   bin_to_ascii (U8 b);
#endif

//_____ D E F I N I T I O N ________________________________________________

static   Bool  usb_get_descriptor   ( void );
static   void  usb_set_address      ( void );
static   Bool  usb_set_configuration( void );
static   void  usb_get_configuration( void );
static   Bool  usb_get_status       ( U8 bmRequestType );
static   Bool  usb_set_feature      ( U8 bmRequestType );
static   Bool  usb_clear_feature    ( U8 bmRequestType );
static   Bool  usb_get_interface    ( void );
static   Bool  usb_set_interface    ( void );

#ifndef USB_REMOTE_WAKEUP_FEATURE
   #error USB_REMOTE_WAKEUP_FEATURE should be defined as ENABLE or DISABLE in conf_usb.h
#endif

#if (USE_DEVICE_SN_UNIQUE==ENABLE)
    U8   f_get_serial_string=FALSE;
#endif


//_____ D E C L A R A T I O N ______________________________________________

#ifdef __GNUC__                          // AVRGCC does not support point to PGM space
   PGM_VOID_P pbuffer;
   #define Usb_write_PGM_byte(byte) (Usb_write_byte(pgm_read_byte_near((unsigned int)byte))) 
#else
   U8   code *pbuffer;
   #define Usb_write_PGM_byte(byte) (Usb_write_byte(*byte))
#endif

U8    endpoint_status[MAX_EP_NB];
U8    data_to_transfer;
U8    usb_configuration_nb;
U8    remote_wakeup_feature = DISABLE; 
static U8    device_status = DEVICE_STATUS;


//! @brief This function reads the SETUP request sent to the default control endpoint
//! and calls the appropriate function. When exiting of the usb_read_request
//! function, the device is ready to manage the next request.
//!
void usb_process_request(void)
{
   U8 bmRequestType;
   U8 bmRequest;

   Usb_ack_control_out();
   bmRequestType = Usb_read_byte();
   bmRequest     = Usb_read_byte();

   switch (bmRequest)
   {
      case SETUP_GET_DESCRIPTOR:
      if (USB_SETUP_GET_STAND_DEVICE == bmRequestType)
      {     
         if( usb_get_descriptor() )
            return;
      }
      break;

      case SETUP_GET_CONFIGURATION:
      if (USB_SETUP_GET_STAND_DEVICE == bmRequestType)
      {
         usb_get_configuration();
         return;
      }
      break;

      case SETUP_SET_ADDRESS:
      if (USB_SETUP_SET_STAND_DEVICE == bmRequestType)
      {
         usb_set_address();
         return;
      }
      break;

      case SETUP_SET_CONFIGURATION:
      if (USB_SETUP_SET_STAND_DEVICE == bmRequestType)
      { 
         if( usb_set_configuration() )
            return;
      }
      break;

      case SETUP_CLEAR_FEATURE:
      if (usb_clear_feature(bmRequestType))
         return;
      break;

      case SETUP_SET_FEATURE:
      if (usb_set_feature(bmRequestType))
         return;
      break;

      case SETUP_GET_STATUS:
      if (usb_get_status(bmRequestType))
         return;
      break;

      case SETUP_GET_INTERFACE:
      if (USB_SETUP_GET_STAND_INTERFACE == bmRequestType)
      {
         if( usb_get_interface() )
            return;
      }
      break;

      case SETUP_SET_INTERFACE:
      if (bmRequestType == USB_SETUP_SET_STAND_INTERFACE)
      {
         if( usb_set_interface() )
            return;
      }
      break;

      default:
      break;
   }

   // un-supported like standard request => call to user read request
   if( !usb_user_read_request(bmRequestType, bmRequest) )
   {
      // Request unknow in the specific request list from interface
      // keep that order (set StallRq/clear RxSetup) or a
      // OUT request following the SETUP may be acknowledged
      Usb_enable_stall_handshake();
      Usb_ack_receive_setup();
      endpoint_status[(EP_CONTROL & MSK_EP_DIR)] = 0x01;
   }
}


//! This function manages the SET ADDRESS request. When complete, the device
//! will filter the requests using the new address.
//!
void usb_set_address(void)
{
   U8 addr = Usb_read_byte();
   Usb_configure_address(addr);

   Usb_ack_receive_setup();

   Usb_send_control_in();                    // send a ZLP for STATUS phase
   while(!Is_usb_in_ready()) {if(Is_usb_vbus_low())break;} // waits for status phase done
                                             // before using the new address
   Usb_enable_address();
}


//! This function manages the SET CONFIGURATION request. If the selected
//! configuration is valid, this function call the usb_user_endpoint_init()
//! function that will configure the endpoints following the configuration
//! number.
//!
Bool usb_set_configuration( void )
{
   U8 configuration_number;

   // Get/Check new configuration
   configuration_number = Usb_read_byte();
   if (configuration_number > NB_CONFIGURATION)
      return FALSE;  //  Bad configuration number then stall request
   Usb_ack_receive_setup();
   usb_configuration_nb = configuration_number;
   
   Usb_send_control_in();                          // send a ZLP for STATUS phase
   usb_user_endpoint_init(usb_configuration_nb);   // endpoint configuration
   Usb_set_configuration_action();
   return TRUE;
}


//! This function manages the GET DESCRIPTOR request. The device descriptor,
//! the configuration descriptor and the device qualifier are supported. All
//! other descriptors must be supported by the usb_user_get_descriptor
//! function.
//! Only 1 configuration is supported.
//!
Bool usb_get_descriptor(void)
{
   Bool zlp;
   U16  wLength;
   U8   descriptor_type ;
   U8   string_type;
   U8   dummy;
   U8   nb_byte;
   U8   byte_to_send;
#if (USE_DEVICE_SN_UNIQUE==ENABLE)
   U16  sn_index=0;
   U8   initial_data_to_transfer;
#endif
    
   zlp             = FALSE;                  /* no zero length packet */
   string_type     = Usb_read_byte();        /* read LSB of wValue    */
   descriptor_type = Usb_read_byte();        /* read MSB of wValue    */

   switch (descriptor_type)
   {
      case DESCRIPTOR_DEVICE:
      data_to_transfer = Usb_get_dev_desc_length(); //!< sizeof (usb_user_device_descriptor);
      pbuffer          = Usb_get_dev_desc_pointer();
      break;
      
      case DESCRIPTOR_CONFIGURATION:
      data_to_transfer = Usb_get_conf_desc_length(); //!< sizeof (usb_user_configuration_descriptor);
      pbuffer          = Usb_get_conf_desc_pointer();
      break;
      
      default:
      if( !usb_user_get_descriptor(descriptor_type, string_type))
         return FALSE;  // Unknow descriptor then stall request
      break;
   }

   dummy = Usb_read_byte();                     //!< don't care of wIndex field
   dummy = Usb_read_byte();
   LSB(wLength) = Usb_read_byte();              //!< read wLength
   MSB(wLength) = Usb_read_byte();
   Usb_ack_receive_setup() ;                  //!< clear the receive setup flag

   if (wLength > data_to_transfer)
   {
      if ((data_to_transfer % EP_CONTROL_LENGTH) == 0) { zlp = TRUE; }
      else { zlp = FALSE; }                   //!< no need of zero length packet
   }
   else
   {
      data_to_transfer = (U8)wLength;         //!< send only requested number of data
   }

   Usb_ack_nak_out();
    
   byte_to_send=0;
#if (USE_DEVICE_SN_UNIQUE==ENABLE)
   initial_data_to_transfer = data_to_transfer;
#endif
   while((data_to_transfer != 0) && (!Is_usb_nak_out_sent()))
   {
      while(!Is_usb_read_control_enabled())
      {
         if (Is_usb_nak_out_sent())
            break;    // don't clear the flag now, it will be cleared after
         if (Is_usb_vbus_low())
            break;          
      }
              
      nb_byte=0;
      while(data_to_transfer != 0)        //!< Send data until necessary
      {
         if(nb_byte++==EP_CONTROL_LENGTH) //!< Check endpoint 0 size
            break;         
         
#if (USE_DEVICE_SN_UNIQUE==ENABLE)

         if(f_get_serial_string && (data_to_transfer < (initial_data_to_transfer-1)))    //if we are sending the signature characters (third byte and more...)
         {                                                                              //(The first two bytes are the length and the descriptor)
            
            switch (byte_to_send)
            {
               case 0:
               Usb_write_byte(bin_to_ascii((Flash_read_sn(sn_index)>>4) & 0x0F)); //sends the fist part (MSB) of the signature hex number, converted in ascii
               break;
              
               case 1:
               Usb_write_byte(0);                                                   //then, sends a null character (Usb_unicode)                    
               break;
    
               case 2:
               Usb_write_byte(bin_to_ascii(Flash_read_sn(sn_index) & 0x0F));      //sends the second part (LSB) of the signature hex number, converted in ascii 
               break;
    
               case 3:
               Usb_write_byte(0);                                                   //then, sends a null character (Usb_unicode)  
               sn_index++;                                                         //increments the signature address pointer.
               break;
            }
            byte_to_send = (byte_to_send+1)%4;     
         }
         else
         {
            Usb_write_PGM_byte(pbuffer++);                                                 //Write a flash byte to USB        
         }        
#else          
         Usb_write_PGM_byte(pbuffer++);          
#endif
         data_to_transfer --;                                                             //decrements the number of bytes to transmit.
      }
      
      if (Is_usb_nak_out_sent())
         break;
      if (Is_usb_vbus_low())
         break;
      Usb_send_control_in();
   }
   
#if (USE_DEVICE_SN_UNIQUE==ENABLE)      
   f_get_serial_string=FALSE;                                                   //end of signature transmission    
#endif   

   if((zlp == TRUE) && (!Is_usb_nak_out_sent()))
   {
      while(!Is_usb_read_control_enabled()) {if(Is_usb_vbus_low())break;}
      Usb_send_control_in();
   }

   while (!(Is_usb_nak_out_sent())) {if(Is_usb_vbus_low())break;}
   Usb_ack_nak_out();
   Usb_ack_control_out();
   return TRUE;
}


//! This function manages the GET CONFIGURATION request. The current
//! configuration number is returned.
//!
void usb_get_configuration(void)
{
   Usb_ack_receive_setup();

   Usb_write_byte(usb_configuration_nb);
   Usb_ack_in_ready();

   while( !Is_usb_receive_out() ){if(Is_usb_vbus_low())break;}
   Usb_ack_receive_out();
}


//! This function manages the GET STATUS request. The device, interface or
//! endpoint status is returned.
//!
Bool usb_get_status( U8 bmRequestType )
{
   U8 wIndex;
   U8 dummy;

   dummy    = Usb_read_byte();                 //!< dummy read
   dummy    = Usb_read_byte();                 //!< dummy read
   wIndex   = Usb_read_byte();

   switch(bmRequestType)
   {
      case USB_SETUP_GET_STAND_DEVICE:
      Usb_ack_receive_setup();
      Usb_write_byte(device_status);
      break;

      case USB_SETUP_GET_STAND_INTERFACE:
      Usb_ack_receive_setup();
      Usb_write_byte(0);      // Reserved - always 0
      break;

      case USB_SETUP_GET_STAND_ENDPOINT:
      Usb_ack_receive_setup();
      wIndex = wIndex & MSK_EP_DIR;
      Usb_write_byte( endpoint_status[wIndex] );
      break;
    
      default:
      return FALSE;
   }
   Usb_write_byte(0);

   Usb_send_control_in();
   while( !Is_usb_receive_out() ){if(Is_usb_vbus_low())break;}
   Usb_ack_receive_out();
   return TRUE;
}


//! This function manages the SET FEATURE request. The USB test modes are
//! supported by this function.
//!
Bool usb_set_feature( U8 bmRequestType )
{
   U8 wValue;
   U8 wIndex;
   U8 dummy;

   switch (bmRequestType)
   {
      case USB_SETUP_SET_STAND_DEVICE:
      wValue = Usb_read_byte();
      switch (wValue)
      {
         case USB_REMOTE_WAKEUP:
         if ((wValue != FEATURE_DEVICE_REMOTE_WAKEUP)
         ||  (USB_REMOTE_WAKEUP_FEATURE != ENABLED))
            return FALSE;              // Invalid request
         device_status |= USB_DEVICE_STATUS_REMOTEWAKEUP;
         remote_wakeup_feature = ENABLED;
         Usb_ack_receive_setup();
         Usb_send_control_in();
         break;

#if (USB_OTG_FEATURE == ENABLED)
         case OTG_B_HNP_ENABLE:
         if (((OTG_BMATTRIBUTES&HNP_SUPPORT) == 0) || (USB_OTG_FEATURE == DISABLED))   // see usb_descriptors.h
            return FALSE;              // Invalid request
         otg_features_supported |= OTG_B_HNP_ENABLE;
         otg_device_nb_hnp_retry = BDEV_HNP_NB_RETRY;
         Usb_ack_receive_setup();
         Usb_send_control_in();
         break;

         case OTG_A_HNP_SUPPORT:
         if (((OTG_BMATTRIBUTES&HNP_SUPPORT) == 0) || (USB_OTG_FEATURE == DISABLED))
            return FALSE;              // Invalid request
         otg_features_supported |= OTG_A_HNP_SUPPORT;
         Usb_ack_receive_setup();
         Usb_send_control_in();
         break;

         case OTG_A_ALT_HNP_SUPPORT:
         if (((OTG_BMATTRIBUTES&HNP_SUPPORT) == 0) || (USB_OTG_FEATURE == DISABLED))
            return FALSE;              // Invalid request
         otg_features_supported |= OTG_A_ALT_HNP_SUPPORT;
         Usb_ack_receive_setup();
         Usb_send_control_in();
         break;
#endif
            
         default:
         return FALSE;                 // Unknow request
         break;
      }
      break;

      case USB_SETUP_SET_STAND_INTERFACE:
      return FALSE;                    // Unknow request
      break;

      case USB_SETUP_SET_STAND_ENDPOINT:
      wValue   = Usb_read_byte();
      dummy    = Usb_read_byte();                //!< dummy read
      if (wValue != FEATURE_ENDPOINT_HALT)
         return FALSE;                 // Unknow request
      wIndex = (Usb_read_byte() & MSK_EP_DIR);
      if (wIndex == EP_CONTROL)
      {
         Usb_enable_stall_handshake();
         Usb_ack_receive_setup();
      }
      Usb_select_endpoint(wIndex);
      if( !Is_usb_endpoint_enabled())
      {
         Usb_select_endpoint(EP_CONTROL);
         return FALSE;              // Invalid request
      }
      Usb_enable_stall_handshake();
      Usb_select_endpoint(EP_CONTROL);
      endpoint_status[wIndex] = 0x01;
      Usb_ack_receive_setup();
      Usb_send_control_in();
      break;

      default:
      return FALSE;                    // Unknow request
      break;
   }
   return TRUE;
}


//! This function manages the SET FEATURE request.
//!
Bool usb_clear_feature( U8 bmRequestType )
{
   U8 wValue;
   U8 wIndex;
   U8 dummy;

   switch (bmRequestType)
   {
      case  USB_SETUP_SET_STAND_DEVICE:
      wValue = Usb_read_byte();
      if ((wValue != FEATURE_DEVICE_REMOTE_WAKEUP) || (USB_REMOTE_WAKEUP_FEATURE != ENABLED))
         return FALSE;              // Invalid request
      device_status &= ~USB_DEVICE_STATUS_REMOTEWAKEUP;
      remote_wakeup_feature = DISABLED;
      Usb_ack_receive_setup();
      Usb_send_control_in();
      break;
   
      case USB_SETUP_SET_STAND_INTERFACE:
      return FALSE;                    // Unknow request
      break;
      
      case USB_SETUP_SET_STAND_ENDPOINT:
      wValue = Usb_read_byte();
      dummy  = Usb_read_byte();
      if (wValue != FEATURE_ENDPOINT_HALT)
         return FALSE;                 // Unknow request
      wIndex = (Usb_read_byte() & MSK_EP_DIR);
      Usb_select_endpoint(wIndex);
      if( !Is_usb_endpoint_enabled())
      {
         Usb_select_endpoint(EP_CONTROL);
         return FALSE;              // Invalid request
      } 
      if(wIndex != EP_CONTROL)
      {
         Usb_disable_stall_handshake();
         Usb_reset_endpoint(wIndex);
         Usb_reset_data_toggle();
      }
      Usb_select_endpoint(EP_CONTROL);
      endpoint_status[wIndex] = 0x00;
      Usb_ack_receive_setup();
      Usb_send_control_in();
      break;
   
      default:
      return FALSE;                    // Unknow request
      break;
   }
   return TRUE;
}


//! This function manages the SETUP_GET_INTERFACE request.
//!
Bool usb_get_interface (void)
{
   U16   wInterface;
   U8    wValue_msb;
   U8    wValue_lsb;

   // Read wValue
   wValue_lsb = Usb_read_byte();
   wValue_msb = Usb_read_byte();
   // wValue = Alternate Setting
   // wIndex = Interface
   LSB(wInterface)=Usb_read_byte();
   MSB(wInterface)=Usb_read_byte();
   if( (0!=wValue_msb) || (0!=wValue_msb) )
      return FALSE;
   Usb_ack_receive_setup();
   
   Usb_write_byte( usb_user_interface_get(wInterface) );
   Usb_send_control_in();

   while( !Is_usb_receive_out() ){if(Is_usb_vbus_low())break;}
   Usb_ack_receive_out();
   return TRUE;
}


//! This function manages the SETUP_SET_INTERFACE request.
//!
Bool usb_set_interface (void)
{
   U16   wInterface;
   U8    wValue_msb;
   U8    wValue_lsb;

   // Read wValue
   wValue_lsb = Usb_read_byte();
   wValue_msb = Usb_read_byte();
   // wValue = Alternate Setting
   // wIndex = Interface
   LSB(wInterface)=Usb_read_byte();
   MSB(wInterface)=Usb_read_byte();
   if( 0!=wValue_msb )
      return FALSE;
   Usb_ack_receive_setup();
   
   usb_user_interface_reset(wInterface, wValue_lsb);
   Usb_select_endpoint(EP_CONTROL);
            
   Usb_send_control_in();
   while(!Is_usb_in_ready()) {if(Is_usb_vbus_low())break;}
   return TRUE;
}


//! This function manages the remote wake up generation
//!
void usb_generate_remote_wakeup(void)
{
   if(Is_pll_ready()==FALSE)
   {
      Pll_start_auto();
      Wait_pll_ready();
   }
   Usb_unfreeze_clock();
   if (remote_wakeup_feature == ENABLED)
   {
      Usb_initiate_remote_wake_up();
      remote_wakeup_feature = DISABLED;
   }
}  


#if ((USB_DEVICE_SN_USE==ENABLE) && (USE_DEVICE_SN_UNIQUE==ENABLE))
//! This function is used to convert a 4 bit number into an ascii character
//! 5 => '5'       10 => 'A'   
//! 
//! @param binary value to convert
//!
//! @return converted character
//!
U8 bin_to_ascii (U8 b)
{
return ( (b <= 0x09) ? (b+'0') : (b+'A'-10) );
}
#endif

