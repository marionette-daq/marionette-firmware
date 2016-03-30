
# Directories for MARIONETTE configuration

MARIONETTE_TOP           = ../..
MARIONETTE_OPENOCD       = $(MARIONETTE_TOP)/toolchain/openocd
#MARIONETTE_PERIPHERALS   = $(MARIONETTE_TOP)/src/peripherals
MARIONETTE_UTIL          = $(MARIONETTE_TOP)/src/util
MARIONETTE_USB           = $(MARIONETTE_TOP)/src/usb
MARIONETTE_FETCH         = $(MARIONETTE_TOP)/src/fetch
MARIONETTE_MSHELL        = $(MARIONETTE_TOP)/src/mshell
MARIONETTE_BOARDS        = $(MARIONETTE_TOP)/src/boards
MARIONETTE_RULES         = $(MARIONETTE_OPENOCD)/openocd.mk

ifeq ($(shell git diff-index --quiet HEAD $(MARIONETTE_TOP)/src ; echo $$?), 1)
INDEX_DIRTY = M
else
INDEX_DIRTY =
endif

VERSION_PREFIX = git-

MARIONETTE_VERSION = "\"$(VERSION_PREFIX)`git rev-parse --short HEAD`$(INDEX_DIRTY)\""


