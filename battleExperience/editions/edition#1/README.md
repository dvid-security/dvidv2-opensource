# Battle Experience - Edition #1

## Theme
The theme of this edition is: hydroelectric energy production

## Setup
This edition takes place on the DVID beta board only.
The challenge starts when the board is powered-on and reset button (STM32 / ESP32) are pushed and released.

## Rules
In order to fight in correct and fair conditions, following rules needs to be respected :
* The challenge needs to be located only on the DVID board (no external servers either no data exchange with computer)
* The challenge can use both MCU (ESP32/STM32) at the same time or sequentially
* The challenge can implements mutlti-stage
* The challenge can use all protocols allowed by DVID hardware (ex.: bluetooth ,thread, UART, SPI)
* Instruction of the challenge must not exceed 200 chars.
* The flag needs to have the following format: FLAG-[16 chars] (ex.: FLAG-SUPERCHALLENGE!!)
* When the challenge is solved, the flag is printed on the screen or sends over UART protocol.
* During challenge creation, protect step-bypass needs to be implemented as much as possible

## Challenge submission
When an university submits its challenge, following elements are mandatory:
* The ESP32 firmware and source code
* The STM32 firmware and source code
* Step-by-step write-up of the challenge with technical details
* Instructions of the challenge (200 char maximum)
* One hit in case of blocking

## Challengers
This edition will fight following university :
* Challenger #1 : ESILV (France) on behalf of DaVinciCode [Facebook](https://www.facebook.com/DaVinciCodeCTF)
* Challenger #2 :  ??

## Planning
* From 10/02/2025: DVID board shipping to university
* From 17/02/2025 to 24/02/2025: challenge development
* On 25/02/2025 08h (FR Timezone): Closing of challenge submission
* From 25/02/2025 to 28/02/2025: challenge review and DVID dispatch to university
* From 03/03/2025 to 10/03/2025: challenge solving
* On 11/03/2025 18h (FR Timezone): Winner announcement
