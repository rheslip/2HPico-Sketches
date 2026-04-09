// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Silence,	// pointer to sample array
	Silence_SIZE,	// size of the sample array
	Silence_SIZE,	//sampleindex. if at end of sample array sound is not playing
	0,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Silence",	// sample name

	Sub_kick_103,	// pointer to sample array
	Sub_kick_103_SIZE,	// size of the sample array
	Sub_kick_103_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sub_kick_103",	// sample name

	Sub_kick_104,	// pointer to sample array
	Sub_kick_104_SIZE,	// size of the sample array
	Sub_kick_104_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sub_kick_104",	// sample name

	Sub_kick_105,	// pointer to sample array
	Sub_kick_105_SIZE,	// size of the sample array
	Sub_kick_105_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sub_kick_105",	// sample name

	Sub_kick_106,	// pointer to sample array
	Sub_kick_106_SIZE,	// size of the sample array
	Sub_kick_106_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sub_kick_106",	// sample name

	Sub_kick_107,	// pointer to sample array
	Sub_kick_107_SIZE,	// size of the sample array
	Sub_kick_107_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sub_kick_107",	// sample name

	Sub_kick_11,	// pointer to sample array
	Sub_kick_11_SIZE,	// size of the sample array
	Sub_kick_11_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sub_kick_11",	// sample name

	Rimshot_091,	// pointer to sample array
	Rimshot_091_SIZE,	// size of the sample array
	Rimshot_091_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rimshot_091",	// sample name

	Rimshot_092,	// pointer to sample array
	Rimshot_092_SIZE,	// size of the sample array
	Rimshot_092_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rimshot_092",	// sample name

	Snr031,	// pointer to sample array
	Snr031_SIZE,	// size of the sample array
	Snr031_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snr031",	// sample name

	Snr032,	// pointer to sample array
	Snr032_SIZE,	// size of the sample array
	Snr032_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snr032",	// sample name

	Snr033,	// pointer to sample array
	Snr033_SIZE,	// size of the sample array
	Snr033_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snr033",	// sample name

	Clap_095,	// pointer to sample array
	Clap_095_SIZE,	// size of the sample array
	Clap_095_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_095",	// sample name

	Clap_096,	// pointer to sample array
	Clap_096_SIZE,	// size of the sample array
	Clap_096_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_096",	// sample name

	Clap_097,	// pointer to sample array
	Clap_097_SIZE,	// size of the sample array
	Clap_097_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_097",	// sample name

	Clap_098,	// pointer to sample array
	Clap_098_SIZE,	// size of the sample array
	Clap_098_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_098",	// sample name

	Clap_099,	// pointer to sample array
	Clap_099_SIZE,	// size of the sample array
	Clap_099_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_099",	// sample name

	Snares_154,	// pointer to sample array
	Snares_154_SIZE,	// size of the sample array
	Snares_154_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snares_154",	// sample name

	Snares_155,	// pointer to sample array
	Snares_155_SIZE,	// size of the sample array
	Snares_155_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snares_155",	// sample name

	Snares_157,	// pointer to sample array
	Snares_157_SIZE,	// size of the sample array
	Snares_157_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snares_157",	// sample name

	Closed_hat_093,	// pointer to sample array
	Closed_hat_093_SIZE,	// size of the sample array
	Closed_hat_093_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closed_hat_093",	// sample name

	Closed_hat_094,	// pointer to sample array
	Closed_hat_094_SIZE,	// size of the sample array
	Closed_hat_094_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closed_hat_094",	// sample name

	Closed_hat_095,	// pointer to sample array
	Closed_hat_095_SIZE,	// size of the sample array
	Closed_hat_095_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closed_hat_095",	// sample name

	Tom_130,	// pointer to sample array
	Tom_130_SIZE,	// size of the sample array
	Tom_130_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_130",	// sample name

	Tom_131,	// pointer to sample array
	Tom_131_SIZE,	// size of the sample array
	Tom_131_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_131",	// sample name

	Tom_132,	// pointer to sample array
	Tom_132_SIZE,	// size of the sample array
	Tom_132_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_132",	// sample name

	Tom_133,	// pointer to sample array
	Tom_133_SIZE,	// size of the sample array
	Tom_133_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_133",	// sample name

	Tom_134,	// pointer to sample array
	Tom_134_SIZE,	// size of the sample array
	Tom_134_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_134",	// sample name

	Tom_135,	// pointer to sample array
	Tom_135_SIZE,	// size of the sample array
	Tom_135_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_135",	// sample name

	Tom_136,	// pointer to sample array
	Tom_136_SIZE,	// size of the sample array
	Tom_136_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom_136",	// sample name

	Open_hat_091,	// pointer to sample array
	Open_hat_091_SIZE,	// size of the sample array
	Open_hat_091_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Open_hat_091",	// sample name

	Open_hat_092,	// pointer to sample array
	Open_hat_092_SIZE,	// size of the sample array
	Open_hat_092_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Open_hat_092",	// sample name

	Open_hat_093,	// pointer to sample array
	Open_hat_093_SIZE,	// size of the sample array
	Open_hat_093_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Open_hat_093",	// sample name

	Crash_061,	// pointer to sample array
	Crash_061_SIZE,	// size of the sample array
	Crash_061_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Crash_061",	// sample name

	Crash_062,	// pointer to sample array
	Crash_062_SIZE,	// size of the sample array
	Crash_062_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Crash_062",	// sample name

	Crash_063,	// pointer to sample array
	Crash_063_SIZE,	// size of the sample array
	Crash_063_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Crash_063",	// sample name

	Ride_081,	// pointer to sample array
	Ride_081_SIZE,	// size of the sample array
	Ride_081_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ride_081",	// sample name

	Ride_082,	// pointer to sample array
	Ride_082_SIZE,	// size of the sample array
	Ride_082_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ride_082",	// sample name

	Percussion10,	// pointer to sample array
	Percussion10_SIZE,	// size of the sample array
	Percussion10_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Percussion10",	// sample name

	Percussion11,	// pointer to sample array
	Percussion11_SIZE,	// size of the sample array
	Percussion11_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Percussion11",	// sample name

	Percussion6,	// pointer to sample array
	Percussion6_SIZE,	// size of the sample array
	Percussion6_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Percussion6",	// sample name

	Percussion7,	// pointer to sample array
	Percussion7_SIZE,	// size of the sample array
	Percussion7_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Percussion7",	// sample name

	Percussion8,	// pointer to sample array
	Percussion8_SIZE,	// size of the sample array
	Percussion8_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Percussion8",	// sample name

	Percussion9,	// pointer to sample array
	Percussion9_SIZE,	// size of the sample array
	Percussion9_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Percussion9",	// sample name

};
