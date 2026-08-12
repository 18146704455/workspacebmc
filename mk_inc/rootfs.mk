
.PHONY: unpack
unpack:
	@$(RM) -rf $(ROOTFS_DIR) && mkdir -p $(ROOTFS_DIR)
	@echo "unpacking basic rootfs"
	@-tar -zxvf $(BASIC_ROOTFS)/basic_rootfs.tar.gz -C $(ROOTFS_DIR) > /dev/null
	@mv $(ROOTFS_DIR)/basic_rootfs/* $(ROOTFS_DIR)
	@$(RM) -rf $(ROOTFS_DIR)/basic_rootfs

.PHONY: app libs common_app app_%

set_initfile:
	@cp -rf $(ROOTFS_OVERLAY)/modprobe.d $(ROOTFS_DIR)/etc/
	@egrep -v '^$$|^#' $(ROOTFS_OVERLAY)/autostart.rule | while read myline; \
	do	\
		FILE=`echo $$myline | cut -d ' ' -f 1`;\
		LEVEL=`echo $$myline | cut -d ' ' -f 2`	;\
		SEQUENCE=`echo $$myline | cut -d ' ' -f 3`; \
		echo "set $(ROOTFS_OVERLAY)/$$FILE init levle $$LEVEL sequence $$SEQUENCE"; \
		[[ $$LEVEL != 'S' ]] && [[ $$LEVEL -gt 6 || $$LEVEL -lt 1 ]] && echo "LEVEL must in 0~6 or S,this is $$LEVEL!!" && exit; \
		[[ $$SEQUENCE -gt 99 || $$SEQUENCE -lt 1 ]] && echo "SEQUENCE must in 1~99, this is  $$SEQUENCE!!!" && exit;\
		[[ ! -f $(ROOTFS_OVERLAY)/$$FILE ]] && echo "ERROR:no such file $(ROOTFS_OVERLAY)/$$FILE" && exit; \
		chmod +x $(ROOTFS_OVERLAY)/$$FILE; \
		$(INSTALL) $(ROOTFS_OVERLAY)/$$FILE  $(ROOTFS_DIR)/etc/rc.d/init.d;\
		OLDPWD=`pwd`;	\
		cd $(ROOTFS_DIR)/etc/rc.d/rc$$LEVEL.d; \
		ln -s ../init.d/$$FILE S$$SEQUENCE$$FILE;\
		cd $$OLDPWD;	\
	done 


libs: 
	@for d in `ls -l $(TOP_ROOT)/sysroot/libs | awk '/^d/ {print $$NF}'`; 	\
	do 																		\
		$(APP_MAKE) -C $(TOP_ROOT)/sysroot/libs/$$d install	 || echo "$$?"; 	\
	done

common_app: app
	@echo "Compiling local app by COMMON_APPS"
	@[ ! -d $(ROOTFS_DEV_DIR) ] && mkdir $(ROOTFS_DEV_DIR) ;cd $(ROOTFS_DEV_DIR) ; mkdir {include,lib} || echo ""
	@for d in $(COMMON_APPS) ;   \
        do                                                            \
            $(APP_MAKE) -C $(SYS_APP_DIR)/$$d install  || exit "$$?"  ;\
        done

app: 
	@echo "Compiling local app"
	@[ ! -d $(ROOTFS_DEV_DIR) ] && mkdir $(ROOTFS_DEV_DIR) ;cd $(ROOTFS_DEV_DIR) ; mkdir {include,lib} || echo ""
	@for d in `ls -l $(TRD_APP_DIR) | awk '/^d/ {print $$NF}'`;   \
        do                                                            \
            $(APP_MAKE) -C $(TRD_APP_DIR)/$$d install    || exit "$$?" ;          \
        done
app_%:
	echo $@
	$(APP_MAKE) -C $(BOARD_DIR)/app/$* install

.PHONY: rootfs-clean
rootfs-clean:
	@for d in `ls -l $(TRD_APP_DIR) | awk '/^d/ {print $$NF}'`;   \
         do                                                           \
             $(MAKE) -C $(TRD_APP_DIR)/$$d clean  >/dev/null;         \
         done
	@for d in `ls -l $(TOP_ROOT)/sysroot/libs | awk '/^d/ {print $$NF}'`; 	\
	do 																		\
		$(APP_MAKE) -C $(TOP_ROOT)/sysroot/libs/$$d clean  >/dev/null; 	\
	done
	@for d in $(COMMON_APPS) ;   \
		do                                                            \
			$(APP_MAKE) -C $(SYS_APP_DIR)/$$d clean  >/dev/null;  \
		done
	@$(RM) -rf $(ROOTFS_DIR)

install_dev:
	@for f in `find $(ROOTFS_DEV_DIR) -name "*.so*"` ;\
	 do 													\
			cp -d $$f $(ROOTFS_DIR)/lib/ 	;\
	 done
	@for dir in "etc" "bin" "sbin" ;\
	do 								\
		[ -d $(ROOTFS_DEV_DIR)/$$dir ] && cp -a $(ROOTFS_DEV_DIR)/$$dir/* $(ROOTFS_DIR)/$$dir/ || echo "" 	;	\
		[ -d $(ROOTFS_DEV_DIR)/usr/$$dir ] && cp -a $(ROOTFS_DEV_DIR)/usr/$$dir/* $(ROOTFS_DIR)/$$dir/	|| echo ""	;\
		[ -d $(ROOTFS_DEV_DIR)/usr/local/$$dir ] && cp -a $(ROOTFS_DEV_DIR)/usr/local/$$dir/* $(ROOTFS_DIR)/$$dir/	|| echo ""	;\
	done

.PHONY: rootfs instal_dev
rootfs: unpack common_app set_initfile install_dev
	@echo "Installing system modules libs"
	@if [ -d $(IMAGE_DIR)/lib/modules ] ; then       \
            cp -rd $(IMAGE_DIR)/lib/* $(ROOTFS_DIR)/lib; \
         fi
	@echo "Make lzma image"
	@(cd $(ROOTFS_DIR)                                      && \
            find . | cpio -o -H newc > $(IMAGE_DIR)/rootfs.cpio && \
            cd $(IMAGE_DIR) && lzma -k -f -9 rootfs.cpio;          \
         )
