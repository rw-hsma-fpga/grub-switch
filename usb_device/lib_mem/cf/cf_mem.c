/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief  This file contains the interface routines of Data Flash memory.
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

#include "config.h"                         // system configuration
#include "cf_mem.h"
#include "cf.h"
#include "fat12_data.h"



//_____ D E C L A R A T I O N ______________________________________________


//! This function initializes the hw/sw ressources required to drive the DF.
//!
void cf_mem_init(void)
{
   cf_init();        // Init the CF driver and its communication link.
}


//! This function tests the state of the CF memory.
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   Not initialize ->    CTRL_BUSY
//!   Else           ->    CTRL_NO_PRESENT
//!
Ctrl_status cf_test_unit_ready(void)
{
   return( (OK==cf_mem_check()) ? CTRL_GOOD : CTRL_NO_PRESENT);
}


//! @brief This function gives the address of the last valid sector.
//!
//! @param *u32_nb_sector  number of sector (sector = 512B)
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   Not initialize ->    CTRL_BUSY
//!   Else           ->    CTRL_NO_PRESENT
//!
Ctrl_status cf_read_capacity( U32 _MEM_TYPE_SLOW_ *u32_nb_sector )
{
   *u32_nb_sector = CF_FAT12_SECTORS - 1;

   return cf_test_unit_ready();
}


//! This function returns the write protected status of the memory.
//!
//! Only used by memory removal with a HARDWARE SPECIFIC write protected detection
//! !!! The customer must unplug the memory to change this write protected status,
//! which cannot be for a DF.
//!
//! @return FALSE, the memory is not write-protected
//!
Bool  cf_wr_protect(void)
{
   return CF_FAT12_WRITE_PROTECTED;
}


//! This function tells if the memory has been removed or not.
//!
//! @return FALSE, The memory isn't removed
//!
Bool  cf_removal(void)
{
   return FALSE;
}



//------------ STANDARD FUNCTIONS to read/write the memory --------------------

//! This function performs a read operation of n sectors from a given address to USB
//!
//! @param addr         Sector address to start the read from
//! @param nb_sector    Number of sectors to transfer
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   A error occur  ->    CTRL_FAIL
//!
Ctrl_status cf_read_10( U32 addr , U16 nb_sector )
{
   U8 status = OK;

   status = cf_read_sector(addr, nb_sector);             // transfer data from memory to USB
   
   if(status == KO)
      return CTRL_FAIL;
      
   return CTRL_GOOD;
}


//! This function performs a write operation of n sectors to a given address from USB
//!
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   A error occur  ->    CTRL_FAIL
//!
Ctrl_status cf_write_10( U32 addr , U16 nb_sector )
{

   if( CF_FAT12_WRITE_PROTECTED )
      return CTRL_FAIL;

   if( KO == cf_write_sector(addr, nb_sector) )  // transfer data from memory to USB
      return CTRL_FAIL;

   return CTRL_GOOD;
}



