#
# embedXcode
# ----------------------------------
# Embedded Computing on Xcode
#
# Copyright © Rei VILO, 2010-2017
# http://embedxcode.weebly.com
# All rights reserved
#
#
# Last update: Feb 10, 2016 release 4.2.5


include $(MAKEFILE_PATH)/About.mk

# panStamp AVR specifics
# ----------------------------------
#
PLATFORM         := LittleRobotFriends
PLATFORM_TAG      = ARDUINO=10801 LITTLEROBOTFRIENDS EMBEDXCODE=$(RELEASE_NOW) ARDUINO_ARCH_AVR
PLATFORM_VERSION := $(LITTLEROBOTFRIENDS_AVR_RELEASE) for Arduino 1.6.5

ifeq ($(BOARD_TAG),lrfdock)
    APPLICATION_PATH := $(LITTLEROBOTFRIENDS_1)
    HARDWARE_PATH     = $(APPLICATION_PATH)/hardware/avr/$(LITTLEROBOTFRIENDS_AVR_RELEASE)
    PLATFORM_TAG     += ARDUINO_AVR_LRF_DOCK
else
    APPLICATION_PATH := $(ARDUINO_PATH)
    HARDWARE_PATH     = $(APPLICATION_PATH)/hardware/arduino/avr
    USER_LIBS_LIST   := $(filter-out 0,$(USER_LIBS_LIST)) LittleRobotFriends
    PLATFORM_TAG     += ARDUINO_AVR_LRF
endif


#HARDWARE_PATH     = $(APPLICATION_PATH)/hardware/avr/1.6.0

# With ArduinoCC 1.6.6, AVR 1.6.9 used to be under ~/Library
TOOL_CHAIN_PATH   = $(ARDUINO_AVR_PATH)/tools/avr-gcc/$(ARDUINO_GCC_AVR_RELEASE)
OTHER_TOOLS_PATH  = $(ARDUINO_AVR_PATH)/tools/avrdude/$(ARDUINO_AVRDUDE_RELEASE)

# With ArduinoCC 1.6.7, AVR 1.6.9 is back under Arduino.app
ifeq ($(wildcard $(TOOL_CHAIN_PATH)),)
    TOOL_CHAIN_PATH   = $(ARDUINO_PATH)/hardware/tools/avr
endif
ifeq ($(wildcard $(OTHER_TOOLS_PATH)),)
    OTHER_TOOLS_PATH  = $(ARDUINO_PATH)/hardware/tools/avr
endif

BUILD_CORE       = avr
BOARDS_TXT      := $(LITTLEROBOTFRIENDS_1)/hardware/avr/$(LITTLEROBOTFRIENDS_AVR_RELEASE)/boards.txt
lrf1200          = $(call PARSE_BOARD,$(BOARD_TAG),build.core)
BUILD_CORE       = $(patsubst arduino:%,%,$(lrf1200))
#BUILD_CORE       = $(call PARSE_BOARD,$(BOARD_TAG),build.core)

AVRDUDE_PATH         = $(ARDUINO_PATH)/hardware/tools/avr

APP_TOOLS_PATH      := $(TOOL_CHAIN_PATH)/bin
CORE_LIB_PATH       := $(HARDWARE_PATH)/cores/$(BUILD_CORE)

BUILD_CORE_LIB_PATH  = $(HARDWARE_PATH)/cores/$(BUILD_CORE)
BUILD_CORE_LIBS_LIST = $(subst .h,,$(subst $(BUILD_CORE_LIB_PATH)/,,$(wildcard $(BUILD_CORE_LIB_PATH)/*.h))) # */
BUILD_CORE_C_SRCS    = $(wildcard $(BUILD_CORE_LIB_PATH)/*.c) # */

BUILD_CORE_CPP_SRCS  = $(filter-out %program.cpp %main.cpp,$(wildcard $(BUILD_CORE_LIB_PATH)/*.cpp)) # */

BUILD_CORE_OBJ_FILES = $(BUILD_CORE_C_SRCS:.c=.c.o) $(BUILD_CORE_CPP_SRCS:.cpp=.cpp.o)
BUILD_CORE_OBJS      = $(patsubst $(APPLICATION_PATH)/%,$(OBJDIR)/%,$(BUILD_CORE_OBJ_FILES))


# Two locations for panStamp libraries
#
APP_LIB_PATH     = $(APPLICATION_PATH)/libraries
APP_LIB_PATH    += $(HARDWARE_PATH)/libraries

p1000    = $(foreach dir,$(APP_LIB_PATH),$(patsubst %,$(dir)/%,$(APP_LIBS_LIST)))
p1000   += $(foreach dir,$(APP_LIB_PATH),$(patsubst %,$(dir)/%/utility,$(APP_LIBS_LIST)))
p1000   += $(foreach dir,$(APP_LIB_PATH),$(patsubst %,$(dir)/%/src,$(APP_LIBS_LIST)))
p1000   += $(foreach dir,$(APP_LIB_PATH),$(patsubst %,$(dir)/%/src/utility,$(APP_LIBS_LIST)))
p1000   += $(foreach dir,$(APP_LIB_PATH),$(patsubst %,$(dir)/%/src/arch/$(BUILD_CORE),$(APP_LIBS_LIST)))
p1000   += $(foreach dir,$(APP_LIB_PATH),$(patsubst %,$(dir)/%/src/$(BUILD_CORE),$(APP_LIBS_LIST)))

APP_LIB_CPP_SRC = $(foreach dir,$(p1000),$(wildcard $(dir)/*.cpp)) # */
APP_LIB_C_SRC   = $(foreach dir,$(p1000),$(wildcard $(dir)/*.c)) # */
APP_LIB_S_SRC   = $(foreach dir,$(p1000),$(wildcard $(dir)/*.S)) # */
APP_LIB_H_SRC   = $(foreach dir,$(p1000),$(wildcard $(dir)/*.h)) # */

APP_LIB_OBJS     = $(patsubst $(APPLICATION_PATH)/%.cpp,$(OBJDIR)/%.cpp.o,$(APP_LIB_CPP_SRC))
APP_LIB_OBJS    += $(patsubst $(APPLICATION_PATH)/%.c,$(OBJDIR)/%.c.o,$(APP_LIB_C_SRC))

BUILD_APP_LIBS_LIST = $(subst $(BUILD_APP_LIB_PATH)/, ,$(APP_LIB_CPP_SRC))

APP_LIBS_LOCK = 1


# Sketchbook/Libraries path
# wildcard required for ~ management
# ?ibraries required for libraries and Libraries
#
ifeq ($(USER_LIBRARY_DIR)/Arduino15/preferences.txt,)
    $(error Error: run Arduino once and define the sketchbook path)
endif

ifeq ($(wildcard $(SKETCHBOOK_DIR)),)
    SKETCHBOOK_DIR = $(shell grep sketchbook.path $(wildcard ~/Library/Arduino15/preferences.txt) | cut -d = -f 2)
endif

ifeq ($(wildcard $(SKETCHBOOK_DIR)),)
    $(error Error: sketchbook path not found)
endif

USER_LIB_PATH  = $(wildcard $(SKETCHBOOK_DIR)/?ibraries)

ifeq ($(BOARD_TAG),lrfdock)
    VARIANT      = LRFDock
else
    lrf1300      = $(call PARSE_BOARD,$(BOARD_TAG),build.variant)
    VARIANT      = $(patsubst arduino:%,%,$(lrf1300))
endif
VARIANT_PATH = $(HARDWARE_PATH)/variants/$(VARIANT)


# Rules for making a c++ file from the main sketch (.pde)
#
PDEHEADER      = \\\#include \"WProgram.h\"  


# Tool-chain names
#
CC      = $(APP_TOOLS_PATH)/avr-gcc
CXX     = $(APP_TOOLS_PATH)/avr-g++
AR      = $(APP_TOOLS_PATH)/avr-ar
OBJDUMP = $(APP_TOOLS_PATH)/avr-objdump
OBJCOPY = $(APP_TOOLS_PATH)/avr-objcopy
SIZE    = $(APP_TOOLS_PATH)/avr-size
NM      = $(APP_TOOLS_PATH)/avr-nm


MCU_FLAG_NAME   = mmcu
MCU             = $(call PARSE_BOARD,$(BOARD_TAG),build.mcu)
F_CPU           = $(call PARSE_BOARD,$(BOARD_TAG),build.f_cpu)
OPTIMISATION   ?= -Os


# LRF Dock USB PID VID
#
USB_VID     := $(call PARSE_BOARD,$(BOARD_TAG),build.vid)
USB_PID     := $(call PARSE_BOARD,$(BOARD_TAG),build.pid)
USB_PRODUCT := $(call PARSE_BOARD,$(BOARD_TAG),build.usb_product)

ifneq ($(USB_VID),)
    USB_FLAGS    = -DUSB_VID=$(USB_VID)
    USB_FLAGS   += -DUSB_PID=$(USB_PID)
    USB_FLAGS   += -DUSBCON
    USB_FLAGS   += -DUSB_MANUFACTURER=''
    USB_FLAGS   += -DUSB_PRODUCT='$(USB_PRODUCT)'
endif

# Arduino Leonardo serial 1200 reset
#
USB_TOUCH := $(call PARSE_BOARD,$(BOARD_TAG),upload.protocol)

ifeq ($(USB_TOUCH),avr109)
    USB_RESET  = python $(UTILITIES_PATH)/reset_1200.py
endif


INCLUDE_PATH    = $(CORE_LIB_PATH) $(APP_LIB_PATH) $(VARIANT_PATH) $(HARDWARE_PATH)
INCLUDE_PATH   += $(sort $(dir $(APP_LIB_CPP_SRC) $(APP_LIB_C_SRC) $(APP_LIB_H_SRC)))
INCLUDE_PATH   += $(sort $(dir $(BUILD_APP_LIB_CPP_SRC) $(BUILD_APP_LIB_C_SRC)))
INCLUDE_PATH   += $(OBJDIR)


# Flags for gcc, g++ and linker
# ----------------------------------
#
# Common CPPFLAGS for gcc, g++, assembler and linker
#
CPPFLAGS     = -g $(OPTIMISATION) $(WARNING_FLAGS) # -w
CPPFLAGS    += -ffunction-sections -fdata-sections -MMD
CPPFLAGS    += -$(MCU_FLAG_NAME)=$(MCU) -DF_CPU=$(F_CPU) $(USB_FLAGS)
CPPFLAGS    += $(addprefix -D, $(PLATFORM_TAG))
CPPFLAGS    += $(addprefix -I, $(INCLUDE_PATH))

# Specific CFLAGS for gcc only
# gcc uses CPPFLAGS and CFLAGS
#
CFLAGS       =

# Specific CXXFLAGS for g++ only
# g++ uses CPPFLAGS and CXXFLAGS
#
CXXFLAGS     = -fno-exceptions -fno-threadsafe-statics

# Specific ASFLAGS for gcc assembler only
# gcc assembler uses CPPFLAGS and ASFLAGS
#
ASFLAGS      = -x assembler-with-cpp

# Specific LDFLAGS for linker only
# linker uses CPPFLAGS and LDFLAGS
#
LDFLAGS      = -w $(OPTIMISATION) -Wl,--gc-sections
LDFLAGS     += -$(MCU_FLAG_NAME)=$(MCU) -DF_CPU=$(F_CPU) -lm
#LDFLAGS     += $(call PARSE_BOARD,$(BOARD_TAG),build.flags.cpu)
#LDFLAGS     += $(OPTIMISATION) $(call PARSE_BOARD,$(BOARD_TAG),build.flags.ldspecs)
#LDFLAGS     += $(call PARSE_BOARD,$(BOARD_TAG),build.flags.libs) --verbose

# Specific OBJCOPYFLAGS for objcopy only
# objcopy uses OBJCOPYFLAGS only
#
OBJCOPYFLAGS  = -O ihex -R .eeprom

# Target
#
TARGET_HEXBIN = $(TARGET_HEX)
#-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0
TARGET_EEP    = $(OBJDIR)/$(TARGET).eep

# Commands
# ----------------------------------
# Link command
#
COMMAND_LINK = $(CC) $(OUT_PREPOSITION)$@ $(LOCAL_OBJS) $(LOCAL_ARCHIVES) $(TARGET_A) -LBuilds $(LDFLAGS)

COMMAND_UPLOAD  = $(AVRDUDE_EXEC) -p$(AVRDUDE_MCU) -D -c$(AVRDUDE_PROGRAMMER) -C$(AVRDUDE_CONF) -P$(USED_SERIAL_PORT) -b$(AVRDUDE_BAUDRATE) -Uflash:w:$(TARGET_HEX):i
