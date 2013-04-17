// the actual header file for attiny13 lives at: /usr/local/CrossPack-AVR/avr/include/avr
// this is the header file that tells the compiler what pins and ports, etc.
// are available on this chip.
#include <avr/io.h>
#include <util/delay_basic.h>
// define what pins the LEDs are connected to.
// in reality, PD6 is really just '6'
#define ALARM PB0 //PIN #5
#define SWITCH PB4 //ADC2, PIN #3
#define LDR PB5 //ADC0, PIN #1

// Some macros that make the code more readable
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)
#define bit_is_set(port, pin) (port & (1<<pin)) == (1<<pin)
#define bit_is_clear(port, pin) (port | ~(1<<pin)) == ~(1<<pin)

int main(void) {
  set_output(DDRB, ALARM); //initialize ALARM pin to be an output
  set_input(DDRB, LDR); //initialize LDR pin to be an input
  set_input(DDRB, SWITCH); //initialize SWITCH pin to be an input

   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //set ADC prescaler (FCPU / 128)
  //ADMUX &= ~(1 << REFS0); //[DEFAULT] set ref voltage to Vcc (5v)
  ADMUX |= (1 << ADLAR); //set ADC to 'left align' mode, only ADCH value will be read
  
  while (1) {
    ADMUX &= ~(1 << MUX1) & ~(1 << MUX0); //set input channel selection to ADC0 (MUX1..0==[00], SWITCH)
    ADCSRA |= (1 << ADEN);  //enable ADC conversions
    ADCSRA |= (1 << ADSC); //start ADC conversion
    while(bit_is_set(ADCSRA, ADSC)){
      //wait for conversion to finish (first conversion takes 25 clock cycles, subsequent ones take 13)
    }
    //check status of switch
    if(ADCH > 128){ //door is shut, ignore light level
      output_low(PORTB, ALARM); //turn off ALARM
      ADCSRA &= ~(1 << ADEN); //disable ADC conversions
      //TODO: put IC to sleep or some other significant delay to reduce power while door is closed
    }
    else{ //door is open, check for light level
      //set input channel selection to ADC2 (MUX1..0==[10], LDR)
      ADMUX |= (1 << MUX1);
      ADMUX &= ~(1 << MUX0); 
      //TODO: make sure changing to different ADC worked
      ADCSRA |= (1 << ADSC); //start ADC conversion
      while(bit_is_set(ADCSRA, ADSC)){
	//wait for conversion to finish (first conversion takes 25 clock cycles, subsequent ones take 13)
      }
      if(ADCH < 64){ //light level is below threshold
	output_high(PORTB, ALARM); //activate ALARM
      }
    }
    // now start over
  }
}
