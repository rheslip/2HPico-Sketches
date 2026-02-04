// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	_clave01,	// pointer to sample array
	_clave01_SIZE,	// size of the sample array
	_clave01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_clave01",	// sample name

	_clave02,	// pointer to sample array
	_clave02_SIZE,	// size of the sample array
	_clave02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_clave02",	// sample name

	_clave03,	// pointer to sample array
	_clave03_SIZE,	// size of the sample array
	_clave03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_clave03",	// sample name

	_conga01,	// pointer to sample array
	_conga01_SIZE,	// size of the sample array
	_conga01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_conga01",	// sample name

	_conga02,	// pointer to sample array
	_conga02_SIZE,	// size of the sample array
	_conga02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_conga02",	// sample name

	_conga03,	// pointer to sample array
	_conga03_SIZE,	// size of the sample array
	_conga03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_conga03",	// sample name

	_conga04,	// pointer to sample array
	_conga04_SIZE,	// size of the sample array
	_conga04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_conga04",	// sample name

	_cowbell,	// pointer to sample array
	_cowbell_SIZE,	// size of the sample array
	_cowbell_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_cowbell",	// sample name

	_rim01,	// pointer to sample array
	_rim01_SIZE,	// size of the sample array
	_rim01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_rim01",	// sample name

	_rim02,	// pointer to sample array
	_rim02_SIZE,	// size of the sample array
	_rim02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_rim02",	// sample name

	_rim03,	// pointer to sample array
	_rim03_SIZE,	// size of the sample array
	_rim03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_rim03",	// sample name

	_rim04,	// pointer to sample array
	_rim04_SIZE,	// size of the sample array
	_rim04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"_rim04",	// sample name

	Clap01,	// pointer to sample array
	Clap01_SIZE,	// size of the sample array
	Clap01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap01",	// sample name

	Clap02,	// pointer to sample array
	Clap02_SIZE,	// size of the sample array
	Clap02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap02",	// sample name

	Clap03,	// pointer to sample array
	Clap03_SIZE,	// size of the sample array
	Clap03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap03",	// sample name

	Closedhat01,	// pointer to sample array
	Closedhat01_SIZE,	// size of the sample array
	Closedhat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat01",	// sample name

	Closedhat02,	// pointer to sample array
	Closedhat02_SIZE,	// size of the sample array
	Closedhat02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat02",	// sample name

	Closedhat03,	// pointer to sample array
	Closedhat03_SIZE,	// size of the sample array
	Closedhat03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat03",	// sample name

	Closedhat04,	// pointer to sample array
	Closedhat04_SIZE,	// size of the sample array
	Closedhat04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat04",	// sample name

	Closedhat05,	// pointer to sample array
	Closedhat05_SIZE,	// size of the sample array
	Closedhat05_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat05",	// sample name

	Closedhat06,	// pointer to sample array
	Closedhat06_SIZE,	// size of the sample array
	Closedhat06_SIZE,	//sampleindex. if at end of sample array sound is not playing
	55,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat06",	// sample name

	Kick01,	// pointer to sample array
	Kick01_SIZE,	// size of the sample array
	Kick01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	56,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick01",	// sample name

	Kick02,	// pointer to sample array
	Kick02_SIZE,	// size of the sample array
	Kick02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	57,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick02",	// sample name

	Kick03,	// pointer to sample array
	Kick03_SIZE,	// size of the sample array
	Kick03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	58,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick03",	// sample name

	Kick04,	// pointer to sample array
	Kick04_SIZE,	// size of the sample array
	Kick04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	59,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick04",	// sample name

	Kick05,	// pointer to sample array
	Kick05_SIZE,	// size of the sample array
	Kick05_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick05",	// sample name

	Kick06,	// pointer to sample array
	Kick06_SIZE,	// size of the sample array
	Kick06_SIZE,	//sampleindex. if at end of sample array sound is not playing
	61,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick06",	// sample name

	Kick07,	// pointer to sample array
	Kick07_SIZE,	// size of the sample array
	Kick07_SIZE,	//sampleindex. if at end of sample array sound is not playing
	62,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick07",	// sample name

	Kick08,	// pointer to sample array
	Kick08_SIZE,	// size of the sample array
	Kick08_SIZE,	//sampleindex. if at end of sample array sound is not playing
	63,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick08",	// sample name

	Kick09,	// pointer to sample array
	Kick09_SIZE,	// size of the sample array
	Kick09_SIZE,	//sampleindex. if at end of sample array sound is not playing
	64,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick09",	// sample name

	Kick10,	// pointer to sample array
	Kick10_SIZE,	// size of the sample array
	Kick10_SIZE,	//sampleindex. if at end of sample array sound is not playing
	65,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick10",	// sample name

	Kick11,	// pointer to sample array
	Kick11_SIZE,	// size of the sample array
	Kick11_SIZE,	//sampleindex. if at end of sample array sound is not playing
	66,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick11",	// sample name

	Kick12,	// pointer to sample array
	Kick12_SIZE,	// size of the sample array
	Kick12_SIZE,	//sampleindex. if at end of sample array sound is not playing
	67,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick12",	// sample name

	Kick13,	// pointer to sample array
	Kick13_SIZE,	// size of the sample array
	Kick13_SIZE,	//sampleindex. if at end of sample array sound is not playing
	68,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick13",	// sample name

	Kick14,	// pointer to sample array
	Kick14_SIZE,	// size of the sample array
	Kick14_SIZE,	//sampleindex. if at end of sample array sound is not playing
	69,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick14",	// sample name

	Kick15,	// pointer to sample array
	Kick15_SIZE,	// size of the sample array
	Kick15_SIZE,	//sampleindex. if at end of sample array sound is not playing
	70,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick15",	// sample name

	Openhat01,	// pointer to sample array
	Openhat01_SIZE,	// size of the sample array
	Openhat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	71,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Openhat01",	// sample name

	Openhat02,	// pointer to sample array
	Openhat02_SIZE,	// size of the sample array
	Openhat02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	72,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Openhat02",	// sample name

	Openhat03,	// pointer to sample array
	Openhat03_SIZE,	// size of the sample array
	Openhat03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	73,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Openhat03",	// sample name

	Rev_hat01,	// pointer to sample array
	Rev_hat01_SIZE,	// size of the sample array
	Rev_hat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	74,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rev_hat01",	// sample name

	Rev_hat02,	// pointer to sample array
	Rev_hat02_SIZE,	// size of the sample array
	Rev_hat02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	75,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rev_hat02",	// sample name

	Shaker01,	// pointer to sample array
	Shaker01_SIZE,	// size of the sample array
	Shaker01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	76,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Shaker01",	// sample name

	Snare01,	// pointer to sample array
	Snare01_SIZE,	// size of the sample array
	Snare01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	77,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare01",	// sample name

	Snare02,	// pointer to sample array
	Snare02_SIZE,	// size of the sample array
	Snare02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	78,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare02",	// sample name

	Snare03,	// pointer to sample array
	Snare03_SIZE,	// size of the sample array
	Snare03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	79,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare03",	// sample name

	Snare04,	// pointer to sample array
	Snare04_SIZE,	// size of the sample array
	Snare04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	80,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare04",	// sample name

	Snare05,	// pointer to sample array
	Snare05_SIZE,	// size of the sample array
	Snare05_SIZE,	//sampleindex. if at end of sample array sound is not playing
	81,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare05",	// sample name

	Snare06,	// pointer to sample array
	Snare06_SIZE,	// size of the sample array
	Snare06_SIZE,	//sampleindex. if at end of sample array sound is not playing
	82,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare06",	// sample name

	Snare07,	// pointer to sample array
	Snare07_SIZE,	// size of the sample array
	Snare07_SIZE,	//sampleindex. if at end of sample array sound is not playing
	83,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare07",	// sample name

	Snare08,	// pointer to sample array
	Snare08_SIZE,	// size of the sample array
	Snare08_SIZE,	//sampleindex. if at end of sample array sound is not playing
	84,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare08",	// sample name

	Snare09,	// pointer to sample array
	Snare09_SIZE,	// size of the sample array
	Snare09_SIZE,	//sampleindex. if at end of sample array sound is not playing
	85,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare09",	// sample name

	Snare10,	// pointer to sample array
	Snare10_SIZE,	// size of the sample array
	Snare10_SIZE,	//sampleindex. if at end of sample array sound is not playing
	86,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare10",	// sample name

	Snare11,	// pointer to sample array
	Snare11_SIZE,	// size of the sample array
	Snare11_SIZE,	//sampleindex. if at end of sample array sound is not playing
	87,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare11",	// sample name

	Snare12,	// pointer to sample array
	Snare12_SIZE,	// size of the sample array
	Snare12_SIZE,	//sampleindex. if at end of sample array sound is not playing
	88,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snare12",	// sample name

	Tom01,	// pointer to sample array
	Tom01_SIZE,	// size of the sample array
	Tom01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	89,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom01",	// sample name

	Tom02,	// pointer to sample array
	Tom02_SIZE,	// size of the sample array
	Tom02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	90,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom02",	// sample name

	Tom03,	// pointer to sample array
	Tom03_SIZE,	// size of the sample array
	Tom03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	91,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom03",	// sample name

	Tom04,	// pointer to sample array
	Tom04_SIZE,	// size of the sample array
	Tom04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	92,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom04",	// sample name

};
