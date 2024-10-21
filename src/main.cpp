#include <avr/io.h>
#include <util/delay.h>

// Define the LED pin
#define LED_PIN PB5


void setup(){
  // Configurar o pino de saida
  DDRB |= (1 << LED_PIN);
  // Configurar perifericos
  // Etc...

}

int main()
{
  setup();
  uint16_t delay = 100, delay_time;

  while (1)
  {
    // Toggle LED
    PORTB ^= (1 << LED_PIN);
    delay += 50;
    if(delay > 1500){
      delay=100;
    }
    delay_time = delay;
    while(delay_time > 0)
    {
      _delay_ms(1);
      delay_time -= 1;
    }
  }
  
  
  return 0;
}