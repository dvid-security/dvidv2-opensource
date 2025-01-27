# Escape Game

🚨 This EscapeGame is based on DVID version 1. The migration to DVIDv2 is still ongoing.
Contact: https://discord.gg/rWsZStSp

## Introduction
### Setup
To play the game, you need to have :

  * Your DVID board (in version 1)
  * Your flashing setup (ex.: avrdude)
  * Internet access
  * Bluetooth control tool (bleah, gatttool or nRFConnect on Android)
  * Have fun!

Also, you need to download and flash elements :
  * Download the bbq firmware : [Download](bbq.hex)
  * Download documentation here : [Download](bbq_datasheet.pdf)
  * Download decrypt python2 script : [Download](decrypt.py)
  * Download encrypted database here : [Download](database.db.enc)
  * Flash the bbq.hex on DVID

  ```bash
  avrdude -pm328p -cusbasp -u -U flash:w:bbq.hex -F
  ```

### Scope
The game perimeter is composed of :

  * The physical part of the game is done on the DVID board
  * The online part of the game is done only on bbqindustries.com root domain


### Support
In case you have any question, please reach out to : contact@dvid.eu or discord Vulcainreo#0358


**You are ready to play the game**

## Game storytelling
### The context

* The company Copernic Industries is an industrial system manufacturer
* The company has developed a Barbecue system to monitor the temperature of meat during cooking. The name is : BBQ
* The sensor (your DVID) needs to be installed near your favorite BBQ and connected to your home wifi
* Bluetooth connection is used for installation and remote control
* The system is composed of:
    * A sensor (the DVID)
    * A phone (local monitor and remote control)
    * A cloud service for alerting
*  The system works as described here:
    * The sensors sends temperature values to the phone over Bluetooth BLE
    * The sensors sends temperature values to cloud over IoT protocols
    * The sensor prints the current temperature

### The mission
You are working for Copernic industries and you CISO is asking you to investigate a security failure on the best seller product : the BBQ.

Your subcontractor seems to have been hacked and a backdoor might be installed deep in the product.
You have one hour to investigate and, if a backdoor is found, try to defuse it.

To achieve this, you have access to :

* All company resources
* Phone application database
* A fresh bbq device


## Game follow-up
### Limitations
* Online resources are located only on root domain ".copernicindustries.com"
* Sub-domain could be available, like xxx.copernicindustries.com
* Attack outside this domain will give no flag and may lead you to prison !
* Bruteforce is allowed with a reasonable throttle but in most of case useless
* Denial of service is forbidden (and of course absolutely useless)

### Progression follow-up
* During the game, you will find:
    * Flag : use to follow up you progression in the game
    * Hint : use to discover the enigma solution
    * Password : use to resolve enigma
* Finding a flag means that the step is finished
* The flag format is : FLAG-xxxxx
* Some data may be encrypted but keep in mind that a key should help you


## Conclusion
* The company name is : Copernic Industries
* The company website : copernicindustries.com
* Talk together if you have difficulties
* Ask the game master if you think you are in the wrong way
* Dig into your courses materials
* Please, respect game limitations
* A hint, a tool or a password can be used only once

**GL HF !**
