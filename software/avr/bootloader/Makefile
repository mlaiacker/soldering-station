# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005

# version
BUILD      = 120515

# program name should not be changed...
PROGRAM    = max-boot

# enter the product name for which you want to build the bootloader/monitor
PRODUCT    = MEGA8
#PRODUCT    = MEGA168
#PRODUCT    = CRUMB8
#PRODUCT    = CRUMB128
#PRODUCT    = CRUMB168
#PRODUCT    = PROBOMEGA128
#PRODUCT    = SAVVY128

# enter the target CPU frequency
#AVR_FREQ   = F3686400
#AVR_FREQ   = F7372800
AVR_FREQ   = F8000000
#AVR_FREQ   = F14745600
#AVR_FREQ   = F16000000
#AVR_FREQ   = F20000000

# enter the parameters for the UISP isp tool
# for an stk500 at com1
ISPTOOL	   = stk500v2
ISPPORT	   = com55
ISPSPEED   = -b 115200
# or an stk200 at lpt1
#ISPTOOL    = stk200
#ISPPORT    = lpt1
#ISPSPEED   =


############################################################
# You should not have to change anything below here.
############################################################


ifeq ($(PRODUCT),MEGA8)
MCU_TARGET = atmega8
LDSECTION  = --section-start=.text=0x1c00
ISPFUSES    = avrdude -c $(ISPTOOL) -p m8 -P $(ISPPORT) $(ISPSPEED) -V -U hfuse:w:0xd2:m -U lfuse:w:0x24:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m8 -P $(ISPPORT) $(ISPSPEED) -U flash:w:$(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).hex
endif

ifeq ($(PRODUCT),MEGA168)
MCU_TARGET = atmega168
LDSECTION  = --section-start=.text=0x3800
#ISPFUSES    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:r:-:h -U hfuse:r:-:h -U lfuse:r:-:h
#ISPFUSES    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0x00:m -U hfuse:w:0xd4:m -U lfuse:w:0xd7:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -U flash:w:$(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).hex
endif

ifeq ($(PRODUCT),CRUMB8)
MCU_TARGET = atmega8
LDSECTION  = --section-start=.text=0x1800
ISPFUSES    = avrdude -c $(ISPTOOL) -p m8 -P $(ISPPORT) $(ISPSPEED) -u -U hfuse:w:0xc8:m -U lfuse:w:0xdf:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m8 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex
endif

ifeq ($(PRODUCT),CRUMB168)
MCU_TARGET = atmega168
LDSECTION  = --section-start=.text=0x3800
ISPFUSES    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0xf8:m -U hfuse:w:0xd7:m -U lfuse:w:0xaf:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex
endif

ifeq ($(PRODUCT),CRUMB128)
MCU_TARGET = atmega128
LDSECTION  = --section-start=.text=0x1E000
ISPFUSES    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0xff:m -U hfuse:w:0xc8:m -U lfuse:w:0xdf:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex
endif

ifeq ($(PRODUCT),PROBOMEGA128)
MCU_TARGET = atmega128
LDSECTION  = --section-start=.text=0x1E000
ISPFUSES    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0xff:m -U hfuse:w:0xc8:m -U lfuse:w:0xdf:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex
endif

ifeq ($(PRODUCT),SAVVY128)
MCU_TARGET = atmega128
LDSECTION  = --section-start=.text=0x1E000
ISPFUSES    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0xff:m -U hfuse:w:0xd8:m -U lfuse:w:0xe4:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex
endif

MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:

SIZE = avr-size
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(PROGRAM).elf


OBJ        = $(PROGRAM).o
OPTIMIZE   = -Os

DEFS       =
LIBS       =

CC         = avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -D$(PRODUCT) -D$(AVR_FREQ) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)


OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).elf sizeafter lst text

program: $(PROGRAM).hex
	$(ISPFUSES)
	$(ISPFLASH)

.PHONY: program_all
program_all: 
	@cat ../main_$(MCU_TARGET).hex | gawk '/^:00000001FF/ == 0' > $(PROGRAM)_main.hex
	@cat $(PROGRAM).hex >> $(PROGRAM)_main.hex
	cp $(PROGRAM)_main.hex $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).hex
#	@$(AVR_SIZE) $@
	$(ISPFUSES)
	$(ISPFLASH)



	
$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep
	rm -rf $(PROGRAM).hex $(PROGRAM).srec $(PROGRAM).bin

lst:  $(PROGRAM).lst

# Display size of file.

sizebefore: $(PROGRAM).elf 
	@echo $(MSG_SIZE_BEFORE) 
	@$(ELFSIZE)

sizeafter: $(PROGRAM).elf
	@echo $(MSG_SIZE_AFTER)
	@$(ELFSIZE)


%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PROGRAM).hex
bin:  $(PROGRAM).bin
srec: $(PROGRAM).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).hex

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).srec

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).bin
