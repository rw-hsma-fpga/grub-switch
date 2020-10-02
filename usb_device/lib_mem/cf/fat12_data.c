#include "fat12_data.h"
#include "boot_choice.h"

// unchanged bootsector data 0x0000..0x00bf
const code unsigned char sector0[0xC0] = {
    0xeb, 0x3c, 0x90, 0x6d, 0x6b, 0x66, 0x73, 0x2e, 0x66, 0x61, 0x74, 0x00, 0x02, 0x02, 0x01, 0x00,
    0x01, 0x10, 0x00, 0x44, 0x00, 0xf8, 0x01, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x29, 0x55, 0x19, 0x85, 0x19, 0x42, 0x4f, 0x4f, 0x54, 0x54,
    0x48, 0x49, 0x53, 0x20, 0x20, 0x20, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x0e, 0x1f,
    0xbe, 0x5b, 0x7c, 0xac, 0x22, 0xc0, 0x74, 0x0b, 0x56, 0xb4, 0x0e, 0xbb, 0x07, 0x00, 0xcd, 0x10,
    0x5e, 0xeb, 0xf0, 0x32, 0xe4, 0xcd, 0x16, 0xcd, 0x19, 0xeb, 0xfe, 0x54, 0x68, 0x69, 0x73, 0x20,
    0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6f, 0x6f, 0x74, 0x61, 0x62, 0x6c,
    0x65, 0x20, 0x64, 0x69, 0x73, 0x6b, 0x2e, 0x20, 0x20, 0x50, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x20,
    0x69, 0x6e, 0x73, 0x65, 0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6f, 0x6f, 0x74, 0x61, 0x62, 0x6c,
    0x65, 0x20, 0x66, 0x6c, 0x6f, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6e, 0x64, 0x0d, 0x0a, 0x70, 0x72,
    0x65, 0x73, 0x73, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x74, 0x6f, 0x20, 0x74,
    0x72, 0x79, 0x20, 0x61, 0x67, 0x61, 0x69, 0x6e, 0x20, 0x2e, 0x2e, 0x2e, 0x20, 0x0d, 0x0a, 0x00
};

// inbetween all 0x00

// unchanged partition table and cluster map data 0x01fe..0x0208
const code unsigned char pt_cluster[0xb] = {
    0x55, 0xaa, 0xf8, 0xff, 0xff, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff
};

// inbetween all 0x00

// fixed directory entries 0x0400..0x4bf ... 6 x 32-Byte entries incl. 2 x 64-Byte b/c of VFAT filename
const code unsigned char dir_table[0xc0] = {
//  B     O     O     T     T     H     I     S                       | Flags:V
    0x42, 0x4f, 0x4f, 0x54, 0x54, 0x48, 0x49, 0x53, 0x20, 0x20, 0x20, 0x08, 0x00, 0x00, 0x80, 0x0a,
    0x5a, 0x0b, 0x5a, 0x0b, 0x00, 0x00, 0x80, 0x0a, 0x5a, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

//  S     W     I     T     C     H                 G     R     B     | Flags:AS.R
    0x53, 0x57, 0x49, 0x54, 0x43, 0x48, 0x20, 0x20, 0x47, 0x52, 0x42, 0x23, 0x00, 0x00, 0xc0, 0x0a,
    0x5a, 0x0b, 0x5a, 0x0b, 0x00, 0x00, 0xc0, 0x0a, 0x5a, 0x0b, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                        //  | Size: 4 bytes - replaced
//  .entries.txt
    0x41, 0x2e, 0x00, 0x65, 0x00, 0x6e, 0x00, 0x74, 0x00, 0x72, 0x00, 0x0f, 0x00, 0x0B, 0x69, 0x00,
    0x65, 0x00, 0x73, 0x00, 0x2E, 0x00, 0x74, 0x00, 0x78, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00,
    // E     N     T     R     I     E     ~     1     T     X     T  | Flags:ASH. (ASHR when write protect switched on)
    0x45, 0x4e, 0x54, 0x52, 0x49, 0x45, 0x7e, 0x31, 0x54, 0x58, 0x54, 0x26, 0x00, 0x00, 0xa0, 0x0a,
    0x5a, 0x0b, 0x5a, 0x0b, 0x00, 0x00, 0xa0, 0x0a, 0x5a, 0x0b, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                        //  | Size: 4 bytes - replaced
//  .bootpins.txt
    0x41, 0x2e, 0x00, 0x62, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x74, 0x00, 0x0f, 0x00, 0x14, 0x70, 0x00,
    0x69, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x2E, 0x00, 0x74, 0x00, 0x00, 0x00, 0x78, 0x00, 0x74, 0x00,
    // B     O     O     T     P     I     ~     1     T     X     T  | Flags:ASHR
    0x42, 0x4F, 0x4F, 0x54, 0x50, 0x49, 0x7e, 0x31, 0x54, 0x58, 0x54, 0x27, 0x00, 0x00, 0x80, 0x0a,
    0x5a, 0x0b, 0x5a, 0x0b, 0x00, 0x00, 0x80, 0x0a, 0x5a, 0x0b, 0x05, 0x00, 0x12, 0x00, 0x00, 0x00
                                                                        //  | Size: is 18
};

 // IN-BETWEEN
 // low-endian file size of SWITCH.GRB 0x43c..0x43d - from progmem table
 // low-endian file size of .entries.txt 0x47c..0x47d - last two EEPROM bytes

 // AFTER:
 // SWITCH.GRB starting at 0x0A00 - from progmem
 // .entries   starting at 0x0E00 - RAM copy of EEPROM
 // .status    starting at 0x1200 - 18-byte switch status
 // rest all 0x00

static U16 entry_start[MAX_NUM_ENTRIES];
static U16 entry_length[MAX_NUM_ENTRIES];

static unsigned char highlight_color[HIGHLIGHT_COLOR_LENGTH] = "";
static unsigned char sleep_secs[SLEEP_SECS_LENGTH] = "";

static char bootfile_parameters[200]; // for longest colors, 3-digit sleep and 80-character menuentry titles;
                               // extend if more parameters become available 

extern U8 bootfile_template_start;
extern U8 bootfile_template_end;

static U16 bootfile_parameters_size;
static U16 bootfile_template_size; 

static U16 complete_bootfile_size;

static U16 entry_file_size;


U16 get_entryfile_size()
{
   return entry_file_size;
}


void set_entryfile_size(U16 file_size)
{
   entry_file_size = file_size;
}


U16 get_bootfile_size()
{
    return complete_bootfile_size;
}

void parse_entry_file()
{
   U16 entry_file_pos = 0;

   U16 i; // general iterator
   U8  entry_index = 1; // line iterator

   // reset values to parse
   for(i=0; i<MAX_NUM_ENTRIES; i++)
   {
      entry_start[i] = 0;
      entry_length[i] = 0;
   }
   for(i=0; i<HIGHLIGHT_COLOR_LENGTH; i++)
      highlight_color[i] = '\0';
   for(i=0; i<SLEEP_SECS_LENGTH; i++)
      sleep_secs[0] = '\0';
 
   // despite some checking, only well-formed files will reasonably work
 
   do // linewise loop
   {
      // start of line
      entry_start[entry_index] = entry_file_pos; // init next entry to come up
      U16 line_start = entry_file_pos;

      char sample;

      do
      {
         sample = '\0';
         if (entry_file_pos == entry_file_size)
            break;

         sample = read_entry_file(entry_file_pos++);
      } while ((sample!='\r') && (sample!='\n'));

      if (sample == '\0') // indicates file end
         break;

      U16 line_end = entry_file_pos - 1;

      if (sample=='\r') // windows-style line end, \n should follow
      {
         if (entry_file_pos == entry_file_size)
            break;
         else
            entry_file_pos++; // skip \n
      }

      // process line
      if (read_entry_file(line_start)=='#') // special parameters
      {
         U8 par_pos = 0; 
         sample = read_entry_file(line_start+1);
         if (sample=='1') // sleep secs
         {
            while(1)
            {
               sample = read_entry_file(line_start+3+par_pos);
               if ( (sample=='#') || (sample==' ') || (sample=='\r') || (sample=='\n') ||
                    (par_pos==SLEEP_SECS_LENGTH-1) )
                    break;
               sleep_secs[par_pos] = sample;
               par_pos++;
            }
         }
         else if (sample=='2') // highlight color
         {
            while(1)
            {
               sample = read_entry_file(line_start+3+par_pos);
               if ( (sample=='#') || (sample==' ') || (sample=='\r') || (sample=='\n') ||
                    (par_pos==HIGHLIGHT_COLOR_LENGTH-1) )
                    break;
               highlight_color[par_pos] = sample;
               par_pos++;
            }
         }
         continue; // next line
      }
      else // menu entry line (can be empty line);
      {
         entry_length[entry_index] = line_end - line_start;
         entry_index++;
         continue; // next line
      }
   } while ( (entry_index < MAX_NUM_ENTRIES) &&         // not max number of entries parsed yet
             (entry_file_pos != entry_file_size) );   // not end of file yet
} 


void build_bootfile_parameters(U8 choice)
{
   const char bootfile_string1[] = "grubswitch_sleep_secs='";
   const char bootfile_string2[] = "'\r\ngrubswitch_choice_color='";
   const char bootfile_string3[] = "'\r\ngrubswitch_choice='";
   const char bootfile_string4[] = "'\r\n";

   U16 i;

   if ((choice==0) || (entry_length[choice]==0))
   {
      bootfile_parameters_size = 0;
      complete_bootfile_size = 0;
      return;
   }

   bootfile_parameters_size = 0;

   for(i=0; bootfile_string1[i]!='\0'; i++)
      bootfile_parameters[bootfile_parameters_size++] = bootfile_string1[i];

   for(i=0; sleep_secs[i]!='\0'; i++)
      bootfile_parameters[bootfile_parameters_size++] = sleep_secs[i];

   for(i=0; bootfile_string2[i]!='\0'; i++)
      bootfile_parameters[bootfile_parameters_size++] = bootfile_string2[i];

   for(i=0; highlight_color[i]!='\0'; i++)
      bootfile_parameters[bootfile_parameters_size++] = highlight_color[i];

   for(i=0; bootfile_string3[i]!='\0'; i++)
      bootfile_parameters[bootfile_parameters_size++] = bootfile_string3[i];

   for(i=0; i<entry_length[choice]; i++)
      bootfile_parameters[bootfile_parameters_size++] = read_entry_file(entry_start[choice] + i);

   for(i=0; bootfile_string4[i]!='\0'; i++)
      bootfile_parameters[bootfile_parameters_size++] = bootfile_string4[i];

    bootfile_template_size = &bootfile_template_end - &bootfile_template_start;
    complete_bootfile_size = bootfile_parameters_size + bootfile_template_size;
}


U8 read_file_SWITCH_GRB(U16 offset)
{
    if ((offset) < bootfile_parameters_size)
        return bootfile_parameters[offset];
    else
    {
        offset -= bootfile_parameters_size;
    }
        return pgm_read_byte((&bootfile_template_start) + offset);
}


U8 read_file_BOOTPINS_TXT(U16 offset)
{
   if (get_choice_mode() == Binary) // binary mode; show 4 bits and BIN suffix
   {
      const unsigned char rest[] = "Binary pick:  ";
      if (offset < 14)
         return rest[offset];
      else
         return ((get_raw_boot_pins() >> (3 - (offset - 14)) ) & 0x01) + 0x30;
   }
   else // 1-aus-n mode; show all 11 selector bits
   {
      const unsigned char rest[] = "11->1: ";
      if (offset < 7)
         return rest[offset];
      else                     
         return ((get_raw_boot_pins() >> (10 - (offset - 7))) & 0x01) + 0x30;
   }
}
