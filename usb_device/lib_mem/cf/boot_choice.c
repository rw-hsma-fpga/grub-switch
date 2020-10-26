#include "config.h"
#include "lib_mcu/usb/usb_drv.h"
#include "boot_choice.h"
#include <avr/interrupt.h>

static U16 sample_choice_pins(void);
static void filter_choice_pins(void);

static pin_mode choice_mode;

static U16 last_choice_pins;
static U16 current_choice_pins;
static U16 pinSamples[3];

static U16 detach_countdown;

// prepare port pins for boot choice
void boot_choice_init(void)
{
    // mode and choice pins are all inputs

    set_mode_input_pullup();

    set_choice_input_pullup(1);
    set_choice_input_pullup(2);
    set_choice_input_pullup(3);
    set_choice_input_pullup(4);



    if (read_mode_pin()==0) // tied to GND
    {
        choice_mode = Binary;
    }
    else
    {
        choice_mode = OneOfN;

        set_choice_input_pullup(5);
        set_choice_input_pullup(6);
        set_choice_input_pullup(7);
        set_choice_input_pullup(8);
        set_choice_input_pullup(9);
        set_choice_input_pullup(10);
        set_choice_input_pullup(11);
    }
    
    // fill variables with initial pattern
    current_choice_pins = sample_choice_pins();
    last_choice_pins    = current_choice_pins;
    pinSamples[0]      = current_choice_pins;
    pinSamples[1]      = current_choice_pins;
    pinSamples[2]      = current_choice_pins;

    // no USB detach for now
    detach_countdown = 0;

    // IRQ setup
    Disable_interrupt();

        // set timer0 counter initial value to 0
        TCNT0 = 0x00;

        // compare match A for 5msec tick
        OCR0A = (FOSC/1000) * 5;

        // clear timer on compare match A
        TCCR0A = (1 << WGM01);
        // start timer0 with /1024 prescaler
        TCCR0B = (1 << CS02) | (1 << CS00);

        // enable compare match interrupt A for Timer0
        TIMSK0 = (1 << OCIE0A);

    Enable_interrupt();

    return;
}


ISR(TIMER0_COMPA_vect)
{
    filter_choice_pins();
    
    /*
    static U8 led_countdown = 200; // 5ms * 200 = 1 sec

    if (led_countdown==0)
    {
        led_countdown = 200;

        if (Debug_Led_is_on())
            Debug_Led_off();
        else
            Debug_Led_on();
    }

    led_countdown--;
    */
}


static U16 sample_choice_pins(void)
{
    U16 all_pins = 0;

    // assemble pin word
    if (choice_mode==OneOfN)
    {
        all_pins |= (read_choice_pin(11));
        all_pins <<= 1;
        all_pins |= (read_choice_pin(10));
        all_pins <<= 1;
        all_pins |= (read_choice_pin(9));
        all_pins <<= 1;
        all_pins |= (read_choice_pin(8));
        all_pins <<= 1;
        all_pins |= (read_choice_pin(7));
        all_pins <<= 1;
        all_pins |= (read_choice_pin(6));
        all_pins <<= 1;
        all_pins |= (read_choice_pin(5));
        all_pins <<= 1;

        all_pins ^= 0x00FE; // invert pins so far - '1' now means tied to ground
    }
    // used in both modes
    all_pins |= (read_choice_pin(4));
    all_pins <<= 1;
    all_pins |= (read_choice_pin(3));
    all_pins <<= 1;
    all_pins |= (read_choice_pin(2));
    all_pins <<= 1;
    all_pins |= (read_choice_pin(1));

    all_pins ^= 0x000F; // invert lowest 4 pins - '1' now means tied to ground

    return all_pins;
}

// should be called from Timer IRQ (every 5ms)
static void filter_choice_pins(void)
{
    // shift data and acquire new sample
    pinSamples[2] = pinSamples[1];
    pinSamples[1] = pinSamples[0];
    pinSamples[0] = sample_choice_pins();

    // detect 3x'1' and adjust current choice
    U16 samplesANDed = pinSamples[2] & pinSamples[1] & pinSamples[0];
    current_choice_pins = last_choice_pins | samplesANDed; // set '1' where three samples were '1'

    // detect 3x'0' and adjust current choice
    U16 samplesORed = pinSamples[2] & pinSamples[1] & pinSamples[0];
    current_choice_pins = current_choice_pins & samplesORed; // set '0' where three samples were '0'

    // stable change triggers USB detach and wait
    if (current_choice_pins != last_choice_pins)
    {
        detach_countdown = 41; // 40x 5msecs = 200msecs
    }
    // make stable sample the new normal
    last_choice_pins = current_choice_pins;

    // Keep USB detached and count down
    if (detach_countdown != 0)
    {
        Usb_detach();

        detach_countdown--;
        if (detach_countdown==0)
            Usb_attach();
    }

}

// TODO: Change to current_choice_pins
U8 get_boot_choice(void)
{
    U8 choice;
    U16 all_pins = 0;

    // assemble pin word
    all_pins = current_choice_pins;

    if (choice_mode==OneOfN)
    {
        for (choice = 1; choice<12; choice++)
        {
            if (all_pins & 0x1)
                return choice; // return first pin tied to ground
            all_pins >>= 1;
        }

        return 0; // no pin tied to GND
    }
    else // binary mode - first 4 bits are boot choice in binary form (0x0..0xF) 
        return (all_pins &= 0x0F);
}

U16 get_raw_boot_pins(void)
{
    return current_choice_pins;
}

pin_mode get_choice_mode(void)
{
    return choice_mode;    
}

