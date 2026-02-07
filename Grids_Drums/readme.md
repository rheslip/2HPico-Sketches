Grids_Drums - Drum machine sketch for 2HPico

This app combines the drum sequencer from Mutable Intruments Grids with sample player code I have used in previous projects such as PicoBeats. It adds another channel to Grids so you can play up to 4 drum samples from the Pico 2's flash memory. See the comments at the top of the sketch for usage of the jacks and controls.

The sketch is set up for 22khz mono samples to save memory since the Waveshare Pico2 module only has 4mb of flash. Not tested yet at 44khz but it should work OK. This code should also run on the 2HPico DSP board if you configure the hardware correctly but that has not been tested yet either.

There is a tool in the resources directory of this archive for easily making new drum kits. Make a subdirectory in your sketch folder, drop .wav file samples in it and put a copy of the wav2header22khz.exe tool in it as well. Click on the .exe file and it will convert the .wav files into the required header files. In your sketch, include the file "samples.h" from the directory you created. It has to be included in the same place in the source code as the example kits - NOT at the top of the sketch. You can only have one set of samples included at a time.

There are three drum maps included: 

grids4ch_resources.h which is the Mutable Instruments drum map expanded to four channels

grids4ch_resources2.h which is the drum map from the Phazerville suite for Ornament and Crime also expanded to four channels. It has a different set of drum sequences.

testmap1.h is a drum map I created from EDM style MIDI drum files using the MIDI2drums.py utility found in the resources directory

Feb 7/2026 

Added no interpolation feature - double clicking the button on the RED page will turn the LED Orange which indicates drum map interpolation is turned off. In this case you will have 25 drum patterns with no "morphing" between patterns. The X and Y pots work the same way but they have only 5 pattern selections each vs continuous in the interpolation mode. This is useful if you want to select a specific pattern or hear what the pattern sounds like with no influence from the adjacent patterns in the drum map.

Added manual reset feature - hold the button to force the sequencer to step 0. Useful to manually sync to other sequencers or to get a roll effect from whatever is on the first drum sequencer step.