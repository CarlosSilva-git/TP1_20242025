#include <avr/io.h>
#include "usart.h"
#include <util/delay.h>

/*
Sets up a variable pwm on PB1 - Digital 9
through OC1A pin :)
first on 1000 Hz
*/
void PWM_init()
{
    DDRB |= (1 << PB1); // Sets portB 1 as output
                        // Configure Timer1 for Fast PWM mode with ICR1 as TOP
    TCCR1A = (1 << COM1A1) | (1 << WGM11);              // Non-inverting mode on OC1A/OC1B
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8, Fast PWM with ICR1
    ICR1 = 1999;
    OCR1A = 1000;
}

void offPWM()
{
    TCCR1A &= ~(1 << COM1A1);
    // Set output pins low
    PORTB &= ~(1 << PB1); // Ensure PB1 and PB2 are low
}
void onPWM()
{
    TCCR1A |= (1 << COM1A1);
}

void changePitch(bool increase){
    uint16_t aux_val = ICR1;
    if(aux_val > 600 && increase){
        ICR1 -= 50;
        OCR1A = ICR1 / 2;
    }
    else if(aux_val < 3000 && !increase)
    {
        ICR1 += 50;
        OCR1A = ICR1 / 2;
    }
    USART_PutString("UPDATING ICR1 TO: ");
    USART_PutNumber(ICR1);
}

int main(void)
{
    USART_Init(9600); // Initialize USART with baud rate 9600 (for 16 MHz clock)
    PWM_init();

    while (1)
    {
        if (USART_ReceiveAvailable())
        {
            uint8_t received;
            USART_PutString("Echoing: ");
            while (USART_GetReceivedData(&received))
            {
                USART_Transmit(received); // Echo back received data
                if(received == 'D')
                {
                    offPWM();
                }
                else if(received == 'L')
                {
                    onPWM();
                }
                else if(received == '+')
                {
                    changePitch(true);
                }
                else if(received == '-')
                {
                    changePitch(false);
                }
            }
            lineFeed();
        }
        _delay_ms(100);
    }
}
