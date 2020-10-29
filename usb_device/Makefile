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
TARGET = $(PROJECT).elf
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

# Eeprom file production flags
HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0

# Include Directories
INCLUDES =  -I"conf" -I"main" -I"./"

# Include Source files list
include mk/sources.mk
include boards/boards.mk

## Build
#allboards: 

.PHONY: all
all: $(TARGET) $(PROJECT).hex size
	@echo "Target board: $(BOARD)"
	@echo ""

## Clean target
.PHONY: clean
clean:
	@echo "Clean project"
	@-rm -rf $(OUTPUT)/dep/* $(OUTPUT)/* $(TARGET_NAME)*.elf $(TARGET_NAME)*.hex $(TARGET_NAME)*.eep $(TARGET_NAME)*.map
	@-rm -rf ../bootfiles/boot.*

## Rebuild the project.
.PHONY: rebuild
rebuild: clean all


## Create/add empty missing bootfiles and dirs
BOOTFILES_DIRS = 1 2 3 4 5 6 7 8 9 A B C D E F

bootfiles: $(BOOTFILES_DIRS)
$(BOOTFILES_DIRS):
	@$(shell mkdir -p ../bootfiles/boot.$@ 2>/dev/null)
	@$(shell touch ../bootfiles/boot.$@/SWITCH.GRB 2>/dev/null)

## Compile

# Create objects files list with sources files
OBJECTS  = $(CSRCS:.c=.o) $(ASSRCS:.S=.o)

.PHONY: objfiles
objfiles: $(OBJECTS)

# create object files from C source files.
%.o: %.c
	@echo 'Building file: $<'
	@$(shell mkdir $(OUTPUT) 2>/dev/null)
	@$(shell mkdir $(OUTPUT)/dep 2>/dev/null)
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(OUTPUT)/$(@F)
	
# Preprocess & assemble: create object files from assembler source files.
%.o: %.S $(BOOTFILES)
	@echo 'Building file: $<'
	@$(shell mkdir $(OUTPUT) 2>/dev/null)
	@$(shell mkdir $(OUTPUT)/dep 2>/dev/null)
	@$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $(OUTPUT)/$(@F)


## Link
$(TARGET): bootfiles $(OBJECTS)
	@echo "Linking"
	@$(CC) $(LDFLAGS) $(addprefix $(OUTPUT)/,$(notdir $(OBJECTS))) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	@echo "Create hex file"
	@avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	@echo "Create eep file"
	@avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@  || exit 0

%.lss: $(TARGET)
	@echo "Create lss file"
	@avr-objdump -h -S $< > $@

size: ${TARGET}
	@avr-size -C --mcu=${MCU} ${TARGET}

