
.PHONY: default
default: all

include $(shell pwd)/../../mk_inc/*.mk
include $(shell pwd)/app/common.mk

RECP_BOARD_NAME=$(shell pwd|xargs basename |cut -d '-' -f 2)
IMAGE_BIN = bmc_$(RECP_BOARD_NAME)_$(shell date +'%F').bin
export RECP_BOARD_NAME

.PHONY: all
all: u-boot-install kernel-install rootfs
	@$(CP) -f $(BOARD_CFG_DIR)/fit-$(RECP_BOARD_NAME).its $(IMAGE_DIR)/
	@sed -i 's#REPLACE#'$(IMAGE_DIR)'#g' $(IMAGE_DIR)/fit-$(RECP_BOARD_NAME).its
	@sed -i 's#TMPDTBNAME#'$(RECP_BOARD_NAME).dtb'#g' $(IMAGE_DIR)/fit-$(RECP_BOARD_NAME).its
	@$(UBOOT_OUTPUT)/tools/mkimage -f $(IMAGE_DIR)/fit-$(RECP_BOARD_NAME).its $(IMAGE_DIR)/fit-$(RECP_BOARD_NAME).itb
	@dd if=/dev/zero                of=$(IMAGE_DIR)/../$(IMAGE_BIN) seek=0   bs=1k count=16384 #16MB
	@dd if=$(IMAGE_DIR)/u-boot.bin  of=$(IMAGE_DIR)/../$(IMAGE_BIN) seek=0   bs=1k
	@dd if=$(IMAGE_DIR)/fit-$(RECP_BOARD_NAME).itb of=$(IMAGE_DIR)/../$(IMAGE_BIN) seek=512 bs=1k
	@mv $(IMAGE_DIR)/../$(IMAGE_BIN) $(IMAGE_DIR)

TEST:
	 @echo $(IMAGE_DIR)

.PHONY: clean
clean: rootfs-clean
	@$(RM) -rf $(BOARD_DIR)/build
