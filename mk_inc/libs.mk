ifneq ($(APP_CFLAGS),)
CFLAGS = $(APP_CFLAGS)
else
CFLAGS = 
endif

ifneq ($(APP_LDFLAGS),)
LDFLAGS = $(APP_LDFLAGS)
else
LDFLAGS=
endif


.PHONY: DEPEND_LIB
DEPEND_LIB:
	@for d in $(DEPENDS) ;do \
		[ -f $(ROOTFS_DEV_DIR)/lib_$${d}_build_time ] && continue ;\
		$(APP_MAKE) -C $(TOP_ROOT)/sysroot/libs/$$d install && echo "$(shell date "+%s" )"> $(ROOTFS_DEV_DIR)/lib_$${d}_build_time || exit "$$?";\
	done 
