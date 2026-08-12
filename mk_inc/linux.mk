
.PHONY: kernel-cfg
kernel-cfg:
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) -C $(KERNEL_DIR) ast2500_defconfig
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) -C $(KERNEL_DIR) menuconfig
	@cp -f $(KERNEL_OUTPUT)/.config $(KERNEL_DIR)/arch/arm/configs/ast2500_defconfig

.PHONY: kernel
kernel:
	@[ -d $(KERNEL_OUTPUT) ] || mkdir -p $(KERNEL_OUTPUT)
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) -C $(KERNEL_DIR) ast2500_defconfig
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) EXTRAVERSION=.$(REL_VERSION) -C $(KERNEL_DIR) -j $(CPU_NO)
	@cp $(BOARD_CFG_DIR)/$(RECP_BOARD_NAME).dts $(KERNEL_DIR)/arch/arm/boot/dts
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) EXTRAVERSION=.$(REL_VERSION) -C $(KERNEL_DIR) $(RECP_BOARD_NAME).dtb

.PHONY: kernel-install
kernel-install: kernel
	@[ -d $(IMAGE_DIR) ] || mkdir -p $(IMAGE_DIR)
	@$(INSTALL) $(KERNEL_OUTPUT)/arch/arm/boot/zImage $(IMAGE_DIR)
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) INSTALL_MOD_PATH=$(IMAGE_DIR) -C $(KERNEL_DIR) modules_install
	@$(INSTALL) $(KERNEL_OUTPUT)/arch/arm/boot/dts/$(RECP_BOARD_NAME).dtb $(IMAGE_DIR)

.PHONY: kernel-clean
kernel-clean:
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(KERNEL_OUTPUT) -C $(KERNEL_DIR) distclean
	@$(RM) -rf $(KERNEL_OUTPUT)
	@$(RM) -rf $(IMAGE_DIR)/zImage
	@$(RM) -rf $(IMAGE_DIR)/lib
	@$(RM) -rf $(IMAGE_DIR)/aspeed-bmc-scm.dtb
