#ifndef TEENSY20_H
#define TEENSY20_H


#include "config.h"
#ifdef TARGET_BOARD_TEENSY20

#define VALID_TARGET_BOARD_DEFINED


// External quartz frequency (kHz) -> CPU core frequency
#define FOSC 16000

#define  LED_PORT             PORTD
#define  LED_DDR              DDRD
#define  LED_BIT              PIND6


#define  Led_init()          (LED_DDR |= (1<<LED_BIT) )

#define  Led_on()            (LED_PORT |=  (1<<LED_BIT))
#define  Led_off()           (LED_PORT &= ~(1<<LED_BIT))



#define  MODE_PORT          B
#define  MODE_PIN           0

#define  CHOICE_PORT1       B
#define  CHOICE_PIN1        1

#define  CHOICE_PORT2       B
#define  CHOICE_PIN2        2

#define  CHOICE_PORT3       B
#define  CHOICE_PIN3        3

#define  CHOICE_PORT4       B
#define  CHOICE_PIN4        7

#define  CHOICE_PORT5       D
#define  CHOICE_PIN5        0

#define  CHOICE_PORT6       D
#define  CHOICE_PIN6        1

#define  CHOICE_PORT7       D
#define  CHOICE_PIN7        2

#define  CHOICE_PORT8       D
#define  CHOICE_PIN8        3

#define  CHOICE_PORT9       C
#define  CHOICE_PIN9        6

#define  CHOICE_PORT10      C
#define  CHOICE_PIN10       7

#define  CHOICE_PORT11      D
#define  CHOICE_PIN11       5


// resolve choice pin macros
#define RESOLVE_PORT(x) CHOICE_PORT##x
#define RESOLVE_PIN(x)  CHOICE_PIN##x

// setting input directions and pullup
#define SIP_PASTE(x,y)          DDR##x &= ~(1 << PIN##x##y); PORT##x |= (1 << PIN##x##y)
#define SIP_EVAL(x,y)           SIP_PASTE(x,y)
#define set_mode_input_pullup()  SIP_EVAL( MODE_PORT , MODE_PIN )
#define set_choice_input_pullup(x)  SIP_EVAL(RESOLVE_PORT(x),RESOLVE_PIN(x))

// read input pins
#define RP_PASTE(x,y)       ((PIN##x >> PIN ##x##y & 0x01))
#define RP_EVAL(x,y)        RP_PASTE(x,y)
#define read_mode_pin()     RP_EVAL( MODE_PORT , MODE_PIN )
#define read_choice_pin(x)  RP_EVAL(RESOLVE_PORT(x),RESOLVE_PIN(x))





#endif   // TARGET_BOARD==TEENSY20

#endif   // TEENSY20_H

