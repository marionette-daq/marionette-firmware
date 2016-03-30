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

SUPPORTS_RESET=`$DFU_PROG --help 2>&1 | grep reset-stm32 | wc -l`
if [ $SUPPORTS_RESET -ne 1 ]
then
  echo This version of dfu-util does not support reset-stm32, which is a custom modification in dsigmas github fork of dfu-tuil. Clone and compile and install daves version.
  exit 1
fi

# $DFU_PROG --list

$DFU_PROG --device 0483:df11 --cfg 1 --intf 0 --alt 0 --download $FIRMWARE_IMAGE --dfuse-address 0x08000000 

# This will generate a reset on the STM32
$DFU_PROG --device 0483:df11 --cfg 1 --intf 0 --alt 0 --reset-stm32
