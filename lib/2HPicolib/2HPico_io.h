
// Copyright 2023 Rich Heslip
//
// Author: Rich Heslip 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// I/O pin definitions for Raspberry Pi Pico eurorack module


#ifndef IO_H_
#define IO_H_

// I2S pins for ADC and DAC
#define MCLK 11
#define BCLK 12
#define WS 13  // this will always be 1 pin above BCLK - can't change it
#define I2S_DATA 14  // Out of Pico to DAC
#define I2S_DATAIN 15  // into Pico from ADC

#define CPU_USE 8 // unused GPIO shows core 1 processor usage

// Gate/trigger digital inputs 
#define TRIGGER 26  // analog input works as a digital input as long as input voltage is 4v or more

#define BUTTON1 2
#define LEDPIN 3 // for RGB Led

#define AIN0 	26
#define AIN1 	27
#define AIN2 	28
#define AIN3 	29 // not available on standard Pico board
#define MUXCTL 5 // hi to read pots 1 & 3, low to read pots 2 & 4

#endif // IO_H_

