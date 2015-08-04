

## Status Meeting Tue 04 August 2015 11:44:17 (PDT)
* (Who: Dave, Gavin, Chris, K, Andrew)

### Next Meeting 18 August 1000 @ Tova

### Next Steps

* Confirm ADC(?) DAC functional before next revision
* USB Descriptor for Composite device
* Current rev 1 boards modded to rev '1.5' for development

### Clocks
* TCXO or Crystal option on STM32
  * NOTE: If TCXO Must set bypass bit on STM32 Clock tree configuration

* ULPI driven from 26Mhz MCOx Output on STM32
  * Webpage for crystal oscillator freq: https://en.wikipedia.org/wiki/Crystal_oscillator_frequencies

### ADC and DAC Requirements notes

#### ADC

##### Sample Modes

1.  Single Sample - right now as fast as possible 
1.  N Samples - as fast as possible
1.  Timestamp/buffered samples
1.  Event triggered
 * Threshold
 * Timestamp
1.  Scheduled Samples

##### Reference Voltage

* External 3.3 V reference by default
* Internal reference optional

##### Software Thoughts

* Considering on thread working constantly over queue of different ADC requests.
* Data is delivered as byte stream on 'data' channel of USB Composite device
 * Maybe get interface descriptor running soon for development.

#### DAC

* Just set a voltage
* Turn on/Turn off 
 * Off - tristate
 * Off - OE (Keeper?)
* Eventually: Function Generator (Sin/Saw/Square)
* Eventually: AWG

### Communications

* Separate Comm and Data Channels on USB
 * Composite device with 2 CDC devices?



