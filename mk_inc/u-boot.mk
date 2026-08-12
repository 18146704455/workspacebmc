
.PHONY: u-boot-cfg
u-boot-cfg:
	@$(CP) $(BOARD_CFG_DIR)/ast2500_common.h $(UBOOT_DIR)/include/configs/
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(UBOOT_OUTPUT) -C $(UBOOT_DIR) ast2500_defconfig
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(UBOOT_OUTPUT) -C $(UBOOT_DIR) menuconfig

.PHONY:u-boot
u-boot:
	@[ -d $(UBOOT_OUTPUT) ] || mkdir -p $(UBOOT_OUTPUT)
	@$(CP) $(BOARD_CFG_DIR)/ast2500_common.h $(UBOOT_DIR)/include/configs/
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(UBOOT_OUTPUT) -C $(UBOOT_DIR) ast2500_config
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(UBOOT_OUTPUT) EXTRAVERSION=.$(REL_VERSION) -C $(UBOOT_DIR) -j $(CPU_NO)
	#@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(UBOOT_OUTPUT) EXTRAVERSION=.$(REL_VERSION) -C $(UBOOT_DIR) env -j $(CPU_NO)

.PHONY: u-boot-install
u-boot-install: u-boot
	@[ -d $(IMAGE_DIR) ] || mkdir -p $(IMAGE_DIR)
	@$(INSTALL) $(UBOOT_OUTPUT)/u-boot.bin            $(IMAGE_DIR)
	#@$(INSTALL) $(UBOOT_OUTPUT)/tools/env/fw_printenv $(IMAGE_DIR)

.PHONY: u-boot-clean
u-boot-clean:
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) O=$(UBOOT_OUTPUT) -C $(UBOOT_DIR) distclean
	@$(RM) -rf $(UBOOT_OUTPUT)
	@$(RM) -rf $(IMAGE_DIR)/u-boot.bin
	#@$(RM) -rf $(IMAGE_DIR)/fw_printenv
