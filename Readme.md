# Library Modules for CONPROSYS [![Build Status](https://travis-ci.org/CONPROSYS/LINUX-SDK_library.svg?branch=master)](https://travis-ci.org/CONPROSYS/LINUX-SDK_library) 

## Features
* Version : 1.3.3.2 (2018/11/22)

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
    * [CPS-AO-1608LI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ao-1604li/price/)
    * [CPS-AI-1608ALI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ai-1608ali/price/)
    * [CPS-AO-1608VLI](https://www.contec.com/en/products-services/daq-control/iiot-conprosys/cps-io-module/cps-ao-1604vli/price/)
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
* Serial Communication Functions Library Module [![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
    * libSerialFunc
        * Version.1.0.7
* cps-driver
    CPS-MCS341 Library Modules [![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
    * libCpsAio (Analog Input/Output)
        * Version 1.0.11
    * libCpsDio (Digital Input/Output)
        * Version 1.0.6
    * libCpsCnt (Counter)
        * Version 0.9.5
    * libCpsSsi (Sensor Input)
        * Version 1.0.6


    > Please build with the [driver](https://github.com/CONPROSYS/LINUX-SDK_driver) (Driver Version 1.3.3.2 or later).

* SubGiga Communication Module ( 920MHz )
    CMM-920 Module Library [![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
    * libConexio
        * Version 1.1.4

* EEPROM Library 
    * libCpsEeprom 
        * Version 0.1.1

## Change log
* Ver 1.3.3.2
    * libCpsAio 1.0.10 -> 1.0.11
        * Fixed MultiAo Function. This device do not update value in Channel 1 or later.


