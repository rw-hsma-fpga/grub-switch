/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the low-level dataflash routines
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

#include "config.h"                         // system configuration
#include "lib_mcu/usb/usb_drv.h"            // usb driver definition
#include "cf.h"                             // dataflash definition
#include "fat12_data.h"
#include "boot_choice.h"



void cf_init (void)
{
   boot_choice_init();

   U16 file_size = eeprom_read_word((const uint16_t*)1022);

   // limit file size to maximum if stored too big
   if (file_size > 960)
      file_size = 960;

   set_entryfile_size(file_size);

   parse_entry_file();
   build_bootfile_parameters(get_boot_choice());
}


//! This function performs a memory check on all DF.
//!
//!
//! @return bit
//!   The memory is ready     -> OK
//!   The memory check failed -> KO
//!
Bool cf_mem_check (void)
{
   // CF memory check.

   return OK;
}


//! This function is optimized and writes nb-sector * 512 Bytes from DataFlash memory to USB controller
//!
//! NOTE:
//!   - First call must be preceded by a call to the cf_read_open() function,
//!   - The USB EPIN must have been previously selected,
//!   - USB ping-pong buffers are free,
//!   - As 512 is always a sub-multiple of page size, there is no need to check
//!     page end for each Bytes,
//!   - Interrupts are disabled during transfer to avoid timer interrupt,
//!   - nb_sector always >= 1, cannot be zero.
//!
//! @param nb_sector    number of contiguous sectors to read [IN]
//!
//! @return The read succeeded   -> OK
//!
Bool cf_read_sector (U32 pos, Uint16 nb_sector)
{
   U8 i, j;

   // Set the global memory ptr at a Byte address.
   U16 sector_address = (pos << CF_LOG2_FAT12_SECTOR_SIZE);

   build_bootfile_parameters(get_boot_choice());
      
   Led_on();

   do
   {
      // make copy of sector start
      U16 curr_read_addr = sector_address;

      for (i = 8; i != 0; i--)
      {
         Disable_interrupt();    // Global disable.
         
         for(j=0;j<64;j++)
         {
             // bootsector
            if (curr_read_addr <= 0x00bf)
               Usb_write_byte(pgm_read_byte(sector0 + curr_read_addr));

            // partition table marker and cluster map
            else if ((curr_read_addr >= 0x01fe) && (curr_read_addr <= 0x0208))
               Usb_write_byte(pgm_read_byte(pt_cluster + curr_read_addr - 0x01fe));

            // file size of 'SWITCH.GRB' in root directory
            else if (curr_read_addr == SWITCH_GRB_SIZE_ADDR)
            {
               U16 file_size = get_bootfile_size();
               Usb_write_byte(LSB(file_size));
               j++; curr_read_addr++;  // double-byte write, skip one loop iteration
               Usb_write_byte(MSB(file_size));
            }

            // file size of '.entries.txt' in root directory
            else if (curr_read_addr == ENTRIES_TXT_SIZE_ADDR)
            {
               U16 file_size = get_entryfile_size();
               Usb_write_byte(LSB(file_size));
               j++; curr_read_addr++;  // double-byte write, skip one loop iteration
               Usb_write_byte(MSB(file_size));
            }

            // root directory except file sizes of 'SWITCH.GRB', '.entries.txt'
            else if ((curr_read_addr >= 0x0400) && (curr_read_addr <= 0x04bf))
               Usb_write_byte(pgm_read_byte(dir_table + curr_read_addr - 0x0400));


            // 'SWITCH.GRB' boot file: Parameters from EEPROM file, template from PROGMEM
            else if ( (curr_read_addr >= SWITCH_GRB_ADDR) && (curr_read_addr < (SWITCH_GRB_ADDR + get_bootfile_size() )) )
               Usb_write_byte(read_file_SWITCH_GRB(curr_read_addr - SWITCH_GRB_ADDR));
            
            // file '.entries.txt'; maximum size is 960 bytes, 64 bytes in EEPROM reserved
            else if ((curr_read_addr >= ENTRIES_TXT_ADDR) && (curr_read_addr < (ENTRIES_TXT_ADDR + 960)))
               Usb_write_byte(read_entry_file(curr_read_addr - ENTRIES_TXT_ADDR));

            // file '.bootpins.txt' -  data is changed based on switch bits
            else if ((curr_read_addr >= BOOTPINS_TXT_ADDR) && (curr_read_addr < (BOOTPINS_TXT_ADDR + 0x12)))
               Usb_write_byte(read_file_BOOTPINS_TXT(curr_read_addr - BOOTPINS_TXT_ADDR));


            else
               Usb_write_byte(0x00);

            curr_read_addr++;
         }

         Usb_send_in();          // Send the FIFO IN content to the USB Host.

         Enable_interrupt();     // Global interrupt re-enable.

         // Wait until the tx is done so that we may write to the FIFO IN again.
         while(Is_usb_write_enabled()==FALSE)
         {
            if(!Is_usb_endpoint_enabled())
               return KO; // USB Reset
         }         
      }
      sector_address += CF_FAT12_SECTOR_SIZE;         // increment global address pointer
      nb_sector--;               // 1 more sector read
   }
   while (nb_sector != 0);

   Led_off();

   return OK;
}



//! This function is optimized and writes nb-sector * 512 Bytes from USB controller to DataFlash memory
//!
//! NOTE:
//!   - First call must be preceded by a call to the cf_write_open() function,
//!   - As 512 is always a sub-multiple of page size, there is no need to check
//!     page end for each Bytes,
//!   - The USB EPOUT must have been previously selected,
//!   - Interrupts are disabled during transfer to avoid timer interrupt,
//!   - nb_sector always >= 1, cannot be zero.
//!
//! @param nb_sector    number of contiguous sectors to write [IN]
//!
//! @return The write succeeded  -> OK
//!
Bool cf_write_sector (U32 pos, Uint16 nb_sector)
{
   U8 i, j;

   // Set the global memory ptr at a Byte address.
   U16 sector_address = (pos << CF_LOG2_FAT12_SECTOR_SIZE);

   Led_on();

   do
   {
      // make copy of sector start
      U16 curr_write_addr = sector_address;

      //# Write 8x64b = 512b from the USB FIFO OUT.
      for (i = 8; i != 0; i--)
      {
         // Wait end of rx in USB EPOUT.
         while(!Is_usb_read_enabled())
         {
            if(!Is_usb_endpoint_enabled())
              return KO; // USB Reset
         }
   
         Disable_interrupt();    // Global disable.

         U8 usb_rx_buffer[64];

         for(j=0;j<64;j++)
         {
            usb_rx_buffer[j] = Usb_read_byte();

            // file size of '.entries.txt' in root directory
            if (curr_write_addr == ENTRIES_TXT_SIZE_ADDR)
            {
               U16 file_size;

               LSB(file_size) = usb_rx_buffer[j];
               j++; curr_write_addr++; // double-byte access, skip one loop iteration
               usb_rx_buffer[j] = Usb_read_byte();
               MSB(file_size) = usb_rx_buffer[j];

               // limit file size to maximum if stored too big
               if (file_size > 960)
                  file_size = 960;

               set_entryfile_size(file_size);

               eeprom_write_word((uint16_t*)1022, file_size);

               parse_entry_file();
               build_bootfile_parameters(get_boot_choice());
            }

            curr_write_addr++;
         }

         // copy 64 bytes to EEPROM if it was file .entries.txt
         U16 last_block_address = curr_write_addr - 64;
         if ( (last_block_address >= ENTRIES_TXT_ADDR) && (last_block_address < (ENTRIES_TXT_ADDR + 960) ))
            eeprom_write_block((U8*)usb_rx_buffer, (U8*)(last_block_address-ENTRIES_TXT_ADDR), 64);

         Usb_ack_receive_out();  // USB EPOUT read acknowledgement.
   
         Enable_interrupt();     // Global re-enable.
      } // for (i = 8; i != 0; i--)

      sector_address += 512;         // Update the memory pointer.
      nb_sector--;               // 1 more sector written

   }
   while (nb_sector != 0);

   Led_off();

   return OK;                  // Write done
}

