//#include <Audio.h>
#include <pico-audio.h>
#include "synth_braids.h"
//#include <Metro.h>

#define BCLK 12
#define WS 13  // this will always be 1 pin above BCLK - can't change it
#define I2S_DATA 14  // Out of Pico to DAC

#define AUDIO_STATS    // shows audio library CPU utilization etc on serial console

AudioSynthBraids         br;
AudioOutputI2S           i2s1;
//AudioControlSGTL5000     sgtl5000_1;
AudioConnection          patchCord1(br, 0, i2s1, 0);
AudioConnection          patchCord2(br, 0, i2s1, 1);

// various timers
//Metro five_sec=Metro(5000); // Set up a 5 second Metro

void setup()
{
  AudioMemory(32);
  i2s1.begin(BCLK,WS,I2S_DATA);

/*
  sgtl5000_1.enable();
  sgtl5000_1.lineOutLevel(29);
  sgtl5000_1.dacVolumeRamp();
  sgtl5000_1.dacVolume(1.0);
  sgtl5000_1.unmuteHeadphone();
  sgtl5000_1.unmuteLineout();
  sgtl5000_1.volume(0.2, 0.2); // Headphone volume
  */
}

int shape =0;


void loop()
{
  uint8_t x=random(11);
  int8_t d=random(60)-30;
  uint8_t p=random(7);

  br.set_braids_pitch(440<<4);
  Serial.printf("%d %s ",shape,br.get_name(shape));
  br.set_braids_shape(shape);
  shape++;
  if (shape ==43) shape=0;

  br.set_braids_timbre(random(30000));
  br.set_braids_color(random(30000));
  delay(2000);

      Serial.print(" Proc = ");
      Serial.print(AudioProcessorUsage());
      Serial.print(" (");    
      Serial.print(AudioProcessorUsageMax());
      Serial.print("),  Mem = ");
      Serial.print(AudioMemoryUsage());
      Serial.print(" (");    
      Serial.print(AudioMemoryUsageMax());
      Serial.println(")");


/* 
  
  Serial.println("Key-Down");
  
  braids.noteOn(48+x, 90+d);
  delay(100);
  braids.noteOn(52+x, 90+d);
  delay(100);
  braids.noteOn(55+x, 90+d);
  delay(100);
  braids.noteOn(60+x, 90+d);
  delay(2000);

  Serial.println("Key-Up");
 braids.noteOff(48+x);
  braids.noteOff(52+x);
  braids.noteOff(55+x);
  braids.noteOff(60+x);
  delay(2000);
*/

}
