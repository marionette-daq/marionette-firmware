#!/bin/bash -x
export FIRMWARE_IMAGE=$1

if [ ! -e $FIRMWARE_IMAGE ]
then
  echo Firmware image $FIRMWARE_IMAGE does not exist
  exit 1
fi

export DFU_PROG=dfu-util
if [ ! -e $DFU_PROG ] && [ -e /opt/dfu-util-dsigma/bin/dfu-util ]
then
  export DFU_PROG=/opt/dfu-util-dsigma/bin/dfu-util
fi

SUPPORTS_RESET_NEW=`$DFU_PROG -V | grep "dfu-util 0.9" | wc -l`
SUPPORTS_RESET_STM32=`$DFU_PROG --help 2>&1 | grep reset-stm32 | wc -l`

if [ "$SUPPORTS_RESET_NEW" -eq "1" ]
then
  LEAVE_APPEND_VALUE=":leave"
fi

if [ $SUPPORTS_RESET_STM32 -ne 1 ] && [ "$SUPPORTS_RESET_NEW" -ne "1" ]
then
  echo This version of dfu-util does not support reset-stm32 or :leave option. Please install the latest dfu-util from the https://sourceforge.net/projects/dfu-util/?source=directory repository
  exit 1
fi

# $DFU_PROG --list

$DFU_PROG --device 0483:df11 --cfg 1 --intf 0 --alt 0 --download $FIRMWARE_IMAGE --dfuse-address 0x08000000$LEAVE_APPEND_VALUE

# This will generate a reset on the STM32
if [ "$SUPPORTS_RESET_STM32" -eq "1" ] && [ "$SUPPORTS_RESET_NEW" -ne "1" ]
then
  $DFU_PROG --device 0483:df11 --cfg 1 --intf 0 --alt 0 --reset-stm32
fi

