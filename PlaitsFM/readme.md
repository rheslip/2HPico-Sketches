# PlaitsFM sketch for 2HPico - Feb 19/2026 

This is a modified version of the Plaits sketch with only the 6opFM engine for playing Yamaha DX7 patches. It has 8 banks of 32 DX7 patches. See the sketch header for more info.

The 6opFM lib is the Plaits library stripped down for just 6opFM. Moved the patch bank files from resources.h to the sketch folder

I wrote a utility sysx2bank which batch converts Dx7 .syx patch bank files into a patch bank header file which should be placed in the sketch folder. This makes it very easy to change the patches used in the sketch - there are thousands of DX7 patches on the internet and most are in .syx format. The utility is in the resources folder of this archive.

You will need copies of 6opFM and STMLIB in your Arduino/Libraries folder in order to compile the PlaitsFM code.