/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the mass storage task.
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
#include "storage_task.h"
#include "lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "usb_specific_request.h"
#include "modules/scsi_decoder/scsi_decoder.h"
#include "modules/control_access/ctrl_access.h"



//_____ D E C L A R A T I O N S ____________________________________________


volatile U8 cpt_sof;

bit ms_data_direction;
static _MEM_TYPE_SLOW_ U8  dCBWTag[4];

extern _MEM_TYPE_SLOW_ U8  g_scsi_status;
extern _MEM_TYPE_FAST_ U32 g_scsi_data_remaining;
extern bit ms_multiple_drive;
extern _MEM_TYPE_SLOW_ U8  g_scsi_command[16];

extern U8   endpoint_status[MAX_EP_NB];

_MEM_TYPE_SLOW_ U8 usb_LUN;


void usb_mass_storage_cbw (void);
void usb_mass_storage_csw (void);

//!
//! @brief This function initializes the hardware/software ressources required for storage task.
//!
//!
//! @param none
//!
//! @return none
//!
//!/
void storage_task_init(void)
{
   Led_init();

   init_aux0_output1();
   init_aux0_output2();
   set_wrprot_input_pullup();

   cf_mem_init();    // Init the hw/sw ressources required to drive the DF.
   Usb_enable_sof_interrupt();
}


//! @brief Entry point of the mass storage task management
//!
//! This function links the mass storage SCSI commands and the USB bus.
//!
//!
//! @param none
//!
//! @return none
void storage_task(void)
{
   if (Is_device_enumerated())
   {
      Usb_select_endpoint(EP_MS_OUT);
      if (Is_usb_receive_out())
      {
         usb_mass_storage_cbw();
         usb_mass_storage_csw();
      }
   }
}


//! This function increments the cpt_sof counter each times
//! the USB Start Of Frame interrupt subroutine is executed (1ms)
//! Usefull to manage time delays
//!
void sof_action()
{
   cpt_sof++;
}


//! @brief USB Command Block Wrapper (CBW) management
//!
//! This function decodes the CBW command and stores the SCSI command
//!
//! @warning Code:?? bytes (function code length)
//!
//! @param none
//!
//! @return none
void usb_mass_storage_cbw (void)
{
bit cbw_error;
U8  c;
U8  dummy;

   cbw_error = FALSE;
   Usb_select_endpoint(EP_MS_OUT);           //! check if dCBWSignature is correct
   if (0x55 != Usb_read_byte())
      { cbw_error = TRUE; } //! 'U'
   if (0x53 != Usb_read_byte())
      { cbw_error = TRUE; } //! 'S'
   if (0x42 != Usb_read_byte())
      { cbw_error = TRUE; } //! 'B'
   if (0x43 != Usb_read_byte())
      { cbw_error = TRUE; } //! 'C'
   if (cbw_error)
   {
      Usb_ack_receive_out();
      Usb_select_endpoint(EP_MS_IN);
      Usb_enable_stall_handshake();
      endpoint_status[(EP_MS_IN & MSK_EP_DIR)] = 0x01;
      return;
   }

   dCBWTag[0] = Usb_read_byte();             //! Store CBW Tag to be repeated in CSW
   dCBWTag[1] = Usb_read_byte();
   dCBWTag[2] = Usb_read_byte();
   dCBWTag[3] = Usb_read_byte();
   
   LSB0(g_scsi_data_remaining) = Usb_read_byte();
   LSB1(g_scsi_data_remaining) = Usb_read_byte();
   LSB2(g_scsi_data_remaining) = Usb_read_byte();
   LSB3(g_scsi_data_remaining) = Usb_read_byte();

   if (Usb_read_byte() != 0x00)              //! if (bmCBWFlags.bit7 == 1) {direction = IN}
   {
      Usb_set_ms_data_direction_in();
   }
   else
   {
      Usb_set_ms_data_direction_out();
   }

   usb_LUN = Usb_read_byte();

   if (!ms_multiple_drive)
   {
      usb_LUN = 0; // only LUN
   }

   dummy      = Usb_read_byte();                // dummy CBWCBLength read


   for (c=0; c<16; c++)                         // store scsi_command
   {
      g_scsi_command[c] = Usb_read_byte();
   }
   Usb_ack_receive_out();

   if (Is_usb_ms_data_direction_in())
   {
      Usb_select_endpoint(EP_MS_IN);
   }

   if (TRUE != scsi_decode_command())
   {
      U8 ep;
      Usb_enable_stall_handshake();
      if (Is_usb_ms_data_direction_in())
      {
         ep = (EP_MS_IN & MSK_EP_DIR);
      }else{
         ep = (EP_MS_OUT & MSK_EP_DIR);
      }
      endpoint_status[ep] = 0x01;
   }
}


//! @brief USB Command Status Wrapper (CSW) management
//!
//! This function sends the status in relation with the last CBW
//!
//!
//! @param none
//!
//! @return none
void usb_mass_storage_csw (void)
{
   Usb_select_endpoint(EP_MS_IN);
   while (Is_usb_endpoint_stall_requested())
   {
      Usb_select_endpoint(EP_CONTROL);
      if (Is_usb_receive_setup())       { usb_process_request(); }
      Usb_select_endpoint(EP_MS_IN);
   }

   Usb_select_endpoint(EP_MS_OUT);
   while (Is_usb_endpoint_stall_requested())
   {
      Usb_select_endpoint(EP_CONTROL);
      if (Is_usb_receive_setup())       { usb_process_request(); }
      Usb_select_endpoint(EP_MS_OUT);
   }


   Usb_select_endpoint(EP_MS_IN);
   while(!Is_usb_write_enabled())
   {
      if(!Is_usb_endpoint_enabled())   return; // USB Reset
   }
                                                         //! write CSW Signature
   Usb_write_byte(0x55);                                 //! 'U'
   Usb_write_byte(0x53);                                 //! 'S'
   Usb_write_byte(0x42);                                 //! 'B'
   Usb_write_byte(0x53);                                 //! 'S'
                                                         //! write stored CBW Tag
   Usb_write_byte(dCBWTag[0]);
   Usb_write_byte(dCBWTag[1]);
   Usb_write_byte(dCBWTag[2]);
   Usb_write_byte(dCBWTag[3]);
                                                         //! write data residue value
   Usb_write_byte( LSB0(g_scsi_data_remaining) );
   Usb_write_byte( LSB1(g_scsi_data_remaining) );
   Usb_write_byte( LSB2(g_scsi_data_remaining) );
   Usb_write_byte( LSB3(g_scsi_data_remaining) );

   //! write command status
   Usb_write_byte(g_scsi_status);                        //! 0 -> PASS, 1 -> FAIL

   Usb_send_in();
}
