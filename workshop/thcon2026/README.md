# Challenge DVID
## Setup

Pour ce challenge, vous devez flasher votre badge DVID avec le micrologiciel suivant :
[firmware.bin](./firmware.bin)

La commande suivante peut être utilisée :
```bash
esptool.py --port /dev/ttyUSB0 --baud 115200 --chip esp32 write_flash 0x10000 firmware.bin
```

Trace console (au cas où)
```bash
esptool.py --port /dev/ttyUSB0 --baud 115200 --chip esp32 write_flash 0x10000 firmware.esp32
esptool.py v4.10.dev2
Serial port /dev/ttyUSB0
Connecting.....
Chip is ESP32-D0WD-V3 (revision v3.1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 78:ee:4c:33:74:d8
Uploading stub...
Running stub...
Stub running...
Configuring flash size...
Flash will be erased from 0x00010000 to 0x0005afff...
Compressed 304432 bytes to 170034...
Wrote 304432 bytes (170034 compressed) at 0x00010000 in 15.0 seconds (effective 162.7 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```


## Le challenge
Votre badge est un cadenas connecté protégé par un mot de passe. D'après nos informations le mot de passe est un **secret** qui est **secretement** bien gardé **secret**. Saurez-vous le retrouver ?

Le cadenas s'utilise en Bluetooth avec l'application NRFConnect.





# DVID Challenge
## Setup

For this challenge, you must flash your DVID badge with the following firmware:
[firmware.bin](./firmware.bin)

The following command can be used:
```bash
esptool.py --port /dev/ttyUSB0 --baud 115200 --chip esp32 write_flash 0x10000 firmware.bin
```

Trace console (au cas où)
```bash
esptool.py --port /dev/ttyUSB0 --baud 115200 --chip esp32 write_flash 0x10000 firmware.esp32
esptool.py v4.10.dev2
Serial port /dev/ttyUSB0
Connecting.....
Chip is ESP32-D0WD-V3 (revision v3.1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 78:ee:4c:33:74:d8
Uploading stub...
Running stub...
Stub running...
Configuring flash size...
Flash will be erased from 0x00010000 to 0x0005afff...
Compressed 304432 bytes to 170034...
Wrote 304432 bytes (170034 compressed) at 0x00010000 in 15.0 seconds (effective 162.7 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```

## The challenge
Your badge is a connected padlock protected by a password. According to our information, the password is a **secret** that is **secretly** well kept **secret**. Will you be able to find it?
The padlock is used via Bluetooth with the NRFConnect application.
