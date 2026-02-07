### python script to create drum patterns for the 2HPico drum machine
## reads a MIDI file and extracts the drum patterns
## works best with GM drums but should work with any drum mapping
## cut and paste the output between the curly brackets and paste into a drum map file
## see the drum maps in 2HPico drums for the format
## patterns MUST be 32 steps and 4 patterns per node - generally will require some editing
### R Heslip Feb 2026

from mido import MidiFile

inputfilename='House 4otf 10.mid'
  
inputfile = MidiFile(inputfilename)

gmnames=["Bass Drum 1","Bass Drum 2","Stick","Snare","Clap","Snare","Low Tom","Closed hat","High Tom","Pedal hat","Low Tom","Open hat","Mid Tom","High-Mid Tom","Crash 1","High Tom","Ride","China","Bell","Tamb","Splash","Cowbell","Crash 2"]

velocity_scale=1.5 # scales the velocity - Grids patterns use 0-255 as a "strength" 
runningtime=0
min_delta=100000  #  make it large to start
rows,cols= 46, 32  # GM MIDI has 46 percussion instruments
velocities=[[] for _ in range(rows)]  # columns are created on the fly
times=[[] for _ in range(rows)]
instruments=[]
lastnoteontime=0
timesincelastnoteon=0
for i, track in enumerate(inputfile.tracks):
    print('Track {}: {}'.format(i, track.name))
    for message in track:
        runningtime=runningtime+message.time # MIDI files use relative time for events- calculate the absolute time
        if message.type=='note_on':
            timesincelastnoteon=runningtime-lastnoteontime
            if timesincelastnoteon > 20: # filter out small time variations
                if min_delta > timesincelastnoteon:
                    min_delta=timesincelastnoteon  # remember the smallest time between events
            lastnoteontime=runningtime
            if message.note not in instruments:
                instruments.append(message.note)  # add the new instrument if not seen before
            index=instruments.index(message.note)
            velocities[index].append(message.velocity)  # save event velocity and time
            times[index].append(runningtime)
        print(message,runningtime)

# add time 0 to the end so it doesn't index off the end below
for inst in instruments:
   index=instruments.index(inst)
   times[index].append(0)

steps=runningtime/min_delta

print("steps ",steps, " Time step ", min_delta)
print ("instruments ",instruments)

print ("{")
print('// Input file:',inputfilename)
print('// {}'.format( track.name))
for inst in instruments:
    index=instruments.index(inst)
    if inst in range(35,57):  # print instrument name if in General MIDI drums range
        print ("// ", inst,gmnames[inst-35])
    else:
        print ("// ", inst)

    time=0
    event=0
    
    for step in range (32):  # Grids uses 32 steps per pattern
        time=step*min_delta
        if (abs(times[index][event]-time) <10): # align events that are close
            print("%3d" % (velocities[index][event]*velocity_scale),",",sep='',end='')
            event=event+1
        else:
            print("%3d" % (0),"%1s" %(","),sep='',end='')
        if step== 15:
            print ("")
        
    print ("")

print ("};")
 
