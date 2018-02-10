# include libs/ngl/Makefile

######################################
# System
######################################
OS = Linux

######################################
# target
######################################
TARGET = ns3

######################################
# building variables
######################################
DEBUG = 0
OPT = -O1

#######################################
# paths
#######################################
# source path
SOURCES_DIR = \
syscalls \
cmsis \
cmsis_boot \
cmsis_boot/startup \
processing_output \
host \
settings \
drivers/epm570 \
drivers/analog \
drivers/esp_07 \
drivers/ad9288 \
drivers/eeprom \
drivers/systick \
drivers/i2c \
drivers/i2c/i2c_gpio \
drivers/lcd \
drivers/lcd/fonts \
drivers/rtc \
fft \
stm_lib/src \
interface \
support \

# firmware library path
PERIFLIB_PATH =
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES = \
init.c \
main.c \
stm32f10x_it.c \
syscalls/syscalls.c \
cmsis_boot/system_stm32f10x.c \
cmsis_boot/startup/startup_stm32f10x_md.c \
processing_output/Processing_and_output.c \
host/IQueue.c \
host/HostCommands.c \
host/Host.c \
host/ReceiveStateMachine.c \
settings/Settings.c \
drivers/epm570/EPM570.c \
drivers/epm570/EPM570_GPIO.c \
drivers/epm570/EPM570_Registers.c \
drivers/analog/AutoCorrectCH.c \
drivers/analog/Analog.c \
drivers/esp_07/ns_esp_07.c \
drivers/ad9288/AD9288.c \
drivers/eeprom/eeprom_WR_RD.c \
drivers/systick/stm32f10x_systick.c \
drivers/systick/systick.c \
drivers/i2c/i2c.c \
drivers/i2c/i2c_gpio/max7320.c \
drivers/i2c/i2c_gpio/i2c_gpio.c \
drivers/i2c/i2c_gpio/pca9675.c \
drivers/lcd/colors-5-6-5.c \
drivers/lcd/fonts/Arial16.c \
drivers/lcd/fonts/SansSerif_9Bold.c \
drivers/lcd/fonts/lucidaConsole10.c \
drivers/lcd/fonts/TimesNewRoman12.c \
drivers/rtc/RTC.c \
fft/fft.c \
stm_lib/src/stm32f10x_tim.c \
stm_lib/src/stm32f10x_pwr.c \
stm_lib/src/stm32f10x_flash.c \
stm_lib/src/stm32f10x_bkp.c \
stm_lib/src/misc.c \
stm_lib/src/stm32f10x_dbgmcu.c \
stm_lib/src/stm32f10x_i2c.c \
stm_lib/src/stm32f10x_gpio.c \
stm_lib/src/stm32f10x_rcc.c \
stm_lib/src/stm32f10x_adc.c \
stm_lib/src/stm32f10x_usart.c \
stm_lib/src/stm32f10x_dma.c \
stm_lib/src/stm32f10x_exti.c \
stm_lib/src/stm32f10x_rtc.c \
interface/gInterface_MENU.c \
interface/ChannelsMenu.c \
interface/Trig_Menu.c \
interface/Quick_Menu.c \
interface/Digit_Trig_Menu.c \
interface/TimeScale_Menu.c \
interface/Advanced_Menu.c \
interface/User_Interface.c \
interface/Meas_Menu.c \
support/Synchronization.c \
support/Measurments.c \
support/Sweep.c \

# ASM sources
ASM_SOURCES =

######################################
# firmware library
######################################
PERIFLIB_SOURCES =

#######################################
# binaries
#######################################
BINPATH =
PREFIX = arm-none-eabi-
CC = $(BINPATH)$(PREFIX)gcc
AS = $(BINPATH)$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)$(PREFIX)objcopy
AR = $(BINPATH)$(PREFIX)ar
SZ = $(BINPATH)$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
CPU = -mcpu=cortex-m3
FPU =
FLOAT-ABI =
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

AS_DEFS =
AS_INCLUDES =

C_DEFS = \
-DSTM32F103RB \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER \
-D__LCD_18_BIT__ \
-D__LCD_DIRECT__
ifeq ($(DEBUG), 1)
C_DEFS += -D__SWD_DEBUG__
endif

C_INCLUDES =  \
-I. \
-Icmsis \
-Icmsis_boot \
-Iprocessing_output \
-Ihost \
-Isettings \
-Idrivers/epm570 \
-Idrivers/analog \
-Idrivers/esp_07 \
-Idrivers/ad9288 \
-Idrivers/eeprom \
-Idrivers/systick \
-Idrivers/i2c \
-Idrivers/i2c/i2c_gpio \
-Idrivers/lcd \
-Idrivers/rtc \
-Ifft \
-Istm_lib/inc \
-Iinterface \
-Iinterface/buttons \
-Isupport \

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -g -Wall -fdata-sections -ffunction-sections -fno-strict-aliasing
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

#######################################
# LDFLAGS
#######################################
LDSCRIPT = arm-gcc-link.ld
ifeq ($(DEBUG), 1)
LDSCRIPT = arm-gcc-link-debug.ld
endif

LIBS = -lGL_HX8352_flash -lc -lm -lnosys -lngl
LIBDIRS_F = -Ldrivers/lcd -Llibs/ngl/build

LDFLAGS = $(MCU) -Wall -g -nostartfiles "-Wl,-Map=$(BUILD_DIR)/$(TARGET).map" -T$(LDSCRIPT) $(LIBDIRS_F) $(LIBS) -Wl,--gc-sections

#######################################
# build the application
#######################################
all:
	@echo building submodule ngl...
	$(MAKE) -C libs/ngl

	@echo building $(TARGET)...
	$(MAKE) $(BUILD_DIR)/$(TARGET).elf
	$(MAKE) $(BUILD_DIR)/$(TARGET).hex
	$(MAKE) $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(notdir $(<:.c)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $(OBJECTS)
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@


.PHONY: clean flash flash_s debug

#######################################
# clean up
#######################################
clean:
	@echo - cleaning $(BUILD_DIR)...
ifeq ($(OS),Windows)
	($(BUILD_DIR):&(rd /s /q "$(BUILD_DIR)" 2> NUL))&
endif
ifeq ($(OS),Linux)
	-@rm -rf $(BUILD_DIR)
	-@rm -rf $(LSTDIR)
	-@rm -rf $(BINDIR)
	-@rm -rf .dep
endif

#######################################
# flash
#######################################
OPENOCD_PARAM = -c "source [find interface/stlink-v2.cfg]"
OPENOCD_PARAM += -c "transport select hla_swd"
OPENOCD_PARAM += -c "source [find target/stm32f4x_stlink.cfg]"

OPENOCD_PARAM_DEBUG = $(OPENOCD_PARAM)
OPENOCD_PARAM_DEBUG += -c "gdb_port 3333"
OPENOCD_PARAM_DEBUG += -c "debug_level 2"
OPENOCD_PARAM_DEBUG += -c "set WORKAREASIZE 0x2000"
OPENOCD_PARAM_DEBUG += -c "reset_config srst_only"

FLASHER = openocd

flash:all
	@echo - programming with $(FLASHER)...
	$(FLASHER) $(OPENOCD_PARAM) -c "program ./$(BUILD_DIR)/$(TARGET).elf verify reset exit"

erase:
	@echo - erasing memory with $(FLASHER)...

debug:flash
	@echo - $(FLASHER) server is running...
	@echo - OPENOCD_PARAM_DEBUG
	$(FLASHER) $(OPENOCD_PARAM_DEBUG)

#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***