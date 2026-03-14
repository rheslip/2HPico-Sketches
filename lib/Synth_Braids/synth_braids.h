#ifndef SYNTH_BRAIDS_H_
#define SYNTH_BRAIDS_H_

#include "AudioStream.h"
#include "macro_oscillator.h"


using namespace braids;

class AudioSynthBraids: public AudioStream
{
public:
        AudioSynthBraids(): AudioStream(0, NULL), kAudioBlockSize(AUDIO_BLOCK_SAMPLES), magnitude(65536.0) { }
        ~AudioSynthBraids() { }

        void set_braids_shape(int16_t shape) {

          // Sets the shape
          MacroOscillatorShape osc_shape = static_cast<MacroOscillatorShape>(shape);
          osc.set_shape(osc_shape);
      	}

        void set_braids_color(int16_t colorbraids) {
      		color = colorbraids;
          osc.set_parameters(timbre,color);
      	}

        void set_braids_timbre(int16_t timbrebraids) {
      		timbre = timbrebraids;
          osc.set_parameters(timbre,color);
      	}

        void set_braids_pitch(int16_t pitchbraids) {
          pitch = pitchbraids;
          if(pre_pitch!=pitch){
    			osc.set_pitch(pitch);
    			pre_pitch = pitch;
    		}
			osc.Strike();
      	}
    const char* get_name(uint8_t n)
       {
         return (settings.metadata(SETTING_OSCILLATOR_SHAPE).strings[n]);
       }
        inline void init_braids(){
            // Initializes the objects
            osc.Init();
            osc.set_shape(MACRO_OSC_SHAPE_CSAW);
            osc.set_parameters(0, 0);

            pitch = 32 << 7;
        }
        virtual void update(void);

private:
        MacroOscillator osc;

        const uint16_t kAudioBlockSize;
        // Globals that define the parameters of the oscillator
        volatile int16_t pitch,pre_pitch;
        volatile int16_t timbre;
        volatile int16_t color;
        volatile int16_t shapebraids;

      	int16_t buffer[AUDIO_BLOCK_SAMPLES] = { 0 };

        volatile int32_t magnitude;
};

#endif
