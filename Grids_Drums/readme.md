Grids_Drums - Drum machine sketch for 2HPico

This app combines the drum sequencer from Mutable Intruments Grids with sample player code I have used in previous projects such as PicoBeats. It adds another channel to Grids so you can play up to 4 drum samples from the Pico 2's flash memory. See the comments at the top of the sketch for usage of the jacks and controls.

The sketch is set up for 22khz mono samples to save memory since the Waveshare Pico2 module only has 4mb of flash. Not tested yet at 44khz but it should work OK. This code should also run on the 2HPico DSP board if you configure the hardware correctly but that has not been tested yet either.

There is a tool in the resources directory of this archive for easily making new drum kits. Make a subdirectory in your sketch folder, drop .wav file samples in it and put a copy of the wav2header22khz.exe tool in it as well. Click on the .exe file and it will convert the .wav files into the required header files. In your sketch, include the file "samples.h" from the directory you created. It has to be included in the same place in the source code as the example kits - NOT at the top of the sketch. You can only have one set of samples included at a time.

There are two drum maps included: 

grids4ch_resources.h which is the Mutable Instruments drum map expanded to four channels

grids4ch_resources2.h which is the drum map from the Phazerville suite for Ornament and Crime also expanded to four channels. It has a different set of drum sequences.