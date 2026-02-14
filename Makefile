# Project Name
TARGET = main
BOOT=bootloader2
# Toolchain Prefix
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size


# Source Files
# SRCS = main.c startup.s
SRCS = main.c startupMX.s

# Object Files
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

# Compiler Flags
CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g \
         -Wall -Wextra \
         -ffreestanding -nostdlib 
	
# Linker Flags
# LDFLAGS = -T linker.ld -nostdlib 
LDFLAGS = -T linkerMX.ld -nostdlib 

# Build Targets
all:$(TARGET).elf $(TARGET).bin
# all:$(TARGET).elf $(TARGET).bin $(TARGET).hex

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble startup file
%.o: %.s
	$(AS) -mcpu=cortex-m3 -mthumb $< -o $@

# Link object files
$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	$(SIZE) $@
# Generate binary file
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	$(OBJCOPY) -O ihex ${TARGET}.elf ${TARGET}.hex

# Clean build files
clean:
	rm -f *.o *.elf *.hex *.lst *.bin
	clear
#info:
#	st-info --flash 
#	st-info --sram 
#	st-info --descr 
#	st-info --serial 
#	st-info --probe

# STM32 flash
fts:
#	TX A10 RX A9
	make clean
	make
	stm32flash -w $(TARGET).elf -v -g 0x08000000  /dev/ttyUSB0
# 	stm32flash -n 4 -R -v -w $(TARGET).bin -s 0x08000000 /dev/ttyUSB0

# Flash to STM32 using st-flash
flash: #$(TARGET).bin
#	st-flash erase 
	make clean
	make 
	st-flash --connect-under-reset write $(TARGET).bin 0x08000000
# 	st-flash write $(TARGET).bin 0x08000000
# 	st-flash read $(TARGET).txt 0x08000000 0x02000
flash_dfu:
	st-flash --connect-under-reset write /home/jeff/Downloads/stm32f1_dfu.bin 0x08000000
dfu:
	make clean
	make
	dfu-util -a 0 -s 0x08000000:leave -D ${TARGET}.bin

# Disassemble for inspection
disasm: $(TARGET).elf
	$(OBJDUMP) -D $(TARGET).elf > $(TARGET).lst

#OBJDUMP=arm-none-eabi-objdump -d ${FILE}.bin
#HEXER=arm-none-eabi-objcopy -O ihex ${FILE}.bin ${FILE}.hex+