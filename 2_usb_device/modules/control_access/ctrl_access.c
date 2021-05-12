/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the interface :
//!  - between MEMORY  <-> USB (chip in USB device mode)
//!  - between MEMORY* <-> RAM (e.g. Embedded FileSystem control)
//!  *in this case the memory may be a USB MS device connected on the USB host of the chip
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
#include "ctrl_access.h"


//_____ D E F I N I T I O N S ______________________________________________

const U8 code  lun_name[]="\"On-chip Code Flash\"";


//*************************************************************************
//**** Listing of communication interfaces ****************************************
//*************************************************************************



//! This function test the state of memory, and start the initialisation of the memory
//!
//! MORE (see SPC-3, 5.2.4) : The TEST UNIT READY command allows an application client
//! to poll a logical unit until it is ready without the need to allocate space for returned data.
//! The TEST UNIT READY command may be used to check the media status of logical units with removable media.
//!
//! @param lun        Logical unit number
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   Memory unplug  ->    CTRL_NO_PRESENT
//!   Not initialize ->    CTRL_BUSY
//!
Ctrl_status mem_test_unit_ready( U8 lun )
{
   if (lun == 0)
      return cf_test_unit_ready();

   return   CTRL_FAIL;
}


//! This function return the capacity of the memory
//!
//! @param lun        Logical unit number
//!
//! @return *u32_last_sector the last address sector
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   Memory unplug  ->    CTRL_NO_PRESENT
//!
Ctrl_status mem_read_capacity( U8 lun , U32 _MEM_TYPE_SLOW_ *u32_last_sector )
{
   if (lun == 0)
      return cf_read_capacity( u32_last_sector );

   return   CTRL_FAIL;
}


//! This function return the sector size of the memory
//!
//! @param lun        Logical unit number
//!
//! @return           size of sector (unit 512B)
//!
U8 mem_sector_size( U8 lun )
{
   return 1;
}


//! This function return is the write protected mode
//!
//! @param lun        Logical unit number
//!
//! Only used by memory removal with a HARDWARE SPECIFIC write protected detection
//! !!! The customer must be unplug the card for change this write protected mode.
//!
//! @return TRUE  -> the memory is protected
//!
Bool  mem_wr_protect( U8 lun )
{
   if (lun == 0)
      return cf_wr_protect();
   else
      return CTRL_FAIL;
}


//! This function inform about the memory type
//!
//! @param lun        Logical unit number
//!
//! @return TRUE  -> The memory is removal
//!
Bool  mem_removal( U8 lun )
{
   if (lun == 0)
      return cf_removal();
   else
      return CTRL_FAIL;
}


//! This function returns a pointer to the LUN name
//!
//! @param lun        Logical unit number
//!
//! @return pointer to code string
//!

U8* mem_name( U8 lun )
{
   if (lun == 0)
      return (U8 code*)lun_name;

   return 0;   // Remove compiler warning
}



//*************************************************************************
//**** Listing of READ/WRITE interface ************************************
//*************************************************************************


//! This function tranfer a data from memory to usb
//!
//! @param lun          Logical unit number
//! @param addr         Sector address to start read (sector = 512B)
//! @param nb_sector    Number of sectors to transfer
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   A error occur  ->    CTRL_FAIL
//!   Memory unplug  ->    CTRL_NO_PRESENT
//!
Ctrl_status    memory_2_usb( U8 lun , U32 addr , U16 nb_sector )
{
   Ctrl_status status=CTRL_FAIL;

   if (lun == 0)
      status = cf_read_10(addr , nb_sector);

   return   status;
}

//! This function transfer a data from usb to memory
//!
//! @param lun          Logical unit number
//! @param addr         Sector address to start write (sector = 512B)
//! @param nb_sector    Number of sectors to transfer
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   A error occur  ->    CTRL_FAIL
//!   Memory unplug  ->    CTRL_NO_PRESENT
//!
Ctrl_status    usb_2_memory( U8 lun , U32 addr , U16 nb_sector )
{
   Ctrl_status status=CTRL_FAIL;

   if (lun == 0)
      status = cf_write_10(addr , nb_sector);

   return   status;
}

