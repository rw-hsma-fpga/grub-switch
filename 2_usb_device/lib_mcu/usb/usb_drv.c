/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the USB driver routines.
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
#include "usb_drv.h"

//_____ M A C R O S ________________________________________________________

//_____ D E C L A R A T I O N ______________________________________________

#if (USB_DEVICE_FEATURE==DISABLED && USB_HOST_FEATURE==DISABLED)
   #error at least one of  USB_DEVICE_FEATURE or USB_HOST_FEATURE should be unabled
#endif

#if (USB_DEVICE_FEATURE==DISABLED)
   #error USB_DEVICE_FEATURE required for this project
#endif


#if (USB_HOST_FEATURE==ENABLED)
   #error USB_HOST_FEATURE not wanted for this project
#endif


#if (USB_DEVICE_FEATURE == ENABLED)

//! usb_configure_endpoint.
//!
//!  This function configures an endpoint with the selected type.
//!
//!
//! @param config0
//! @param config1
//!
//! @return Is_endpoint_configured.
//!
U8 usb_config_ep(U8 config0, U8 config1)
{
    Usb_enable_endpoint();
    UECFG0X = config0;
    UECFG1X = (UECFG1X & (1<<ALLOC)) | config1;
    Usb_allocate_memory();
    return (Is_endpoint_configured());
}

//! usb_select_endpoint_interrupt.
//!
//! This function select the endpoint where an event occurs and returns the
//! number of this endpoint. If no event occurs on the endpoints, this
//! function returns 0.
//!
//!
//! @param none
//!
//! @return endpoint number.
//!
U8 usb_select_enpoint_interrupt(void)
{
U8 interrupt_flags;
U8 ep_num;

   ep_num = 0;
   interrupt_flags = Usb_interrupt_flags();

   while(ep_num < MAX_EP_NB)
   {
      if (interrupt_flags & 1)
      {
         return (ep_num);
      }
      else
      {
         ep_num++;
         interrupt_flags = interrupt_flags >> 1;
      }
   }
   return 0;
}

//! usb_send_packet.
//!
//! This function moves the data pointed by tbuf to the selected endpoint fifo
//! and sends it through the USB.
//!
//!
//! @param ep_num       number of the addressed endpoint
//! @param *tbuf        address of the first data to send
//! @param data_length  number of bytes to send
//!
//! @return address of the next U8 to send.
//!
//! Example:
//! usb_send_packet(3,&first_data,0x20);    // send packet on the endpoint #3
//! while(!(Usb_tx_complete));              // wait packet ACK'ed by the Host
//! Usb_clear_tx_complete();                     // acknowledge the transmit
//!
//! Note:
//! tbuf is incremented of 'data_length'.
//!
U8 usb_send_packet(U8 ep_num, U8* tbuf, U8 data_length)
{
U8 remaining_length;

   remaining_length = data_length;
   Usb_select_endpoint(ep_num);
   while(Is_usb_write_enabled() && (0 != remaining_length))
   {
      Usb_write_byte(*tbuf);
      remaining_length--;
      tbuf++;
   }
   return remaining_length;
}

//! usb_read_packet.
//!
//! This function moves the data stored in the selected endpoint fifo to
//! the address specified by *rbuf.
//!
//!
//! @param ep_num       number of the addressed endpoint
//! @param *rbuf        aaddress of the first data to write with the USB data
//! @param data_length  number of bytes to read
//!
//! @return address of the next U8 to send.
//!
//! Example:
//! while(!(Usb_rx_complete));                      // wait new packet received
//! usb_read_packet(4,&first_data,usb_get_nb_byte); // read packet from ep 4
//! Usb_clear_rx();                                 // acknowledge the transmit
//!
//! Note:
//! rbuf is incremented of 'data_length'.
//!
U8 usb_read_packet(U8 ep_num, U8* rbuf, U8  data_length)
{
U8 remaining_length;

   remaining_length = data_length;
   Usb_select_endpoint(ep_num);

   while(Is_usb_read_enabled() && (0 != remaining_length))
   {
      *rbuf = Usb_read_byte();
      remaining_length--;
      rbuf++;
   }
   return remaining_length;
}

//! usb_halt_endpoint.
//!
//! This function sends a STALL handshake for the next Host request. A STALL
//! handshake will be send for each next request untill a SETUP or a Clear Halt
//! Feature occurs for this endpoint.
//!
//! @param ep_num number of the addressed endpoint
//!
//! @return none
//!
void usb_halt_endpoint (U8 ep_num)
{
   Usb_select_endpoint(ep_num);
   Usb_enable_stall_handshake();
}

//! usb_init_device.
//!
//! This function initializes the USB device controller and
//! configures the Default Control Endpoint.
//!
//!
//! @param none
//!
//! @return status
//!
U8 usb_init_device (void)
{
   Usb_select_endpoint(EP_CONTROL);
   if(!Is_usb_endpoint_enabled())
   {
#if (USB_LOW_SPEED_DEVICE==DISABLE)
      return usb_configure_endpoint(EP_CONTROL,    \
                              TYPE_CONTROL,  \
                              DIRECTION_OUT, \
                              SIZE_64,       \
                              ONE_BANK,      \
                              NYET_DISABLED);
#else
      return usb_configure_endpoint(EP_CONTROL,    \
                              TYPE_CONTROL,  \
                              DIRECTION_OUT, \
                              SIZE_8,       \
                              ONE_BANK,      \
                              NYET_DISABLED);
#endif
   }
   return FALSE;
}

#endif

