modified version of Paul Stoffregen's wav2sketch utility https://raw.githubusercontent.com/PaulStoffregen/Audio/master/extras/wav2sketch/wav2sketch.c

creates the required sample header files for the Grids_Drums sketch

usage: put your 22 or 44khz samples into a subdirectory within the sketch folder along with a copy of the utility. 

run wav2header22khz - it will generate header files for all the samples (note that it may change the names to make them C compatible). 44khz samples will be resampled to 22khz. 

The wav2header44khz tool will generate 44khz header files from 44khz samples.

The tool generates an include file "samples.h" with all the samples, a sample table "sampledefs.h" and C header files for each of the samples.

Include the file "samples.h" in the Grids_Drums sketch - see the sketch for where to put this include (DON'T include it at the beginning!)

Note that the tool will simplify long sample names so they fit into 20 characters. Thiscould cause name collisions when you compile - the solution is to rename the problematic input files. 

Note also that the tool will put your samples in alphabetical order to facilitate grouping samples by name.

Oct 2023 - I used this tool for a couple of RP2040 Pico projects at 22khz and 44khz so there are two versions of the source. Could have added a command line switch but Windows 10 makes it so hard to run command line anything this is probably easier. 
Put the appropriate .exe file in a subdirectory with the samples you want converted to headers, click on the .exe and the magic happens.

R Heslip Feb 2019

