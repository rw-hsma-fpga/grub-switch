#ifndef _BOARDS_H_
#define _BOARDS_H_

// add additional board header files here
#include "boards/include/teensy20.h"
#include "boards/include/grubsw_custom.h"


#ifndef VALID_TARGET_BOARD_DEFINED
   #error NO VALID TARGET BOARD SPECIFIED
#endif



// helper macros - resolve choice pin from index
#define RESOLVE_PORT(x) CHOICE_PORT##x
#define RESOLVE_PIN(x)  CHOICE_PIN##x

// helper macros - setting input direction and pullup
#define SIP_PASTE(x,y)          DDR##x &= ~(1 << PIN##x##y); PORT##x |= (1 << PIN##x##y)
#define SIP_EVAL(x,y)           SIP_PASTE(x,y)

// helper macros - setting output direction
#define SOP_PASTE(x,y)          DDR##x |= (1 << PIN##x##y)
#define SOP_EVAL(x,y)           SOP_PASTE(x,y)

// helper macros - read input pins
#define RP_PASTE(x,y)       ((PIN##x >> PIN ##x##y & 0x01))
#define RP_EVAL(x,y)        RP_PASTE(x,y)

// helper macros - write output pins
#define W0P_PASTE(x,y)       PORT##x &= ~(1 << PIN##x##y)
#define W0P_EVAL(x,y)        W0P_PASTE(x,y)
#define W1P_PASTE(x,y)       PORT##x |= (1 << PIN##x##y)
#define W1P_EVAL(x,y)        W1P_PASTE(x,y)



// set pins input and pullup macros
#define set_mode_input_pullup()  SIP_EVAL( MODE_PORT , MODE_PIN )
#define set_wrprot_input_pullup()  SIP_EVAL( WRPROT_PORT , WRPROT_PIN )
#define set_choice_input_pullup(x)  SIP_EVAL(RESOLVE_PORT(x),RESOLVE_PIN(x))

// read input pin macros
#define read_mode_pin()     RP_EVAL( MODE_PORT , MODE_PIN )
#define read_wrprot_pin()   RP_EVAL( WRPROT_PORT , WRPROT_PIN )
#define read_choice_pin(x)  RP_EVAL(RESOLVE_PORT(x),RESOLVE_PIN(x))

// LED pin macros
#define  Led_init()          SOP_EVAL( LED_PORT , LED_PIN )
#define  Led_on()            W1P_EVAL( LED_PORT , LED_PIN )
#define  Led_off()           W0P_EVAL( LED_PORT , LED_PIN )



#endif // _BOARDS_H_

