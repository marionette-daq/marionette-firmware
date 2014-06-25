
# Directories for MARIONETTE configuration

MARIONETTE_TOP           = ../..
MARIONETTE_OPENOCD       = $(MARIONETTE_TOP)/toolchain/openocd
MARIONETTE_DEVICES       = $(MARIONETTE_TOP)/src/devices
MARIONETTE_UTIL          = $(MARIONETTE_TOP)/src/util
MARIONETTE_BOARDS        = $(MARIONETTE_TOP)/src/boards
MARIONETTE_RULES         = $(MARIONETTE_OPENOCD)/openocd.mk

ifeq ($(shell git diff-index --quiet HEAD $(MARIONETTE_TOP)/src ; echo $$?), 1)
INDEX_DIRTY = M
else
INDEX_DIRTY =
endif

VERSION_PREFIX =

MARIONETTE_VERSION = "\"$(VERSION_PREFIX)`git rev-parse --short HEAD`$(INDEX_DIRTY)\""


