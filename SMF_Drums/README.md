# SMF-Drums Standard MIDI file drum machine for 2HPico

April 2026 - still has sync problems to fix

Notes:

The sketch is set up for standard MIDI files with GM mapped drums. It will still work with non-GM mapping but it might be a little harder to map sample sounds to the individual drum tracks.

You MUST compile this sketch with space allocated for a file system in the /Tools/Flash Size Arduino menu. I suggest 512k because all the MIDI files get loaded into RAM and the Pico 2 only has 520kb RAM.

To load your MIDI files, hold the button while powering up. The LED will turn purple. Connect the Pico's USB port to a host computer it will appear as a flash drive. 

If this is the first time you have connected you should format the drive. You should only have to format the drive once - even if you change the code the flash drive should still be intact.

You MUST create four bank directories named "bank1", "bank2", "bank3" and "bank4". Drag and drop your MIDI files into these bank folders. There is a maximum of 32 MIDI files per bank. Any more than 32 files per bank will be ignored.
You should eject the drive when finished to ensure the file system does not get corrupted. Power the module off and back on for normal operation.

A fast flashing LED on power up means there was a problem with the file system or one of the files.

I strongly suggest you put only a few patterns in each bank until you get used to using the module and mapping drum sounds.

Drum sounds:

This one works the same way as the Grids sketch - put your drum samples in a subfolder of the sketch and run the wav2header22khz utility which creates a header file which you include in the sketch.
Wave2header22khz sorts the samples in alphabetical order. Its very helpful to have a silent sample and make sure its the first sample e.g. "00silence.wav". This allows tracks to be muted by turning the sample knob fully ccw.
You should also prefix the filenames with the GM note # of that sound e.g. "36Bassdrum.wav". The sketch will auto assign voices by mapping the GM note in the MIDI file to a sample with the same GM note number. 
You can have saveral samples with the same note # - it will assign the alphabetically first sample. If none of your sample names match the GM notes in the MIDI file it will map the GM note to the first sample - which will be silence if you follow the suggested naming prefix. You can assign these voices manually by using the knobs.

You need the updated version of wav2header22khz in this archive /resources which now supports assigning MIDI note number from the filename prefix. 




