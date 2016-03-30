##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Compiler options here.
ifeq ($(USE_OPT),)
  #USE_OPT = -Og -ggdb -fomit-frame-pointer -falign-functions=16 -Wno-main -std=gnu99
  #USE_OPT = -O2 -ggdb -fomit-frame-pointer -falign-functions=16 -Wno-main -Wno-unused  -std=gnu99
  USE_OPT = -Og -ggdb -fomit-frame-pointer -falign-functions=16 -Wno-main -Wno-unused  -std=gnu99
  USE_OPT += -DARM_MATH_CM4 -D__FPU_PRESENT
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT = 
endif

# Enable this if you want link time optimizations (LTO)
ifeq ($(USE_LTO),)
  USE_LTO = no
endif

# If enabled, this option allows to compile the application in THUMB mode.
ifeq ($(USE_THUMB),)
  USE_THUMB = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = no
	# disabled because this throws errors since it can't find the conf files
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU on Cortex-M4 (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = softfp
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, sources and paths
#

# Define project name here
PROJECT = ch

# Imported source files and paths
CONF_DIR  = conf
TOOLCHAIN = ../../toolchain
CHIBIOS   = ../../ChibiOS-RT
BOARD     = Marionette_PCB_rev2
BOARDDIR  = ../boards/$(BOARD)

# Marionette specific files
include $(TOOLCHAIN)/marionette.mk

# Startup files
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk

# HAL-OSAL files
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(BOARDDIR)/board.mk
include $(CHIBIOS)/os/hal/osal/rt/osal.mk

# RTOS files
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/rt/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F429xI.ld

FATFS_DIR=../ext/fatfs
FATFSSRC = ${CHIBIOS}/os/various/fatfs_bindings/fatfs_diskio.c \
           ${CHIBIOS}/os/various/fatfs_bindings/fatfs_syscall.c \
           $(FATFS_DIR)/src/ff.c \
           $(FATFS_DIR)/src/option/ccsbcs.c 

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
MSRC = $(wildcard $(MARIONETTE_UTIL)/*.c)
MSRC += $(wildcard $(MARIONETTE_USB)/*.c)
#MSRC += $(wildcard $(MARIONETTE_PERIPHERALS)/*.c)
MSRC += $(wildcard $(MARIONETTE_FETCH)/*.c)
MSRC += $(wildcard $(MARIONETTE_MSHELL)/*.c)

CSRC = $(STARTUPSRC) \
			 $(KERNSRC) \
			 $(PORTSRC) \
       $(OSALSRC) \
       $(HALSRC) \
       $(PLATFORMSRC) \
       $(BOARDSRC) \
       $(CHIBIOS)/os/hal/lib/streams/memstreams.c \
       $(CHIBIOS)/os/hal/lib/streams/chprintf.c \
       $(MSRC) \
			 $(FATFSSRC) \
       main.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC =

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACSRC =

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACPPSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCPPSRC =

# List ASM source files here
ASMSRC = $(STARTUPASM) $(PORTASM) $(OSALASM)

INCDIR = $(STARTUPINC) $(KERNINC) $(PORTINC) $(OSALINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) \
         $(CHIBIOS)/os/hal/lib/streams $(CHIBIOS)/os/various \
         $(MARIONETTE_UTIL)/include \
	       $(MARIONETTE_USB)/include \
         $(MARIONETTE_FETCH)/include \
	       $(MARIONETTE_MSHELL)/include \
	       include \
	       $(FATFS_DIR)/src \
	       $(CONF_DIR)

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

MCU  = cortex-m4

#TRGT = arm-elf-
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc 
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# ARM-specific options here
AOPT =

# THUMB-specific options here
TOPT = -mthumb -DTHUMB 

# Define C warning options here
#CWARN = -Wall -Wextra -Wstrict-prototypes  -Wdisabled-optimization \
	#-Wdouble-promotion -Wformat=2 -Wfloat-equal \
	#-Waggressive-loop-optimizations -Wunsafe-loop-optimizations \
	#-Waggregate-return -Wlogical-op -Wmissing-include-dirs \
	#-Wpointer-arith -Wredundant-decls 

CWARN = -Wall -Wextra -Wstrict-prototypes  -Wdisabled-optimization \
	-Wdouble-promotion -Wformat=2 -Wfloat-equal \
	-Waggressive-loop-optimizations \
	-Waggregate-return -Wlogical-op

#-Wmissing-include-dirs
	
# Define C++ warning options here
CPPWARN = -Wall -Wextra -Wundef


##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = -DGIT_COMMIT_VERSION=$(MARIONETTE_VERSION)
UDEFS += -DDBG_MSG_ENABLE=1

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS = -lm

#
# End of user defines
##############################################################################

RULESPATH = $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC
include $(RULESPATH)/rules.mk
include $(MARIONETTE_RULES)


