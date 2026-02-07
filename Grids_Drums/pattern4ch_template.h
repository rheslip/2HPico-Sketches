// Copyright 2012 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Resources definitions.
//
// Automatically generated with:
// make resources

// R Heslip Feb 2026 - added a fourth pattern to each node for my 2HPico drum machine which can do 4 tracks
// this is a template to use with the Python MIDI to drum pattern script
// cut the end of the scripts output (between "{" and "};") and paste into the node slots below
// there MUST be 25 nodes, 4 patterns per node and patterns MUST have a length of 32
// the script will always output a length of 32 per pattern but there could be more or less than 4 patterns depending on how many drums the MIDI file uses
// edit till the above requrements are met!!!

#include <stdint.h>

#ifndef GRIDS_RESOURCES_H_
#define GRIDS_RESOURCES_H_

namespace grids {

const uint8_t node_0[] = 

const uint8_t node_1[] = 

const uint8_t node_2[] = 

const uint8_t node_3[] = 

const uint8_t node_4[] = 

const uint8_t node_5[] = 

const uint8_t node_6[] = 

const uint8_t node_7[] = 

const uint8_t node_8[] = 

const uint8_t node_9[] = 

const uint8_t node_10[] = 

const uint8_t node_11[] = 

const uint8_t node_12[] = 

const uint8_t node_13[] = 

const uint8_t node_14[] = 

const uint8_t node_15[] =
 
const uint8_t node_16[] = 

const uint8_t node_17[] =

const uint8_t node_18[] = 

const uint8_t node_19[] = 

const uint8_t node_20[] = 

const uint8_t node_21[] = 

const uint8_t node_22[] = 

const uint8_t node_23[] = 

const uint8_t node_24[] = 

const uint8_t* drum_map[5][5] = {
  { node_10, node_8, node_0, node_9, node_11 },
  { node_15, node_7, node_13, node_12, node_6 },
  { node_18, node_14, node_4, node_5, node_3 },
  { node_23, node_16, node_21, node_1, node_2 },
  { node_24, node_19, node_17, node_20, node_22 },
};

} // namespace grids

#endif  // GRIDS_RESOURCES_H_