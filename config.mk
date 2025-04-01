
# Defaults
TARGET_ARCH			:= $(shell uname -m)
BUILD_TARGET 		:= debug

FILENAME.static := $(PROJECT_NAME).a
FILENAME.shared := lib$(PROJECT_NAME).so
FILENAME.exec := $(PROJECT_NAME)$(FILENAME_EXT.$(BUILD_TARGET)).$(TARGET_ARCH)

FILE_EXT.CC := c
FILE_EXT.CXX := cpp

# Compilers - selected by project
CC := gcc
CXX := g++

# Static programs
LD := g++
AR := gcc-ar

# Common flags
CCFLAGS_COMMON		:= -c -pipe -fPIC
CXXFLAGS_COMMON		:= -c -pipe -fPIC
LDFLAGS_COMMON		:=

# Conditional arch params - x86 only
PLATFORM_FLAGS.x86_64	:= -m64
PLATFORM_FLAGS.i386		:= -m32

# Debug mode 
FILENAME_EXT.debug		:= _dbg
FILENAME_EXT.release	:= # Empty

EXTRA_FLAGS.debug		:= -g -Og -ggdb -Wall
EXTRA_FLAGS.release 	:= -O2 -DNDEBUG

# Flags for use in makefiles
BUILD_FLAGS := $(PLATFORM_FLAGS.$(TARGET_ARCH)) $($(COMPILER)FLAGS_COMMON) $(EXTRA_FLAGS.$(BUILD_TARGET)) $(EXTRA_BUILD_FLAGS)
LINK_FLAGS	:= $(PLATFORM_FLAGS.$(TARGET_ARCH)) $(LDFLAGS_COMMON) $(EXTRA_LINK_FLAGS)
OUTPUT_FILENAME := $(FILENAME.$(PROJECT_TYPE))

# Header print
$(info )
$(info Building project: $(PROJECT_NAME) [$(OUTPUT_FILENAME)])
$(info TARGET_ARCH = $(TARGET_ARCH))
$(info $(COMPILER) = $($(COMPILER)), $(COMPILER)FLAGS = $(BUILD_FLAGS))
$(info LD = $(LD), LDFLAGS = $(LINK_FLAGS))
$(info )
