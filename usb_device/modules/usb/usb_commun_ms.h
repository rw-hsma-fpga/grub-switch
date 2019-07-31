/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the usb MassStorage definition constant parameters
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

#ifndef _USB_COMMUN_MS_H_
#define _USB_COMMUN_MS_H_
                             
//! \name Global Class, SubClass & Protocol constants for Mass Storage
//! @{
#define  MS_CLASS                            0x08  //!< Mass Storage Class

// ABOUT SUBCLASS, only sub class 6 must be used for new design.
// What is the SCSI transparent command set (Sub Class 6) ?
// The SCSI specifications are available from http://www.t10.org/,
// but these documents don't mention a transparent command set.
// According to its creator, subclass 06h means that the host should determine the device type by issuing a SCSI INQUIRY command.
// In the returned INQUIRY data, bits 4..0 of byte 0 specify a peripheral device type (PDT).
// The SCSI Primary Commands (SPC) specification defines various PDTs and the specifications they should comply with. 
// The code in the VERSION returned in INQUIRY corresponds to a command set of PDT.
#define  MS_SUB_CLASS1                       0x01  //!< Reduced Block Commands (RBC) T10, project 1240-D
#define  MS_SUB_CLASS2                       0x02  //!< SFF-8020i or MMC-2 (ATAPI) command blocks
#define  MS_SUB_CLASS3                       0x03  //!< Typically, a tape device uses QIC-157 command blocks
#define  MS_SUB_CLASS4                       0x04  //!< (UFI) Typically, a floppy disk drive (FDD) device
#define  MS_SUB_CLASS5                       0x05  //!< SFF-8070i command blocks 
#define  MS_SUB_CLASS6                       0x06  //!< SCSI transparent Command Set

// ABOUT PROTOCOL, The CBI transport specification is approved for use only with full-speed floppy disk drives.
// CBI shall not be used in high-speed capable divices, or in devices other than floppy disk drives.
// Usage of CBI for any new design is discouraged. 
#define  MS_PRO_CTRLBULKINT_COMP             0x00  //! Control/Bulk/Interrupt (CBI) protocol with command completion interrupt
#define  MS_PRO_CTRLBULKINT_NOCOMP           0x01  //! Control/Bulk/Interrupt (CBI) protocol with NO command completion interrupt
#define  MS_PROTOCOL_BULK_ONLY               0x50  //!< Bulk-Only Transport
#define  MS_PROTOCOL                         MS_PROTOCOL_BULK_ONLY
//! @}

//! \name Specific setup requests from Mass Storage
//! @{
#define  SETUP_MASS_STORAGE_GET_MAX_LUN      0xFE
#define  SETUP_MASS_STORAGE_RESET            0xFF
//! @}

#endif   // _USB_COMMUN_MS_H_

