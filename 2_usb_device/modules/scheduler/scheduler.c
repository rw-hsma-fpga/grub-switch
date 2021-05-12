/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the scheduler routines
//!
//! Configuration:
//!   - SCHEDULER_TYPE in scheduler.h header file
//!   - Task & init for at least task number 1 must be defined
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

//!_____ I N C L U D E S ____________________________________________________
#define _SCHEDULER_C_
#include "config.h"                         // system definition 
#include "conf/conf_scheduler.h"            // Configuration for the scheduler
#include "scheduler.h"                      // scheduler definition 


//!_____ M A C R O S ________________________________________________________
//!_____ D E F I N I T I O N ________________________________________________
#if SCHEDULER_TYPE != SCHEDULER_FREE
//! When SCHEDULER_TYPE != SCHEDULER_FREE, this flag control task calls.
bit   scheduler_tick_flag;
#endif

#ifdef TOKEN_MODE
//! Can be used to avoid that some tasks executes at same time. 
//! The tasks check if the token is free before executing. 
//! If the token is free, the tasks reserve it at the begin of the execution 
//! and release it at the end of the execution to enable next waiting tasks to 
//! run.	
Uchar token;
#endif

//!_____ D E C L A R A T I O N ______________________________________________
//! Scheduler initialization
//!
//! Task_x_init() and Task_x_fct() are defined in config.h
//!
//! @warning Code:XX bytes (function code length)
//!
//! @param  :none
//! @return :none
void scheduler_init (void)
{
   #ifdef Scheduler_time_init
      Scheduler_time_init();
   #endif
   #ifdef TOKEN_MODE
      token =  TOKEN_FREE;
   #endif
   #ifdef Scheduler_task_1_init
      Scheduler_task_1_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_2_init
      Scheduler_task_2_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_3_init
      Scheduler_task_3_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_4_init
      Scheduler_task_4_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_5_init
      Scheduler_task_5_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_6_init
      Scheduler_task_6_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_7_init
      Scheduler_task_7_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_8_init
      Scheduler_task_8_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_9_init
      Scheduler_task_9_init();  
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_10_init
      Scheduler_task_10_init();
      Scheduler_call_next_init();
   #endif
   #ifdef Scheduler_task_11_init
      Scheduler_task_11_init();
      Scheduler_call_next_init();
   #endif
   Scheduler_reset_tick_flag();
}

//! Task execution scheduler
//!
//! @warning Code:XX bytes (function code length)
//!
//! @param  :none
//! @return :none
void scheduler_tasks (void)
{
   // To avoid uncalled segment warning if the empty function is not used
   scheduler_empty_fct();

   for(;;)
   {
      Scheduler_new_schedule();
      #ifdef Scheduler_task_1
         Scheduler_task_1();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_2
         Scheduler_task_2();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_3
         Scheduler_task_3();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_4
         Scheduler_task_4();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_5
         Scheduler_task_5();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_6
         Scheduler_task_6();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_7
         Scheduler_task_7();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_8
         Scheduler_task_8();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_9
         Scheduler_task_9();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_10
         Scheduler_task_10();
         Scheduler_call_next_task();
      #endif
      #ifdef Scheduler_task_11
         Scheduler_task_11();
         Scheduler_call_next_task();
      #endif
   }
}

//! Init & run the scheduler
//!
//! @warning Code:XX bytes (function code length)
//!
//! @param  :none
//! @return :none
void scheduler (void)
{
   scheduler_init();
   scheduler_tasks();
}


//! Do nothing
//! Avoid uncalled segment warning if the empty function is not used
//!
//! @warning Code:XX bytes (function code length)
//!
//! @param  :none
//! @return :none
void scheduler_empty_fct (void)
{
}

