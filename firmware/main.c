// the actual header file for attiny13 lives at: /usr/local/CrossPack-AVR/avr/include/avr                                                                                                              
// this is the header file that tells the compiler what pins and ports, etc.                                                                                                                           
// are available on this chip.                                                                                                                                                                         
#define F_CPU 9600000UL / 8

#include <avr/io.h>
#include <util/delay.h>

#define ALARM PB0
#define SWITCH PB3

int i;

// Some macros that make the code more readable
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

int main(void) {
  // initialize the direction of PB0 to be an output
  set_output(DDRB, ALARM);
  set_input(DDRB, SWITCH);
  PORTB = (1 << SWITCH);
  ADCSRA |= (1 << ADEN);  //enable ADC conversions                                                                                                                                                      
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //set ADC prescaler (FCPU / 128)                                                                                                              
  ADMUX |= (1 << ADLAR); //set ADC to 'left align' mode, only ADCH value will be read                                                                                                                  
  ADMUX |= (1 << MUX0); //set AD input AD1 (PB2)

  while (1) {
    if(bit_is_clear(PINB, SWITCH)){
      ADCSRA |= (1 << ADEN);  //enable ADC conversions                                                                                                                                            
      ADCSRA |= (1 << ADSC);  //start single ADC conversion                                                                                                                                            
      while(ADCSRA & (1 << ADSC)); //wait for conversion to finish                                                                                                                                     
      ADCSRA |= (1 << ADSC);  //start single ADC conversion
      while(ADCSRA & (1 << ADSC)); //wait for conversion to finish                                                                                                                                     
      ADCSRA &= ~(1 << ADEN); //disable ADC                                                                                                                                            

      if(ADCH < 64){
	output_high(PORTB, ALARM);
      }
      else{
	output_low(PORTB, ALARM);
      }
    }
    else{
      output_low(PORTB, ALARM);
    }
    // now start over                                                                                                                                                                                  
  }
}
