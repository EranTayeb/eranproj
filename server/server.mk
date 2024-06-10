################################################################################
#
# server package
#
################################################################################

SERVER_VERSION = 1.0
SERVER_SITE = package/server/src
SERVER_SITE_METHOD = local# Other methods like git,wget,scp,file etc. are also available.

define SERVER_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define SERVER_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/server  $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
