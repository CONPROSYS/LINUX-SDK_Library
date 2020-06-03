# Library Modules for CONPROSYS [![Build Status](https://travis-ci.org/CONPROSYS/LINUX-SDK_library.svg?branch=master)](https://travis-ci.org/CONPROSYS/LINUX-SDK_library) [![Coverity Scan Build Status](https://img.shields.io/coverity/scan/17302.svg)](https://scan.coverity.com/projects/conprosys-linux-sdk_library)

## Features
* Version : 1.5.1.0 (2020/06/02)

## Licenses

|Sub Folder| License |
|:---|:---|
|libSerialFunc (Serial Communication Functions Library Module) | [![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) |
|cps-driver (CPS-MCS341 Library Modules)| [![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) |
|SubGiga Communication Module | [![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) |
|EEPROM Library||


## Support CONTEC Devices
### Controllers

* CPS-MC341-ADSC1
* CPS-MC341-ADSC2
* CPS-MC341G-ADSC1
* CPS-MC341Q-ADSC1
* CPS-MC341-DS1
* CPS-MC341-DS2
* CPS-MC341-DS11
* CPS-MC341-A1

* [CPS-MCS341-DS1-111](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/m2m-controller/cps-mcs341-ds1-111/price/)
* [CPS-MCS341-DS1-131](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/m2m-controller/cps-mcs341-ds1-131/price/)
* [CPS-MCS341G-DS1-130](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/m2m-controller/cps-mcs341g-ds1-130/price/)
* [CPS-MCS341Q-DS1-131](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/m2m-controller/cps-mcs341q-ds1-131/price/)

### Modules (CPS-MCS341 Series only)
* Analog Input/Output
    * [CPS-AI-1608LI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ai-1608li/price/)
    * [CPS-AO-1604LI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ao-1604li/price/)
    * [CPS-AI-1608ALI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ai-1608ali/price/)
    * [CPS-AO-1604VLI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ao-1604vli/price/)
* Sensor Input
    * [CPS-SSI-4P](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ssi-4p/price/)
* Digital Input/Output
    * [CPS-DIO-0808L](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-dio-0808l/price/)
    * [CPS-DIO-0808BL](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-dio-0808bl/price/)
    * [CPS-DIO-0808RL](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-dio-0808rl/price/)
    * CPS-DIO-0808RBL
    * [CPS-RRY-4PCC](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-rry-4pcc/price/)
    * [CPS-DI-16L](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-di-16l/price/)
    * CPS-DI-16BL
    * [CPS-DI-16RL](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-di-16rl/price/)
    * CPS-DI-16RBL
    * [CPS-DO-16L](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-do-16l/price/)
    * CPS-DO-16BL
    * [CPS-DO-16RL](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-do-16rl/price/)
    * CPS-DO-16RBL
* Serial Communication
    * [CPS-COM-1PC](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-com-1pc/price/)
    * [CPS-COM-2PC](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-com-2pc/price/)
    * [CPS-COM-1PD](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-com-1pd/price/)
    * [CPS-COM-2PD](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-com-2pd/price/)
* Counter
    * [CPS-CNT-3202I](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-cnt-3202i/price/)

## Folder / Sources
* Serial Communication Functions Library Module
    * libSerialFunc
        * Version.1.0.7
* cps-driver
    CPS-MCS341 Library Modules
    * libCpsAio (Analog Input/Output)
        * Version 1.2.3
    * libCpsDio (Digital Input/Output)
        * Version 1.0.8
    * libCpsCnt (Counter)
        * Version 1.0.4
    * libCpsSsi (Sensor Input)
        * Version 1.0.7


    > Please build with the [driver](https://github.com/CONPROSYS/LINUX-SDK_driver) (Driver Version 1.5.0.0 or later).

* SubGiga Communication Module ( 920MHz )
    CMM-920 Module Library
    * libConexio
        * Version 1.1.5

* EEPROM Library 
    * libCpsEeprom 
        * Version 0.1.2

## Change log
* Ver 1.5.1.0
    * libCpsDio 1.0.7 -> 1.0.8
        Fixed, the kernel panic is occured to use the ContecCpsDioOutBit function. 
* Ver 1.5.0.0
    * libCpsAio 1.2.2 -> 1.2.3
        * Fixed, after the application is setting the sampling clock 10 micro second, the ContecCpsAioMultiAi function is occured the samplingclock Error flag.
* Ver 1.4.2.0
    * libCpsAio 1.2.1 -> 1.2.2
        * Fixed Channel's Value is not right channel data.
        * Change Makefile version.
* Ver 1.4.1.0
    * libCpsAio 1.2.0 -> 1.2.1
        * Fixed ContecCpsAioSetAiStopTrigger function. Change AIOECU_SRC_START to CPS_AIO_ECU_SRC_NON_CONNECT.
        * Fixed many functions.(CoverityScan's Many Defacts.)
    * libCpsDio 1.0.6 -> 1.0.7
        * Fixed many functions.(CoverityScan's Many Defacts.)
    * libCpsCnt 1.0.3 -> 1.0.4
        Fixed many functions.(CoverityScan's Many Defacts.)
    * libCpsSsi 1.0.6 -> 1.0.7
        * Fixed many functions.(CoverityScan's Many Defacts.)
    * libCpsEeprom 0.1.1 -> 0.1.2
        * Fixed SetEepromFilePath function.(CoverityScan's Many Defacts.)
    * libConexio 1.1.4 -> 1.1.5
        * Fixed freeConexioCMM920_packet function.(CoverityScan's Many Defacts.)

* Ver 1.4.0.0
    * libCpsAio 1.0.11 -> 1.2.0
        * Add AiStopTrigger, AiResetDevice , AiResetMemory Functions and more.

* Ver 1.3.3.2
    * libCpsAio 1.0.10 -> 1.0.11
        * Fixed MultiAo Function. This device do not update value in Channel 1 or later.


