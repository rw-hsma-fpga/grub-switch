##############################################################################
# Generic Makefile for following parts AT90USBx ATMegaxUx
###############################################################################

OUTPUT = build

# default board choice
BOARD = TEENSY20

# General Flags
CC = avr-gcc
MCU = atmega32u4
TARGET_NAME = GRUB_SWITCH

PROJECT = $(TARGET_NAME)_$(BOARD)
#BOARDCFG = $(BOARD).mk


# Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -DTARGET_BOARD_$(BOARD)

# Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -fsigned-char -ffunction-sections
CFLAGS += -Wno-unused-but-set-variable -Wunused-function
CFLAGS += -MD -MP -MT $(OUTPUT)/$(*F).o -MF $(OUTPUT)/dep/$(@F).d 

# Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

# Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(PROJECT).map,--cref,--gc-sections,--relax

# Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

# Include Directories
INCLUDES =  -I"conf" -I"main" -I"./"

# Include Source files list
include mk/sources.mk
# Create objects files list with sources files
OBJECTS  = $(CSRCS:.c=.o) $(ASSRCS:.S=.o)

#include boards/boards.mk

## Build
#allboards: 

.PHONY: all
all: $(PROJECT).elf $(PROJECT).hex size
	@echo "Target built: $(BOARD)"
	@echo ""

## Clean target
.PHONY: clean
clean:
	@echo "Clean project"
	@-rm -rf $(OUTPUT)/dep/* $(OUTPUT)/* $(TARGET_NAME)*.elf $(TARGET_NAME)*.hex $(TARGET_NAME)*.eep $(TARGET_NAME)*.map

## Rebuild the project.
.PHONY: rebuild
rebuild: clean all


## Compile

.PHONY: objfiles
objfiles: $(OBJECTS)

# create object files from C source files.
%.o: %.c
	@echo 'Compiling file: $<'
	@$(shell mkdir $(OUTPUT) 2>/dev/null)
	@$(shell mkdir $(OUTPUT)/dep 2>/dev/null)
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(OUTPUT)/$(@F)

# Preprocess & assemble: create object files from assembler source files.
%.o: %.S
	@echo 'Assembling file: $<'
	@$(shell mkdir $(OUTPUT) 2>/dev/null)
	@$(shell mkdir $(OUTPUT)/dep 2>/dev/null)
	@$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $(OUTPUT)/$(@F)


## Link #$(OBJECTS)
$(PROJECT).elf: $(OBJECTS)
	@echo "Linking"
	@$(CC) $(LDFLAGS) $(addprefix $(OUTPUT)/,$(notdir $(OBJECTS))) -o $(PROJECT).elf


%.hex: $(PROJECT).elf
	@echo "Create hex file"
	@avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

size: $(PROJECT).elf
	@avr-size -C --mcu=${MCU} $(PROJECT).elf

