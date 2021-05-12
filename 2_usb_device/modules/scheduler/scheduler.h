/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file is the definition of the scheduler
//!
//!  This file contains the scheduler definition and the task function to be
//!  executed by the scheduler
//!  NOTE:
//!    SCHEDULER_TICK & FPER are defined in config.h
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

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

//!_____ I N C L U D E S ____________________________________________________
#ifdef KEIL
#include <intrins.h>
#define Wait_semaphore(a) while(!_testbit_(a))
#else
#define Wait_semaphore(a) while(!(a)) (a) = FALSE
#endif

//!_____ M A C R O S ________________________________________________________
//! Definition of Task ID. This ID is used to properly send the event to a
//! specific task.
//! Mind, it will be possible to send an event to many task by TASK_1 | TASK_0.
//! The name of the define can be changed by another define. That customization
//! should be done in the file mail_evt.h
#define TASK_DUMMY   0x00           // This define is mandatory
#define TASK_0       0x01
#define TASK_1       0x02
#define TASK_2       0x04
#define TASK_3       0x08
#define TASK_4       0x10
#define TASK_5       0x20
#define TASK_6       0x40
#define TASK_7       0x80

// This define is mandatory
#define ALL_TASK     (TASK_0|TASK_1|TASK_2|TASK_3|TASK_4|TASK_5|TASK_6|TASK_7)
//! End Task ID

//!----- Scheduler Types -----
#define SCHEDULER_CUSTOM      0
#define SCHEDULER_TIMED       1
#define SCHEDULER_TASK        2
#define SCHEDULER_FREE        3



#ifdef Scheduler_time_init
  extern  void Scheduler_time_init   (void);
#endif

#ifdef Scheduler_task_1_init
  extern  void Scheduler_task_1_init (void);
#else
  // if you do not want init at all, do:
  // #define Scheduler_task_1_init scheduler_empty_fct
  #error Scheduler_task_1_init must be defined in config.h file
#endif
#ifdef Scheduler_task_2_init
  extern  void Scheduler_task_2_init (void);
#endif
#ifdef Scheduler_task_3_init
  extern  void Scheduler_task_3_init (void);
#endif
#ifdef Scheduler_task_4_init
  extern  void Scheduler_task_4_init (void);
#endif
#ifdef Scheduler_task_5_init
  extern  void Scheduler_task_5_init (void);
#endif
#ifdef Scheduler_task_6_init
  extern  void Scheduler_task_6_init (void);
#endif
#ifdef Scheduler_task_7_init
  extern  void Scheduler_task_7_init (void);
#endif
#ifdef Scheduler_task_8_init
  extern  void Scheduler_task_8_init (void);
#endif
#ifdef Scheduler_task_9_init
  extern  void Scheduler_task_9_init (void);
#endif
#ifdef Scheduler_task_10_init
  extern  void Scheduler_task_10_init (void);
#endif
#ifdef Scheduler_task_11_init
  extern  void Scheduler_task_11_init (void);
#endif


#ifdef Scheduler_task_1
  extern  void Scheduler_task_1 (void);
#else
  // if you do not want task at all, do:
  // #define Scheduler_task_1 scheduler_empty_fct
  #error Scheduler_task_1 must be defined in config.h file
#endif
#ifdef Scheduler_task_2
  extern  void Scheduler_task_2 (void);
#endif
#ifdef Scheduler_task_3
  extern  void Scheduler_task_3 (void);
#endif
#ifdef Scheduler_task_4
  extern  void Scheduler_task_4 (void);
#endif
#ifdef Scheduler_task_5
  extern  void Scheduler_task_5 (void);
#endif
#ifdef Scheduler_task_6
  extern  void Scheduler_task_6 (void);
#endif
#ifdef Scheduler_task_7
  extern  void Scheduler_task_7 (void);
#endif
#ifdef Scheduler_task_8
  extern  void Scheduler_task_8 (void);
#endif
#ifdef Scheduler_task_9
  extern  void Scheduler_task_9 (void);
#endif
#ifdef Scheduler_task_10
  extern  void Scheduler_task_10 (void);
#endif
#ifdef Scheduler_task_11
  extern  void Scheduler_task_11 (void);
#endif

//!_____ D E F I N I T I O N ________________________________________________
#if SCHEDULER_TYPE != SCHEDULER_FREE
extern  bit   scheduler_tick_flag;
#endif

#ifdef TOKEN_MODE
extern Uchar token;
#define TOKEN_FREE      0
#endif

//!_____ D E C L A R A T I O N ______________________________________________
void scheduler_init         (void);
void scheduler_tasks        (void);
void scheduler              (void);
void scheduler_empty_fct    (void);

#ifndef SCHEDULER_TYPE
  #error You must define SCHEDULER_TYPE in config.h file
#elif SCHEDULER_TYPE == SCHEDULER_FREE
  #define Scheduler_set_tick_flag()
  #define Scheduler_reset_tick_flag()
#elif SCHEDULER_TYPE == SCHEDULER_TIMED
  #define Scheduler_new_schedule()      Wait_semaphore(scheduler_tick_flag)
  #define Scheduler_set_tick_flag()     (scheduler_tick_flag = TRUE)
  #define Scheduler_reset_tick_flag()   (scheduler_tick_flag = FALSE)
#elif SCHEDULER_TYPE == SCHEDULER_TASK
  #define Scheduler_call_next_task()    Wait_semaphore(scheduler_tick_flag)
  #define Scheduler_set_tick_flag()     (scheduler_tick_flag = TRUE)
  #define Scheduler_reset_tick_flag()   (scheduler_tick_flag = FALSE)
#elif SCHEDULER_TYPE == SCHEDULER_CUSTOM
  #error Make sure you have setup macro/fct Scheduler_new_schedule & Scheduler_call_next_task
  #define Scheduler_set_tick_flag()     (scheduler_tick_flag = TRUE)
  #define Scheduler_reset_tick_flag()   (scheduler_tick_flag = FALSE)
#endif

#ifndef Scheduler_call_next_task
  #define Scheduler_call_next_task()
#endif
#ifndef Scheduler_new_schedule
  #define Scheduler_new_schedule()
#endif
#ifndef Scheduler_call_next_init
  #define Scheduler_call_next_init()
#endif

#endif //! _SCHEDULER_H_

