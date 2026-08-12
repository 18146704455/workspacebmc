#!/bin/sh
#
# Copyright 2019-present Scistor. All Rights Reserved.
#
# This program file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program in a file named COPYING; if not, write to the
# Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA

SYSFS_I2C_DEVICES="/sys/bus/i2c/devices"
SYSFS_I2C_DRIVERS="/sys/bus/i2c/drivers"

#
# instantiate an i2c device.
# $1 - parent bus number
# $2 - device address
# $3 - device name/type
#
i2c_device_add() {
    local bus
    local addr
    local device
    bus="$1"
    addr="$2"
    device="$3"
    echo ${device} ${addr} > /sys/class/i2c-dev/i2c-${bus}/device/new_device
}

#
# delete an i2c device.
# $1 - parent bus number
# $2 - device address
#
i2c_device_delete() {
    local bus
    local addr
    bus="$1"
    addr="$2"
    echo ${addr} > /sys/class/i2c-dev/i2c-${bus}/device/delete_device
}

#
# instantiate an i2c-mux device and wait untill all its child buses
# are initialized before the function returns.
# $1 - parent bus number
# $2 - i2c-mux device address
# $3 - i2c-mux device name/type
# $4 - bus number of the last i2c-mux channel
#
i2c_mux_add_sync() {
    retry=0
    max_retry=100
    bus_dir="/sys/class/i2c-dev/i2c-${4}"

    i2c_device_add ${1} ${2} ${3}

    until [ -d ${bus_dir} ]
    do
        usleep 2000 # sleep for 2 milliseconds

        retry=$((retry + 1))
        if [ $retry -ge ${max_retry} ]
        then
            echo "failed to create child buses for i2c-mux ${1}-${2}!"
            return 1
        fi
    done

    return 0
}

