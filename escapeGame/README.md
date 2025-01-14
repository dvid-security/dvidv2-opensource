# Escape Game

🚨 This EscapeGame is based on DVID version 1. The migration to DVIDv2 is incoming.

## Introduction
### Setup
To play the game, you need to prepare :

  * Your DVID board (in version 1)
  * Flashing setup (ex.: avrdude)
  * Internet access
  * Bluetooth control tool (bleah, gatttool or nRFConnect on Android)
  * Take fun

Also, you need to download and flash some elements :
  * Download the tms firmware : [Download](tms.hex)
  * Download documentation here : [Download](tms_datasheet.pdf)
  * Download decrypt python2 script : [Download](decrypt.py)
  * Download encrypted database here : [Download](database.db.enc)
  * Flash the tms.hex on DVID

  ```bash
  avrdude -pm328p -cusbasp -u -U flash:w:tms.hex -F
  ```

### Scope
The game perimeter is composed of :

  * Physical play will be done on the DVID board
  * Online play will be done only on tmsindustries.com root domain


### Support
In case of questions : contact@dvid.eu or discord Vulcainreo#0358


**You are ready to play the game**

## Game storytelling
### The context

* The company TMS Industries is an industrial system manufacturer
* The company has developped an system to monitor the temperature. The name is : tms
* The sensor (your DVID) needs to be installed near your favoriture tms and connected to home wifi
* Bluetooth connection is used for installation and remote control
* The system is composed by
    * Sensor (the DVID)
    * A phone (local monitor and remote control)
    * A cloud service for alerting
*  The system works as
    * The sensors sends temperature value to the phone over Bluetooth BLE
    * The sensors sends temperature value to cloud over IoT protocols
    * The sensor prints current temperature

### The mission
You are working to TMS industries and you CISO is asking you to investigate about a security failure on the best seller product : the tms.

Your subcontractor seems to have been hacked and a backdoor might be installed deeper.
You have one hour to investigate and, if a backdoor is found, try to defuse it

To acheive, you have acces to :

* All company ressource
* Phone application database
* The fresh produced tms device


## Game follow-up
### Limitations
* Online resources are located only on root domain ".tmsindustries.com"
* Sub-domain could be available like xxx.tmsindustries.com
* Attack outside this domain will give no flag and prison !
* Bruteforce is allowed with reasonable throttle
* Denial of service is forbidden (and of course absolutely useless)

### Progression follow-up
* During the game, you will find
    * Flag : use to follow up you progression in the game
    * Hint : use to discover the enigma solution
    * Password : use to resolv the enigma
* Finding a flag means that the step is finished
* The flag format is : FLAG-xxxxx
* Some data may be encrypted but keep in mind that a key should help you


## Conclusion
* The company name is : TMS Industries
* The company website : tmsindustries.com
* Talk together if you have difficulties
* Ask to the game master if you think you are in the wrong way
* Dig into your courses materials
* Please, respect game limitations
* A hint, a tool or a password can be used only once

**GL HF !**
