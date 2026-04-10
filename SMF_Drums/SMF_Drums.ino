
// Copyright (c) 2026, Rich Heslip
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
/*
 R Heslip Drum Machine app for 2HPico eurack module March 2026
 Plays standard MIDI file drum patterns. The sketch is set up for standard MIDI files with GM mapped drums which is generally how MIDI drum files are set up. 
 It will work with non-GM mapping but it might be a little harder to map sounds to the individual drum tracks by ear.

Notes:
You MUST compile this sketch with space allocated for a file system in the /Tools/Flash Size Arduino menu. I suggest 512k because all the MIDI files get loaded into RAM and the Pico 2 only has 520kb RAM.
To load your MIDI files, hold the button while powering up. The LED will turn purple. Connect the Pico's USB port to a host computer it will appear as a flash drive. 
If this is the first time you have connected you should format the drive. You should only have to format the drive once - even if you change the code the flash drive should still be intact.
You MUST create four bank directories named "bank1", "bank2", "bank3" and "bank4". Drag and drop your MIDI files into these bank folders.
There is a maximum of 32 MIDI files per bank. Any more than 32 files per bank will be ignored.
You should eject the drive when finished to ensure the file system does not get corrupted. Power the module off and back on for normal operation.
A fast flashing LED on power up means there was a problem with the file system or one of the files.
I strongly suggest you put only a few patterns in each bank until you get used to mapping samples to MIDI tracks by ear.

Drum sounds:
This one works the same way as the Grids sketch - put your drum samples in a subfolder of the sketch and run the wav2header22khz utility which creates a header file which you include in the sketch. 
Wave2header22khz sorts the samples in alphabetical order. Its very helpful to have a silent sample and name it so its the first sample e.g. "00silence.wav". 
This allows tracks to be muted by turning the sample knob fully ccw. You should also prefix the sample filenames with the GM note # of that sound e.g. "36Bassdrum.wav". 
The sketch will auto assign voices by mapping the GM note in the MIDI file to a sample with the same GM note number in its prefix. 
You can have saveral samples with the same GM note #. In this case it will assign the alphabetically first sample, which you can override by using the knobs.
If none of your sample names match the GM notes in the MIDI file it will map that GM note to the first sample which will be silence if you follow the suggested naming prefix. 
You can assign any voice manually by using the knobs - autoassignment provides a fast way to preview MIDI files without a lot of knob twisting.


 top jack - clock input - clocks on +ve edge. clock must be at 1/16 note rate ie 16 clocks per bar for 4/4 time signature.
 middle jack - reset input to sync with other sequencers - resets on +ve edge
 bottom jack - audio out

 button - click to advance to next page
 - hold to manually reset to first step
 - hold when powering up to enter flash drive mode

 page 1 parameters - Red LED (flashes when clock is present)
 Pot 1 - bank selection. 7-9 O'clock bank 1, 9-12 bank 2, 12-3 O'clock bank 2, 3-5 O'clock bank 4
 pot 2 - MIDI pattern selection within the selected bank
    ** note that all tracks are assigned in ascending order by GM note #  **
 pot 3 - Selects drum track 1 sample - for GM files this will always be the bass drum if a bass drum is in the MIDI pattern
 pot 4 - Selects drum track 2 sample - will always be the next GM sound after the bass track, usually snare

 page 2 parameters - Green LED
 Pot 1 - Selects drum track 3 sample  
 pot 2 - Selects drum track 4 sample
 pot 3 - Selects drum track 5 sample
 pot 4 - Selects drum track 6 sample

 page 3 parameters Aqua LED
 Pot 1 - Selects drum track 7 sample
 Pot 2 - Selects drum track 8 sample
 Pot 3 - Selects drum track 9 sample
 Pot 4 - Selects drum track 10 sample
*/

#include "2HPico.h"
#include <I2S.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "pico/multicore.h"
//#include "LittleFS.h"
#include <FatFS.h>
#include <FatFSUSB.h>
#include "midi-parser.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MONITOR_CPU1  // define to enable 2nd core monitoring
//#define DEBUG   // comment out to remove debug code
//#define dumpMIDI  // define to dump MIDI file info 

//#define SAMPLERATE 11025 
#define SAMPLERATE 22050  // saves CPU cycles
//#define SAMPLERATE 44100


Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

I2S DAC(OUTPUT);  // 

// constants for integer to float and float to integer conversion
#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

#define CLOCKIN TRIGGER  // top jack is clock
#define RESETIN AIN1   // middle jack used to reset the sequencer

#define CLKDELAYCOUNT 5
bool playing=0;
bool clocked=0;
bool resetedge=0;
bool loopsynced=1;  // flags end of file and resync to clock needed
uint16_t clkdelay=CLKDELAYCOUNT;  // wait this many clocks after reset or stopped clock to measure clock again
uint32_t lastclock,clockperiod,edgetime,nextMIDIevent; // clock related timers that use micros()
uint32_t eventtimeleft; // keeps track of next event when clock is stopped in useconds
uint32_t totaltime,looptime; // total time of the file and the adjusted loop time in ppqn ticks
uint32_t accumtime; // accumulated time in the loop
int32_t totalnotes,notes; // number of note on events in the current file
//
int32_t clocktimer,resetdebouncetimer,ledtimer,banktimer,filetimer; // various timer counts

#define POTAUTOLOCK 1000 // lock pot 0 (bank) and pot 1 (file) setting in after its been stable for this long
#define LEDOFF 25 // LED trigger flash time 

// clickbutton library is used to detect clicks, doubleclicks, holds etc
ClickButton button1 (BUTTON1);
#define BUTTON_TIMER_MICROS 10000 // 100hz for button

int16_t ppqn=96;  // this changes - depends on the MIDI file

// RP2040 timer code from https://github.com/raspberrypi/pico-examples/blob/master/timer/timer_lowlevel/timer_lowlevel.c
// Use alarm 0
#define ALARM_NUM 0
#define ALARM_IRQ timer_hardware_alarm_get_irq_num(timer_hw, ALARM_NUM)

static void alarm_in_us(uint32_t delay_us) {
  hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
  irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
  irq_set_enabled(ALARM_IRQ, true);
  alarm_in_us_arm(delay_us);
}

static void alarm_in_us_arm(uint32_t delay_us) {
  uint64_t target = timer_hw->timerawl + delay_us;
  timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}

static void alarm_irq(void) {
  button1.service();    // check the button input
  hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM); // clear IRQ flag
  alarm_in_us_arm(BUTTON_TIMER_MICROS);  // reschedule interrupt
}

#define NUMUISTATES 3
enum UIstates {SET1,SET2,SET3} ;
uint8_t UIstate=SET1;


#define NUM_VOICES 10  // 
struct voice_t {
  int16_t sample;   // 
} voice[NUM_VOICES] = {
  1,      // default voice 0 assignment - will be the bass drum if file is GM
  0,      // default voice 1 assignment - rest of voices should start as silence which makes it much easier to pick sounds by ear
  0,    // default voice 2 assignment 
  0,    // default voice 3 assignment 
  0,      // default voice 4 assignment 
  0,      // default voice 5 assignment 
  0,    // default voice 6 assignment 
  0,    // default voice 7 assignment 
  0,    // default voice 8 assignment 
  0,    // default voice 9 assignment 
};  


// we can have an arbitrary number of samples but you will run out of memory (or CPU) at some point
// sound sample files must be 22khz 16 bit signed PCM format - see the sample include files for examples
// sample files are compiled into arrays and stored in program flash
// if your sketch is too big you may have to fiddle with the flash partition scheme to enable more of the flash to be used for program space

// the header files can be auto generated by the wav2header utility
// put your 22khz or 44khz PCM wav files in a sample directory, run the utility and it will generate all the header files

// include only ONE sample header file
//#include "808samples/samples.h" // 808 sounds
//#include "Angular_Jungle_Set/samples.h"   // Jungle soundfont set - great!
//#include "Angular_Techno_Set/samples.h"   // Techno
//#include "Acoustic3/samples.h"   // acoustic drums 
//#include "Pico_kit/samples.h"   // assorted samples
//#include "testkit/samples.h"   // assorted samples
//#include "EDM_kits/samples.h"   // Techno, Pop, Trap, House
#include "EDM/samples.h"   // Techno, Pop, Trap, House
//#include "House/samples.h"   // House 808 909 style kit

#define NUM_SAMPLES (sizeof(sample)/sizeof(sample_t))
#define NUM_BANKS 4
#define MAX_FILES_PER_BANK 32

int16_t currentbank,currentfile,lastbank,lastfile;

struct bankinfo {
  uint8_t * buffer[MAX_FILES_PER_BANK];
  int32_t buffersize[MAX_FILES_PER_BANK];
  int16_t numfiles;
} patternbank[NUM_BANKS];

// load the slots in a bank from a directory in the file system
int16_t readbank(int16_t bank, String dirName) {
  Dir dir = FatFS.openDir(dirName);
  patternbank[bank].numfiles=0;
  while (true) {
    if (!dir.next()) {
      // no more files
      break;
    }
   // Serial.print(dir.fileName());
    File MIDIfile;
    if(dir.fileSize()) {
      MIDIfile= dir.openFile("r");
     // Serial.print(MIDIfile.size());
     // Serial.print(" ");
      patternbank[bank].buffer[patternbank[bank].numfiles]=0; // null pointer means no file content    
      uint32_t index=0;

      if (patternbank[bank].buffer[patternbank[bank].numfiles]=(uint8_t *)malloc(MIDIfile.size()*sizeof(uint8_t))) { // allocate RAM to store the file
        while (MIDIfile.available()) patternbank[bank].buffer[patternbank[bank].numfiles][index++]=MIDIfile.read();
      //  Serial.printf(" Read %d bytes\n",index);
        patternbank[bank].buffersize[patternbank[bank].numfiles]=dir.fileSize();
        ++patternbank[bank].numfiles;
      }
      else {
        Serial.printf("memory allocation error\n");
        return -1; // error
      }
    }
    else {
      Serial.printf("error reading file %s\n",dir.fileName());
      return -1; // error
    }
  } 
  return patternbank[bank].numfiles;
}

void printDirectory(String dirName, int numTabs) {
  Dir dir = FatFS.openDir(dirName);

  while (true) {

    if (!dir.next()) {
      // no more files
      break;
    }
    for (int i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(dir.fileName());
    if (dir.isDirectory()) {
      Serial.println("/");
      printDirectory(dirName + "/" + dir.fileName(), numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(dir.fileSize(), DEC);
      time_t cr = dir.fileCreationTime();
      struct tm tmstruct;
      localtime_r(&cr, &tmstruct);
      Serial.printf("\t%d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
    }
  }
}

// midi parser stuff

uint8_t instrument[NUM_VOICES]; // maps MIDI note # to a voice
int8_t instrumentcount=0;

struct midi_parser parser;
enum midi_parser_status status;

void resetmidiparser(uint8_t * buffer, int32_t size) {
  parser.state = MIDI_PARSER_INIT;
  parser.size  = size;
  parser.in    = (const uint8_t*)buffer;
}

// Comparison function for instruments (uint_8) 
int uint8_compare(const void *a, const void *b) {
    uint8_t uint_a = *((uint8_t*)a);
    uint8_t uint_b = *((uint8_t*)b);

    if (uint_a < uint_b) return -1;
    if (uint_a > uint_b) return 1;
    return 0;
}

void scaninstruments(uint8_t * buffer, int32_t size) {
  bool voiceassigned;
  enum midi_parser_status status;
  resetmidiparser(buffer,size);
  for(int8_t i=0; i< NUM_VOICES; ++i) instrument[i]=255; // clear instrument assignments
  instrumentcount=totalnotes=totaltime=0;    
  while (1) {  // scan the MIDI file to find GM instruments used
    status = midi_parse(&parser);
    if (status==MIDI_PARSER_EOB) break;
    if (status==MIDI_PARSER_TRACK_MIDI) totaltime+=parser.vtime; // add up all the time ticks in the track
    if ((status==MIDI_PARSER_TRACK_MIDI) && (parser.midi.status==9)) ++totalnotes; // keep track of events for synchronization
    if ((status==MIDI_PARSER_TRACK_MIDI) && (parser.midi.status==9) && (parser.midi.param1 > 23) && (parser.midi.param1 <84)) { // note on event - filter out any trash notes
      voiceassigned=0;
      int16_t i;
      for (i=0; i< NUM_VOICES;++i) { // check if this instrument is assigned
      //Serial.printf("index %d param1 % instrument %d\n",i,parser.midi.param1,instrument[i]);
        if (parser.midi.param1==instrument[i]) { // 
          voiceassigned=1;
          break;
        }
        if (instrument[i]==255) break;
      }
      if (!voiceassigned && (instrumentcount < NUM_VOICES)) {
        instrument[i]=parser.midi.param1; // assign the instrument 
        ++instrumentcount;
      }      
    }
  }
  qsort(instrument, sizeof(instrument)/sizeof(uint8_t), sizeof(uint8_t), uint8_compare); // sort instrument assignments into ascending order
}

// reset the parser, scan the MIDI file for the new instrument set, and reset again so its ready to start from the beginning
// do this every time bank or pattern is changed
void resetandscan(uint8_t * buffer, int32_t size) {
  resetmidiparser(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // set the parser to the first bank and file
  scaninstruments(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // find instruments used in the MIDI file
  resetmidiparser(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // reset the parser again
}

// error condition - loops forever
void fatalerror(const char * errorstring){
  Serial.printf("%s\n",errorstring);
  while (1) {
    LEDS.setPixelColor(0, RED);  // flashing red
    LEDS.show();
    delay(75);
    LEDS.setPixelColor(0, 0);  // 
    LEDS.show();
    delay(75);
  } 
}

// map samples to the tracks in the current MIDI file using their MIDI note number
void mapGMsamples (void){
  bool found;
  for (int8_t i=0; i<instrumentcount;++i) {
    found=0;
    for (int8_t j=0; j< NUM_SAMPLES;++j) {
      if (instrument[i]==sample[j].MIDINOTE) {
        voice[i].sample=j;
        found=1;
        break;
      }
    }
    if (!found) voice[i].sample=0; // if we can't find a sample with the same GM note# use sample 0 which should be silence
  }
}

void setup() { 
  Serial.begin(115200);
//  while(!Serial) ;

// set up I/O pins
 
   pinMode(10,OUTPUT); // hi = CPU busy
     pinMode(11,OUTPUT); // hi = CPU busy
#ifdef MONITOR_CPU1 // for monitoring 2nd core CPU usage
  pinMode(CPU_USE,OUTPUT); // hi = CPU busy
#endif 

  pinMode(CLOCKIN,INPUT); // gate/trigger in used for clock
  pinMode(RESETIN,INPUT); // 2nd jack in used for reset
  pinMode(BUTTON1,INPUT_PULLUP); // button in
  pinMode(MUXCTL,OUTPUT);  // analog switch mux

  LEDS.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  analogReadResolution(AD_BITS); // set up for max resolution
// initialize the pot readings
  for (int16_t i=0; i<NUMPOTS;++i) {
    pot[i]=0;
    potlock[i]=0;
  }

// set up Pico I2S for PT8211 stereo DAC
	DAC.setBCLK(BCLK);
	DAC.setDATA(I2S_DATA);
	DAC.setBitsPerSample(16);
	DAC.setBuffers(1, 128, 0); // DMA buffer - 32 bit L/R words
	DAC.setLSBJFormat();  // needed for PT8211 which has funny timing
	DAC.begin(SAMPLERATE);

  alarm_in_us(BUTTON_TIMER_MICROS); // start the button IRQ

  if (!FatFS.begin())  Serial.print("Can't mount FS"); // start up filesystem
  else Serial.print("mounted FS");

  if(!digitalRead(BUTTON1)) { // button held on powerup, start in USB mode so files can be managed from host
    FatFSUSB.begin(); 
    LEDS.setPixelColor(0, VIOLET);  
    LEDS.show();
    delay(2000); // TinyUSB seems to have a race condition, see https://github.com/hathach/tinyusb/discussions/1764
    Serial.println("FatFSUSB started.");
    Serial.println("Connect drive via USB to upload/erase files");
    while(1); // loop forever - USB files handled in background 
  }  
  else {
    LEDS.setPixelColor(0, RED); 
    LEDS.show();
    printDirectory("/", 0); // do a directory dump
  }
  // read MIDI files from flash file system
  currentbank=currentfile=0;
  if (readbank(0, "/bank1") < 0) fatalerror("can't read bank 1");
  if (readbank(1, "/bank2") < 0) fatalerror("can't read bank 2");
  if (readbank(2, "/bank3") < 0) fatalerror("can't read bank 3");
  if (readbank(3, "/bank4") < 0) fatalerror("can't read bank 4");

  resetandscan(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // parse the file to find what instruments are used

  clocktimer=millis(); // initial clock measurement
}



void loop() {

  ClickButton::Button b=button1.getButton();
    switch (b) {
    case ClickButton::Clicked:
      ++UIstate;
      if (UIstate >= NUMUISTATES) UIstate=SET1;
      lockpots();
      break;
    case ClickButton::DoubleClicked:
      mapGMsamples();
      lockpots();
      break;
    case ClickButton::Held:  // hold to reset sequencer
      resetmidiparser(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // reset the parser
      playing=loopsynced=0; // stop playing, sync to clock
      nextMIDIevent=micros();  // fake event to start the parser loop again
      break;
    default:
      break;
  }

  samplepots();

// set parameters from panel pots
// 
  switch (UIstate) {
    case SET1:
      LEDS.setPixelColor(0, RED); 
      if (!potlock[0]) {
        currentbank=map(pot[0],0,AD_RANGE-1,0,NUM_BANKS); // top pot on the panel
        if (lastbank!=currentbank) { // if this is a new bank reset the parser
          resetandscan(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // parse the new file to find what instruments are used
          lastbank=currentbank;
          mapGMsamples();
          banktimer=millis(); // parameter is still changing so delay locking
        }
        if ((millis()-banktimer) > POTAUTOLOCK) potlock[0]=1; // its been stable for a while so lock it
      }
      if (!potlock[1]) {
        currentfile=map(pot[1],0,AD_RANGE-1,0,patternbank[currentbank].numfiles);  // 
        if (lastfile!=currentfile) { // if this is a new file reset the parser
          resetandscan(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // parse the new file to find what instruments are used
          lastfile=currentfile;
          mapGMsamples();
          filetimer=millis(); // parameter is still changing so delay locking
        }
        if ((millis()-filetimer) > POTAUTOLOCK) potlock[1]=1; // its been stable for a while so lock it
      }
      if (!potlock[2]) voice[0].sample=map(pot[2],0,AD_RANGE-1,0,NUM_SAMPLES-1); //
      if (!potlock[3]) voice[1].sample=map(pot[3],0,AD_RANGE-1,0,NUM_SAMPLES-1);  // 
      break;
    case SET2:
      LEDS.setPixelColor(0, GREEN);
      if (!potlock[0]) voice[2].sample=map(pot[0],0,AD_RANGE-1,0,NUM_SAMPLES-1); // top pot on the panel
      if (!potlock[1]) voice[3].sample=map(pot[1],0,AD_RANGE-1,0,NUM_SAMPLES-1);  // 
      if (!potlock[2]) voice[4].sample=map(pot[2],0,AD_RANGE-1,0,NUM_SAMPLES-1);
      if (!potlock[3]) voice[5].sample=map(pot[3],0,AD_RANGE-1,0,NUM_SAMPLES-1); 
      break;
    case SET3:
      LEDS.setPixelColor(0, AQUA);
      if (!potlock[0]) voice[6].sample=map(pot[0],0,AD_RANGE-1,0,NUM_SAMPLES-1); // top pot on the panel
      if (!potlock[1]) voice[7].sample=map(pot[1],0,AD_RANGE-1,0,NUM_SAMPLES-1);  // 
      if (!potlock[2]) voice[8].sample=map(pot[2],0,AD_RANGE-1,0,NUM_SAMPLES-1);
      if (!potlock[3]) voice[9].sample=map(pot[3],0,AD_RANGE-1,0,NUM_SAMPLES-1); 
      break;
    default:
      break;
  }

/*
// edge triggered reset
  if (!digitalRead(RESETIN))  { // look for reset - reset input is inverted
    if (((millis()-resetdebouncetimer) > CLOCK_DEBOUNCE) && !resetedge) {  // true on rising edge
      resetedge=1;
      resetmidiparser(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // reset the parser
      playing=loopsynced=0; // stop playing, sync to clock
      nextMIDIevent=micros();  // fake event to start the parser loop again
    }
  }
  else {
    resetedge=0;
    resetdebouncetimer=millis();
  }
*/

// reset when input is high
  if (!digitalRead(RESETIN))  { // look for reset - reset input is inverted
    resetmidiparser(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // reset the parser
    playing=loopsynced=clocked=0; // stop playing, sync to clock
    nextMIDIevent=micros();  // fake event to start the parser loop again
    clkdelay=CLKDELAYCOUNT;
  }

// note that the clock doesn't trigger MIDI file events directly - it provides a timebase in 1/16th notes
// also note that the clock is not debounced. my Performer only outputs a 2ms clock pulse and debouncing caused missed clocks at higher BPMs
  if (!digitalRead(CLOCKIN)) {  // look for rising edge of clock input which is inverted
    if (!clocked) {
      LEDS.setPixelColor(0,0 ); // show clock as a LED off flash
      LEDS.show();  // update LED
      ledtimer=millis(); // start led off timer
      edgetime=micros();
      if (clkdelay>0) --clkdelay; // delay clock measurement after clock is stopped to avoid bad measurments
      // ideally clock period should be averaged over time but the measurement is +- a ms or so which is acceptable
      if (((edgetime - lastclock) < 1000000) && (clkdelay==0)) clockperiod=edgetime-lastclock; // handles case of stopped clock which would otherwise result in a very long clockperiod. 1000000 us ~ 15 BPM minimum
      lastclock=edgetime; 
      // if (!loopsynced) Serial.printf("sync %u\n",micros());
      clocked=playing=loopsynced=1;
    }
  }
  else {   
      clocked=0;
  }

  if (((micros()-edgetime) > (clockperiod*3)/2) && playing) { // check that clock is running
    playing=0; // clock not running, stop playing
    eventtimeleft=nextMIDIevent-micros();  // capture how much time is left till next event
  }

  if (!playing) nextMIDIevent=micros()+eventtimeleft;   // clock is stopped so reschedule the next midi event 
 
  bool voiceassigned;  // compiler complains if this in the case statement
  int16_t sixteenths,bars;

  if (playing && loopsynced && (micros() >= nextMIDIevent)) {

    status = midi_parse(&parser);
    switch (status) {
      case MIDI_PARSER_EOB:  // should never get here - sync logic resets on last note of file
        resetmidiparser(patternbank[currentbank].buffer[currentfile],patternbank[currentbank].buffersize[currentfile]); // reset the parser
        playing=loopsynced=clocked=0; // stop playing, sync to clock
        nextMIDIevent=micros();  // fake event to start the parser loop again
     //   Serial.printf("EOF  %u\n",micros());
        break;
      case MIDI_PARSER_ERROR:   
        break;
      case MIDI_PARSER_INIT:     
        break;
      case MIDI_PARSER_HEADER:
        ppqn=parser.header.time_division;
        notes=accumtime=0;   // initialize the note counter and accumulated time
        sixteenths=totaltime*4/ppqn;  // ppqn/4 is the time for a sixteenth note- inverted for math precision
        bars=sixteenths/16;
        if ((sixteenths%16) !=0) {
          bars++; // make the loop at least one bar and make it an integer number of bars - some MIDI files are not set up like this
          looptime=bars*16*ppqn/4; // if loop is not a multiple of 16 clocks make it so
        }
        else looptime=totaltime;
     //   Serial.printf("totaltime=%u looptime=%u clocks=%u  bars=%u  newbars=%u  ppqn=%d \n", totaltime, looptime,sixteenths,sixteenths/16,bars,ppqn );
        break;
      case MIDI_PARSER_TRACK:
        break;
      case MIDI_PARSER_TRACK_MIDI:
        // the MIDI file contains a pulse per quarter note value (ppqn). incoming clock is at 1/16 note rate
        nextMIDIevent=parser.vtime*clockperiod*4/ppqn+micros();  // ppqn/4 is the time for a sixteenth note- inverted for math precision
        if (notes==totalnotes) nextMIDIevent=micros(); // skips any note offs that come after the last note on

        if (parser.midi.status==9) { // note on event so find the voice thats assigned to this instrument
          for (int16_t i=0; i< NUM_VOICES;++i) { // check if this GM percussion instrument is assigned
            if (parser.midi.param1==instrument[i]) {
              rp2040.fifo.push(((0x90 | i)<<24) | (parser.midi.param2 <<16));  // tell other core to play this voice  
              break;
            }
          }
          ++notes; 
          if (notes==totalnotes) { // this is the last note on, we need to resync to clock at end of the file
            nextMIDIevent=micros()+(looptime-accumtime)*clockperiod*4/ppqn-20000; // schedule last event(s) so we have time to handle EOF and sync to next clock
          }
        }
        accumtime+=parser.vtime;
        break;

      case MIDI_PARSER_TRACK_META:
        break;
      case MIDI_PARSER_TRACK_SYSEX:
        break;
      default:
        Serial.printf("unhandled state: %d\n", status);
    } // end case
  }

 if ((millis()-ledtimer) > LEDOFF ) LEDS.show();  // update LEDs only if not doing off flash  
} // end loop()

// second core setup
// second core is dedicated to sample processing
void setup1() {
delay (1000); // wait for main core to start up peripherals
}

// process audio samples
void loop1(){
  int32_t samplesum=0;
  int32_t command;
  uint8_t message,velocity,track;

#ifdef MONITOR_CPU1  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif

// get note on messages from core1 thru the FIFO. 
// if both cores write the same variable (sample[].sampleindex in this case) it will mess up now and then so communicate MIDI message with the interprocessor FIFO
// we don't care about note offs - just let the sample play thru
  while (rp2040.fifo.available()) { // get MIDI command, channel# = voice#
    command=rp2040.fifo.pop(); //
    track=(command>>24) & 0xf; 
    message= (command>>24) & 0xf0;  
    velocity=(command >>16) & 0x7f;

    switch (message) {
      case 0x90: // note on
        sample[voice[track].sample].play_volume=velocity; // set velocity
        sample[voice[track].sample].sampleindex=0; // trigger sample for this track
        break;
      case 0x80: // note off
        break;
      default:
        break;
    }       
  }

  for (int i=0; i< NUM_SAMPLES;++i) {  // look for samples that are playing, scale their volume, and add them up
    if (sample[i].sampleindex < sample[i].samplesize) samplesum+=(int32_t)(sample[i].samplearray[sample[i].sampleindex++]*sample[i].play_volume);  // thats a mouthful!
  }
  samplesum=samplesum>>7;  // adjust for play_volume multiply above
  if  (samplesum>32767) samplesum=32767; // clip if sample sum is too large
  if  (samplesum<-32767) samplesum=-32767;

 // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
	DAC.write(int16_t(samplesum)); // left
	DAC.write(int16_t(samplesum)); // right

#ifdef MONITOR_CPU1
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}





