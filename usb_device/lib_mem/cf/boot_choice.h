#ifndef _BOOT_CHOICE_H
#define _BOOT_CHOICE_H

#include "config.h"

typedef enum { Binary, OneOfN } pin_mode;

// prepare port pins for boot choice
void boot_choice_init(void);
U8 get_boot_choice(void);
U8 get_raw_boot_pins(void);
pin_mode get_choice_mode(void);

#endif // _BOOT_CHOICE_H
