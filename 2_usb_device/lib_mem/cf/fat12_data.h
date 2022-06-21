#ifndef _FAT12_DATA_H_
#define _FAT12_DATA_H_

#include "config.h"
#include <avr/eeprom.h>

// Attributes of fake FAT12 partition
#define CF_FAT12_SECTORS            66
#define CF_FAT12_SECTOR_SIZE        512
#define CF_LOG2_FAT12_SECTOR_SIZE   9
#define CF_FAT12_WRITE_PROTECTED    FALSE

// file and directory entry positions
#define SWITCH_GRB_ADDR 0x0A00
#define SWITCH_GRB_SIZE_ADDR 0x043c

#define ENTRIES_TXT_ADDR 0x0E00
#define ENTRIES_TXT_SIZE_ADDR 0x047c

#define BOOTPINS_TXT_ADDR 0x1200

// .entries.txt file parameters
#define MAX_NUM_ENTRIES 16
#define HIGHLIGHT_COLOR_LENGTH 16
#define SLEEP_SECS_LENGTH 4

// .entries.txt EEPROM STORAGE
#ifdef __AVR_ATmega32U4__
   #define ENTRIES_TXT_MAX_SIZE 960
   #define ENTRIES_TXT_SIZE_EEP_ADDR 1022
#else
   #ifdef __AVR_ATmega16U4__
      #define ENTRIES_TXT_MAX_SIZE 448
      #define ENTRIES_TXT_SIZE_EEP_ADDR 510
   #else
      #error NO_SUPPORTED_CHIP
   #endif
#endif



// unchanged bootsector data 0x0000..0x00bf
extern const code unsigned char sector0[0xC0];

// unchanged partition table and cluster map data 0x01fe..0x0208
extern const code unsigned char pt_cluster[0xb];

// fixed directory entries 0x0400..0x4bf
extern const code unsigned char dir_table[0xc0];

U16 get_entryfile_size();
void set_entryfile_size(U16 file_size);

#define read_entry_file(i) eeprom_read_byte((U8*)i)
void parse_entry_file();

void build_bootfile_parameters(U8 choice);
U16 get_bootfile_size();

U8 read_file_SWITCH_GRB(U16 offset);
U8 read_file_BOOTPINS_TXT(U16 offset);

#endif  // _FAT12_DATA_H_