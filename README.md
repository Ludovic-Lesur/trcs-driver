# Description

This repository contains the **TRCS** current sensor board driver.

# Hardware

The TRCS board was designed on **Circuit Maker V1.3**. Below is the list of hardware revisions:

| Hardware revision | Description | Status |
|:---:|:---:|:---:|
| [TRCS HW1.0](https://365.altium.com/files/C4EB9CA3-C92D-11EB-A2F6-0A0ABF5AFC1B) | Initial version. | :white_check_mark: |

# Dependencies

The driver relies on:

* An external `types.h` header file defining the **standard C types** of the targeted MCU.
* The **embedded utility functions** defined in the [embedded-utils](https://github.com/Ludovic-Lesur/embedded-utils) repository.

Here is the versions compatibility table:

| **trcs-driver** | **embedded-utils** |
|:---:|:---:|
| [sw1.2](https://github.com/Ludovic-Lesur/trcs-driver/releases/tag/sw1.2) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw1.1](https://github.com/Ludovic-Lesur/trcs-driver/releases/tag/sw1.1) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw1.0](https://github.com/Ludovic-Lesur/trcs-driver/releases/tag/sw1.0) | [sw5.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw5.0) to [sw6.2](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.2) |

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `TRCS_DRIVER_DISABLE_FLAGS_FILE` | `defined` / `undefined` | Disable the `trcs_driver_flags.h` header file inclusion when compilation flags are given in the project settings or by command line. |
| `TRCS_DRIVER_DISABLE` | `defined` / `undefined` | Disable the TRCS driver. |
| `TRCS_DRIVER_GPIO_ERROR_BASE_LAST` | `<value>` | Last error base of the low level GPIO driver. |
| `TRCS_DRIVER_TIMER_ERROR_BASE_LAST` | `<value>` | Last error base of the low level timer driver. |
| `TRCS_DRIVER_ADC_ERROR_BASE_LAST` | `<value>` | Last error base of the low level ADC driver. |
| `TRCS_DRIVER_ADC_RANGE_MV` | `<value>` | Conversion range of the low level ADC peripheral in mV. |
