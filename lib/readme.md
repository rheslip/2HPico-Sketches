# 2HPico libraries

2HPico is small support library for the 2HPico module - Put it in your Arduino/Libraries folder

Feb 17/2026 - added some more RGB color defs to 2HPicolib for Plaits

Feb 17/2026 Added slightly tweeked version of arduinoMI Plaits 1.2 and STMLIB 1.2 by poetaster https://github.com/poetaster/arduinoMI

Library would compile but linker had multiple defs because of stmlib_all.cpp and plaits_all.cpp. I deleted the .cpp files and saved a .zip copy.

You will need copies of PLAITS and STMLIB in your Arduino/Libraries folder in order to compile the Plaits code

Feb 19/2026 - added 6opFM lib which is the Plaits library stripped down for just 6opFM. Moved the patch bank files from resources.h to the sketch folder

You will need copies of 6opFM and STMLIB in your Arduino/Libraries folder in order to compile the PlaitsFM code

March 14/2026 - added Synth_Braids library which originated at https://codeberg.org/positionhigh/MicroDexed-touch/src/branch/main/third-party. Required for Pico_Audio_Braids sketch