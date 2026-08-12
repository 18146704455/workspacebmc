#单板的网络交换芯片位于bmc的总线1上，和其他单板不同(总线0),
#所以需要修改通用工具目录下的相关宏配置(通用工具未提交)
#目录：

xarhbmc/sysroot/app/DSDT_3.6.3/usrapp/vlan16u/bmcmii.c
--->#define SWITCH_MII_BUS 0

xarhbmc/sysroot/app/net_switch/netswitch.c
--->#define SWICH88E6155_MDIO_BUS    0

#

