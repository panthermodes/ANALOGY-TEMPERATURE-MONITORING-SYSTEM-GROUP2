#define F_CPU 16000000UL

#include  <avr/io.h>
#include <util/delay.h>

void ADC_Init(void)
{
    ADMUX = (1 << REFS0);

    ADCSRA =
        (1 << ADEN)  |
        (1 << ADPS2) |
        (1 << ADPS1);
}

unsigned int ADC_Read(unsigned char channel)
{
    channel &= 0x07;

    ADMUX = (ADMUX & 0xE0) | channel;

    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    return ADC;
}

int main(void)
{
    ADC_Init();

    DDRB |= (1 << PB0);
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB4);

    DDRD |= (1 << PD6);

    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    while (1)
    {
        unsigned int adcValue;
        float voltage;
        float temperature;

        adcValue = ADC_Read(0);

        voltage = (adcValue * 5.0) / 1023.0;

        temperature = voltage * 100.0;

        PORTB &= ~((1 << PB0) | (1 << PB2) | (1 << PB4));

        PORTD &= ~(1 << PD6);

        if (temperature < 30)
        {
            PORTB |= (1 << PB0);
        }
        else if (temperature < 40)
        {
            PORTB |= (1 << PB2);
        }
        else
        {
            PORTB |= (1 << PB4);

            if (!(PIND & (1 << PD2)))
            {
                PORTD &= ~(1 << PD6);
            }
            else
            {
                PORTD |= (1 << PD6);
            }
        }

        _delay_ms(200);
    }

    return 0;
}