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

#ifndef _MEM_CTRL_H_
#define _MEM_CTRL_H_


#include "ctrl_status.h"
#include "lib_mem/cf/cf_mem.h"


//_____ D E F I N I T I O N S ______________________________________________


//**** Listing of communication interfaces ****************************************


Ctrl_status    mem_test_unit_ready( U8 lun );
Ctrl_status    mem_read_capacity( U8 lun , U32 _MEM_TYPE_SLOW_ *u32_nb_sector );
U8             mem_sector_size( U8 lun );
Bool           mem_wr_protect( U8 lun );
Bool           mem_removal( U8 lun );
U8 code*       mem_name( U8 lun );


//**** Listing of READ/WRITE interface ************************************

//---- Interface for USB ---------------------------------------------------
Ctrl_status memory_2_usb( U8 lun , U32 addr , U16 nb_sector );
Ctrl_status usb_2_memory( U8 lun , U32 addr , U16 nb_sector );

#endif   // _MEM_CTRL_H_

