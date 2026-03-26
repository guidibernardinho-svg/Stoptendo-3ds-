#---------------------------------------------------------------------------------
# 3DS Homebrew Makefile - devkitPro / libctru / citro2d
#---------------------------------------------------------------------------------
APP_TITLE   := 3DS CHIP-8 Player
APP_AUTHOR  := SeuNome
APP_DESC    := Emulador CHIP-8 para Nintendo 3DS
APP_VERSION := 1.0.0

TARGET      := 3ds-player
BUILD       := build
SOURCES     := source
INCLUDES    := include
DATA        := data
ROMFS       := romfs

#---------------------------------------------------------------------------------
include $(DEVKITPRO)/libctru/3ds_rules

ARCH        := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS      := -g -Wall -O2 -mword-relocations \
               -fomit-frame-pointer -ffunction-sections \
               $(ARCH) $(INCLUDE) -D__3DS__
CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++14
ASFLAGS     := -g $(ARCH)
LDFLAGS     := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS        := -lcitro2d -lcitro3d -lctru -lm
LIBDIRS     := $(CTRULIB) $(PORTLIBS)

#---------------------------------------------------------------------------------
include $(DEVKITPRO)/rules/3ds.mk
