
release_build:
	echo $(RECP_BOARD_NAME) $(COMMIT_VERSION) $(BUILD_TIME) >release


install: release_build
	$(CP) -rf inittab $(ROOTFS_DIR)/etc/
	$(CP) -rf release $(ROOTFS_DIR)/etc/
	#$(CP) -rf ifconfig.eth1 $(ROOTFS_DIR)/etc/sysconfig/
	$(CP) -rf ifconfig.eth0 $(ROOTFS_DIR)/etc/sysconfig/
	$(CP) -rf i2c_device_register.sh  $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rbf nps.sh  $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rbf standby_init.sh  $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rbf copc_power.sh  $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rf ifup 	  $(ROOTFS_DIR)/sbin/
	$(CP) -rf netcfg 	  $(ROOTFS_DIR)/etc/
	$(CP) -rf process.sh $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rf check_come_type.sh $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rf bslot $(ROOTFS_DIR)/usr/sbin/
	$(CP) -rf config 	  $(ROOTFS_DIR)/etc/

clean:
	-rm -rf release
