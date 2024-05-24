#ifndef TOMATO_PINS_H
#define TOMATO_PINS_H

/*
 * This header file is largely just to define
 * nice names for the pin numbers
 * and ADC unit channel numbers
 * + there's declaration for a couple functions:
 *    - init_pins just initializes the ADC pins
 *    - analogRead is pretty much the same as arduino's analogRead
*/

// Pin number definitions.

#define OUTPUT_OSC1 16u
#define OUTPUT_OSC2 17u
#define OUTPUT_OSC3 18u
#define OUTPUT_OSC4 19u


#define OUTPUT_LFO1 25u
#define OUTPUT_LFO2 26u


#define KNOB_OSC1_PITCH 36u
#define KNOB_OSC1_DUTY 39u

#define KNOB_OSC2_PITCH 34u
#define KNOB_OSC2_DUTY 35u

#define KNOB_OSC3_PITCH 27u
#define KNOB_OSC3_DUTY 14u

#define KNOB_OSC4_PITCH 32u
#define KNOB_OSC4_DUTY 33u

#define KNOB_LFO1_RATE 12u
#define KNOB_LFO2_RATE 13u



// ADC channel numbers for each pin

#define KNOB_CH_OSC1_PITCH 0
#define KNOB_CH_OSC1_DUTY 3

#define KNOB_CH_OSC2_PITCH 6
#define KNOB_CH_OSC2_DUTY 7

#define KNOB_CH_OSC3_PITCH 4
#define KNOB_CH_OSC3_DUTY 5

#define KNOB_CH_OSC4_PITCH 7
#define KNOB_CH_OSC4_DUTY 6

#define KNOB_CH_LFO1_RATE 5
#define KNOB_CH_LFO2_RATE 4

// Pin numbers for each ADC unit/chanel

#define ADC1_CH0_PIN 36u
#define ADC1_CH3_PIN 39U
#define ADC1_CH4_PIN 32u
#define ADC1_CH5_PIN 33u
#define ADC1_CH6_PIN 34u
#define ADC1_CH7_PIN 35u
#define ADC2_CH0_PIN  4u
#define ADC2_CH1_PIN  0u
#define ADC2_CH2_PIN  2u
#define ADC2_CH3_PIN 15u
#define ADC2_CH4_PIN 13u
#define ADC2_CH5_PIN 12u
#define ADC2_CH6_PIN 14u
#define ADC2_CH7_PIN 27u

/// @brief Hardcoded function to enable ADC pins
void init_pins();

/// @brief Reads a value from an analog pin
/// @param pin Pin number
/// @return 12-bit integer reperesenting the input voltage on the pin.
int analogRead(unsigned char pin);

#endif