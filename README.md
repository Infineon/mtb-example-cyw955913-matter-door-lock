# Matter AIROC&trade; door lock example

This code example makes use of Matter on an Infineon AIROC&trade; board to demonstrate its functionality as a Wi-Fi door lock device.

<a name="requirements"></a>

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/design-resources/development-tools/sdk/modustoolbox-software) v3.5 or later (tested with v3.6)
- Board support package (BSP) minimum required version for : v3.0.0
- Programming language: C/C++
- Associated parts: CYW955913EVK-01

<a name="supported toolchains"></a>


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; embedded compiler v14.2.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`

<a name="supported kits"></a>


## Supported kits (make variable 'TARGET')

- [CYW955913EVK-01 Wi-Fi Bluetooth&reg; Prototyping Kit](https://www.infineon.com/evaluation-board/CYW955913EVK-01) (`CYW955913EVK-01`)


<a name="software setup"></a>


## Software setup

See the [ModusToolbox&trade; user guide](https://www.infineon.com/product-information/modustoolbox-info-hub) for information about installing and configuring the tools package.

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://teratermproject.github.io/index-en.html).

<a name="hardware setup"></a>


## Hardware setup

This example uses the kit’s default configuration. See the respective kit guide to ensure that the kit is configured correctly.

> **Note:** The CYW55513EVK-01 Wi-Fi Bluetooth&reg; Prototyping Kit ships with KitProg3 v2.30 installed. ModusToolbox&trade; requires KitProg3 with latest v2.50. Before using this code example, make sure that the board is upgraded to KitProg3 v2.50. The tool and instructions are available in the [Firmware Loader](https://github.com/Infineon/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

<a name="Using the code example"></a>

## LCD Touch Shield

The 2.8" TFT Touch Shield for Arduino with Capactive Touch may be used with this code example.
The LCD is available from [Adafruit](https://www.adafruit.com/product/1947) or
[Amazon](https://www.amazon.com/Adafruit-1947-Shield-Arduino-Capacitive/dp/B00R3R65C0).

### LCD Modifications
By default, several of the SPI lines are routed to the 2x3 ICSP header on the LCD shield. These need to be
routed to the Arduino header. The ICSP jumpers need to be cut and the SPI jumpers soldered closed.

<ul>
<li>ICSP jumpers - These jumpers are located below the 2x3 header on the shield. They are labeled ICSP and outlined in white on the board silk. These jumpers can be cut to disconnect the SPI pins from the 2x3 header.</li>
<li>SPI jumpers - These jumpers are located directly below the ICSP jumpers. They are labeled 13, SO, SI/11 and are outlined in white on the board silk. You can solder these jumpers closed to connect the SCK, MISO and MOSI pins to GPIO pins:</li>
<ul>
<li>MOSI to pin 11</li>
<li>MISO to pin 12</li>
<li>SCK to pin 13</li>
</ul>
</ul>

The jumpers for the backlight and the touchscreen interrupt should also be soldered closed.

<ul>
<li>The TFT backlight jumper is located next to the TS int jumper and is labeled backlight/#5 on the board silk</li>
<li>The touchscreen interrupt pin jumper is located to the left of backlight jumper and is labeled TS int/#7 on the board silk</li>
</ul>

### CYW955913EVK-01 Settings

Before mounting the LCD Touch Shield on the CYW955913EVK-01, the CYW955913EVK-01 board must be configured correctly.

<ul>
<li>Set jumper J20 to connect pins 1 and 3 to set the VIOREF voltage selection jumper to 3.3V</li>
<li>Remove jumper J10 to disconnect the potentiometer and route LHL_GPIO_8 to the Arduino header</li>
<li>Set SW5 switches to OFF to route BT_GPIO_16 and BT_GPIO_17 from the LEDs to the Arduino header</li>
</ul>

### Mount the LCD Touch Shield

Once the modifications have been made to the LCD Touch Shield and the proper settings applied to the CYW955913EVK-01, the
LCD board should be mounted on the Arduino header on the CYW955913EVK-01 board.

### Building the LCD enabled lock application

The LCD support in the lock application is enabled by setting the defines **ENABLE_UI** and **LV_CONF_INCLUDE_SIMPLE** in
the application Makefile.

## Using the code example

### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/row/public/documents/30/44/infineon-modustoolbox-project-creator-user-guide-usermanual-en.pdf) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*).

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target)

   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**

   > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you.

   b.	Select this code example from the list by enabling its check box

   > **Note:** You can narrow the list of displayed examples by typing in the filter box.

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**

   d. Click **Create** to complete the application creation process

</details>

<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[mtb-example-threadx-hello-world](https://github.com/Infineon/mtb-example-threadx-hello-world)" application with the desired name "MyDoorLock" configured for the *CYW955913EVK-01* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CYW955913EVK-01 --app-id mtb-example-cyw955913-matter-door-lock --user-app-name MyDoorLock --target-dir "C:/mtb_projects"
   ```


The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br>

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/row/public/documents/30/68/infineon-modustoolbox-tools-package-quick-start-guide-gettingstarted-en.pdf) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).

</details>



### Open the project

After the project has been created, you can open it in your preferred development environment.


<details><summary><b>Eclipse IDE</b></summary>

If you opened the Project Creator tool from the Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/row/public/documents/30/44/infineon-modustoolbox-eclipse-ide-user-guide-usermanual-en.pdf) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/row/public/documents/30/44/infineon-visual-studio-code-user-guide-usermanual-en.pdf) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>




<details><summary><b>Command line</b></summary>

If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command-line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/row/public/documents/30/68/infineon-modustoolbox-tools-package-quick-start-guide-gettingstarted-en.pdf) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation


1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector

2. Open a terminal program and select the debug COM port. Set the serial port parameters to 8N1 and 115200 baud

   > **Note** When building the application with the LCD enabled, the HCI UART should be used rather than the Peripheral UART as the Peripheral UART lines are used for the SPI interface. The terminal program should be
disconnected from the UART when programming the application and when booting/resetting the application as the application will
not boot properly with the terminal program connected to the HCI UART. The terminal program should be
connected to the UART when the Infineon splash screen appears on the display.

3. Program the board using one of the following:

   <b>Using Eclipse IDE for ModusToolbox&trade;</b>

      1. Select the application project in the Project Explorer

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program**

   <b>Using CLI</b>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:
      ```
      make program TARGET=APP_<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TARGET=APP_CYW955913EVK-01 TOOLCHAIN=GCC_ARM
      ```

4. After programming, press the reset button on the board. Confirm that the expected output is displayed on the UART terminal

    ```
    CHIP:CYW955913: ====================================================
    CHIP:CYW955913: chip-CYW955913-lock-app starting Version 1
    CHIP:CYW955913: ====================================================
    ```

## Matter Controllers

The door lock application is compatible with the standard Matter controllers such as Apple Home, Google Home,
Amazon Alexa, and Samsung SmartThings. Each controller application will have their own method of commissioning
a Matter device onto the Matter fabric for that controller. Please refer to the instructions for the controller
you are using for how to commission a Matter device.

>**Note:** You may get a warning from the controller application that the door lock application is not a certified
Matter device. This is because the application is using a development Device Attestation Certificate (DAC). You may
need to select an option allowing you to commission devices that are not certified with the controller you use. Please
refer to the controller application instructions for how to do so.

## Setting up chip-tool

Chip-tool is a command line interface reference Matter controller implementation used for developing, commissioning, and controlling
Matter accessory devices. It is maintained by the Matter community and is available from the [Matter GitHub](https://github.com/project-chip/connectedhomeip) site.

Once the application is running, set up chip-tool on Raspberry Pi 4 to perform commissioning and cluster control.

-   Set up python controller

           $ cd {path-to-connectedhomeip}
           $ ./scripts/examples/gn_build_example.sh examples/chip-tool out/debug

-   Execute the controller

           $ ./out/debug/chip-tool


## Commissioning over Bluetooth&reg; LE

- Run the built executable file and with the following parameters: 

   - discriminator 
   - Pairing code of the remote device
   - Network credentials 


         $ ./out/debug/chip-tool pairing ble-wifi 1234 ${SSID} ${PASSWORD} 20202021 3840

         Parameters:
         1. Discriminator: 3840
         2. Setup-pin-code: 20202021
         3. Node ID: 1234 (you can assign any node id)
         4. SSID : Wi-Fi SSID
         5. PASSWORD : Wi-Fi Password

<a name="Notes"></a>


> **Note:** Raspberry Pi 4 Bluetooth&reg; LE connection issues can be avoided by running the following commands. These power cycle the Bluetooth&reg; hardware and disable BR/EDR mode.

          $ sudo btmgmt -i hci0 power off
          $ sudo btmgmt -i hci0 bredr off
          $ sudo btmgmt -i hci0 power on

<a name="Cluster control"></a>


## Cluster control

-   After successful commissioning, use the Lock/Unlock cluster command to toggle device between Lock or Unlock states

    `$ ./out/debug/chip-tool doorlock lock-door 1234 1 --timedInteractionTimeoutMs 100`

    `$ ./out/debug/chip-tool doorlock unlock-door 1234 1 --timedInteractionTimeoutMs 100`

-   The Lock/Unlock status of the door lock can be observed with 'Red LED' on the board. This LED is configured with `LOCK_STATE_LED` in `include/AppConfig.h`

<a name="Factory Reset"></a>


## Factory Reset

> **Note:** Follow this step only if we need to commission the board to different access point SSID.

-	Rebooting the board after commissioning will not clear all Wi-Fi Credentials and Commissioning data. The only way to change Wi-Fi credentials is to factory reset and rerun commissioning process
-	Press the "USER BTN" on the board for 2 seconds and release it to clear Commissioning data so that board can be commissioning to different Access point
-	Make sure to check the logs on Serial terminal to confirm if factory reset is triggered
```
CHIP:CYW955913: Factory Reset Triggered. Press the button again within 5s to cancel.
```


<a name="OTA Software Update"></a>


## OTA software update
-  Over The Air (OTA) support can be included in the Wi-Fi door lock application by following either of these routes:


### Using the Infineon OTA middleware library
The OTA library provides support for Over-The-Air update of the application code running on an Infineon device. The ModusToolbox&trade; OTA code examples import the ota-update library automatically. This application is built in ModusToolbox&trade; software, integrated with the OTA library.

For more information refer - [OTA_using_ModusToolbox] https://github.com/Infineon/ota-update


### Using MATTER (project-chip repository)

Matter provides a system for OTA to be run as a provider requestor.

For further information, head to [Matter] https://github.com/project-chip/

<a name="AIROC&trade; Deep Sleep"></a>


## AIROC&trade; Deep Sleep

- AIROC&trade; can put to Deep Sleep for longer period of time by suspending Network stack and enabling WLAN Offload features using [LPA](https://github.com/Infineon/lpa).

- Refer to the LPA Quick Start Guide - [Code Snippets](https://github.com/Infineon/lpa/blob/master/docs/api_reference_manual/html/group__lpautilities.html) for more information on APIs to be used.

- Network stack suspend/resume logic should be added after Wi-Fi Connection is successfully established. This can be done in `AppTask::Init` function after starting `DnssdServer`.

```
    if (event->InternetConnectivityChange.IPv4 == kConnectivity_Established ||
        event->InternetConnectivityChange.IPv6 == kConnectivity_Established)
    {
        chip::app::DnssdServer::Instance().StartServer();
        //Create new task which calls wait_net_suspend API implemented in LPA
    }
```

- Configure Various WLAN offloads using the ModusToolbox&trade; Device Configurator. Refer to the LPA [Quick Start Guide](https://github.com/Infineon/lpa/blob/master/docs/api_reference_manual/html/index.html#section_lpa_getting_started)


## Design and Implementation

### Resources and settings

**Table 2. Application resources**

 Resource  |  Alias/object     |    Purpose
 :-------- | :-------------    | :------------
 UART (HAL)    |cy_retarget_io_uart_obj| UART HAL object used by retarget-io for the Debug UART port
 GPIO (HAL)    | CYBSP_USER_LED        | User LED
 GPIO (HAL)    | CYBSP_USER_BTN        | User button

<br />


## Related resources

Resources  | Links
-----------|----------------------------------
Device documentation | Contact [Infineon Support](https://www.infineon.com/support) for device documentation
Development kits | Contact [Infineon Support](https://www.infineon.com/support) for kit samples
Libraries on GitHub  | [mtb-pdl-cat5](https://github.com/Infineon/mtb-pdl-cat5) – CYW55913 Peripheral Driver Library (PDL)  <br> [mtb-hal-cat5](https://github.com/Infineon/mtb-hal-cat5) – Hardware Abstraction Layer (HAL) library <br> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Tools  | [ModusToolbox&trade;](https://www.infineon.com/design-resources/development-tools/sdk/modustoolbox-software) – ModusToolbox&trade; software is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSOC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices, XMC&trade; Industrial MCUs, and EZ-USB&trade;/EZ-PD&trade; wired connectivity controllers. ModusToolbox&trade; incorporates a comprehensive set of BSPs, HAL, libraries, configuration tools, and provides support for industry-standard IDEs to fast-track your embedded application development.

<br>


## Other resources

Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.

Lock/unlock images by pngtree.com

## Document history


Document title: *CE236478* - *Wi-Fi door lock*

| Version | Description of change |
| ------- | --------------------- |
| 1.5.2   | Add LCD display support|
| 1.4.2   | New code example for Wi-Fi door lock|

<br>

All referenced product or service names and trademarks are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.


---------------------------------------------------------

(c) 2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG. All rights reserved.
This software, associated documentation and materials ("Software") is owned by Infineon Technologies AG or one of its affiliates ("Infineon") and is protected by and subject to worldwide patent protection, worldwide copyright laws, and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software. If no license agreement applies, then any use, reproduction, modification, translation, or compilation of this Software is prohibited without the express written permission of Infineon.
<br>
Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC USE/PURPOSE OR MERCHANTABILITY. Infineon reserves the right to make changes to the Software without notice. You are responsible for properly designing, programming, and testing the functionality and safety of your intended application of the Software, as well as complying with any legal requirements related to its use. Infineon does not guarantee that the Software will be free from intrusion, data theft or loss, or other breaches (“Security Breaches”), and Infineon shall have no liability arising out of any Security Breaches. Unless otherwise explicitly approved by Infineon, the Software may not be used in any application where a failure of the Product or any consequences of the use thereof can reasonably be expected to result in personal injury.
