
# make rule for using dfutil to program the marionette board via USB
# Note: the dsigma version of dfutil is needed to allow for resetting the stm32

write_dfu: $(BUILDDIR)/$(PROJECT).bin
	$(MARIONETTE_TOP)/toolchain/dfu_program.sh $(BUILDDIR)/$(PROJECT).bin

