MIDI2Drums.py  Feb 6/2026 R Heslip


This Python script will take a drum pattern MIDI file as input and output drum patterns in a format that is compatible with the Grids_Drums sketch. Its still experimental and has not been tested much but it does produce usable results.

Place the script in the same directory as the files you want to convert. You will have to edit the "inputfilename" variable to point it to the MIDI file you want to process. Run the script and it will produce a dump of the MIDI file contents, a summary of instruments used, number of steps detected, some timing info and lastly the drum patterns.

Copy the last section of the output between the "{" and the "};" which is formatted as a C initializer. Paste that into one of the nodes in the template file included in the Grids_drums sketch folder. There are more instructions in the template file.

I made a new drum map "testmap1.h" using a bunch of EDM style MIDI drum sequences. It sounds OK and there is lots of variety in it but its pretty hard to dial in any one of those specific patterns because of the way the Grids code interpolates across the drum map. Lots of fun to experiment with tho.