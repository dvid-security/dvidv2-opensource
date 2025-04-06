# Write-up of the Helloword Edition

## Setup
xxxx

## Solution
xxxx


Un drone a survolé une zone interdite
Le drone expose du Wifi (mydrone) et du bluetooth. Le bluetooth permet l'apparaillage.
Les fonctions de vol sont sur ESP32 et STM32 gère les échanges ave le pilote

stage 1: capturer la séquence de boot en UART ESP32 / SMT32 pour cartographier l'usage des MCU
stage 2: récupérer la clé Wifi via bluetooth
stage 3: se connecter au wifi et récupérer le fichier de point GPX et récupérer le flag dessiné par les points GPS.
