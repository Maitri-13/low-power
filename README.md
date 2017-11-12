# low-power
ECEN5023-002 Final project- This is a project to design a intrusion-detection device that can send a notification to a user.

## Maintainers
This repository is maintained by [Maitri Chatthopadhyay](https://github.com/Maitri-13), [Akshit Shah](https://github.com/Akshit21), and [Chase Stewart](https://github.com/ChaseStewart)

## Architecture
From a high level, the hardware and software of the project combine to implement the following flow:
![project flow](https://github.com/Maitri-13/low-power/raw/master/Documentation/img/High_Level_Diagram_detail.PNG)

To complete this, we will use the following HW peripherals:
![HW Block Diagram](https://github.com/Maitri-13/low-power/raw/master/Documentation/img/MCU_Peripherals.png)


## Hardware
For those who simply wish to use the Hardware outputs, please consult the `Hardware/Outputs/` folder. It will hold the Gerber files, schematic, and BOM.

The hardware portions of the project are implemented as a set of Altium Designer files. The key components of the altium project include: * the project-specific component library (`Hardware\Libraries\LowPower_3ER_Parts.IntLib`).
* the project file (`Hardware/LPEDT.PrjPcb`).
* the schematic and PCB files (`Hardware/*.SchDoc` and `Hardware/*.PcbDoc`).


## Software
The software will be split into a few sections:

* PanoptiNode - the embedded EFM32PG code for our PCB itself
* Application - the code for the notification application.

