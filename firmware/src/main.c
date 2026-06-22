/*
====================================================
PROJECT:
ATmega32 Temperature Monitoring System

FEATURES:
1. Read temperature from ADC0 (Potentiometer/LM35)
2. Green LED for Normal Temperature
3. Yellow LED for Warning Temperature
4. Red LED for High Temperature
5. Buzzer ON during High Temperature
6. Push Button on PD2 silences buzzer
7. Reset button connected to RST pin
====================================================
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

/*
----------------------------------------------------
ADC INITIALIZATION
----------------------------------------------------
*/
void ADC_Init(void)
{
    // Use AVCC as ADC reference voltage
    ADMUX = (1 << REFS0);

    // Enable ADC
    // Prescaler = 64
    ADCSRA =
        (1 << ADEN) |
        (1 << ADPS2) |
        (1 << ADPS1);
}

/*
----------------------------------------------------
READ ADC CHANNEL
----------------------------------------------------
channel = ADC input number
Returns 10-bit ADC value
----------------------------------------------------
*/
unsigned int ADC_Read(unsigned char channel)
{
    // Limit channel between 0 and 7
    channel &= 0x07;

    // Select ADC channel
    ADMUX = (ADMUX & 0xE0) | channel;

    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait until conversion complete
    while (ADCSRA & (1 << ADSC));

    // Return ADC result
    return ADC;
}

/*
----------------------------------------------------
MAIN PROGRAM
----------------------------------------------------
*/
int main(void)
{
    // ADC initialization
    ADC_Init();

    /*
    ---------------------------------------------
    LED OUTPUTS
    PB0 = Green LED
    PB2 = Yellow LED
    PB4 = Red LED
    ---------------------------------------------
    */
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB4);

    /*
    ---------------------------------------------
    Buzzer Output
    PD6 = Buzzer
    ---------------------------------------------
    */
    DDRD |= (1 << PD6);

    /*
    ---------------------------------------------
    Push Button Input
    PD2 = Alarm Silence Button
    Internal Pull-Up Enabled
    ---------------------------------------------
    */
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    while (1)
    {
        unsigned int adcValue;

        float voltage;
        float temperature;

        /*
        -----------------------------------------
        Read ADC0
        -----------------------------------------
        */
        adcValue = ADC_Read(0);

        /*
        -----------------------------------------
        Convert ADC value to Voltage
        -----------------------------------------
        */
        voltage =
            (adcValue * 5.0) / 1023.0;

        /*
        -----------------------------------------
        LM35 gives 10mV per degree Celsius
        100mV = 10°C
        Therefore:
        Temp = Voltage × 100
        -----------------------------------------
        */
        temperature =
            voltage * 100.0;

        /*
        -----------------------------------------
        Turn everything OFF first
        -----------------------------------------
        */
        PORTB &= ~(1 << PB0);
        PORTB &= ~(1 << PB2);
        PORTB &= ~(1 << PB4);

        PORTD &= ~(1 << PD6);

        /*
        -----------------------------------------
        NORMAL TEMPERATURE
        Below 30°C
        -----------------------------------------
        */
        if (temperature < 30)
        {
            PORTB |= (1 << PB0);
        }

        /*
        -----------------------------------------
        WARNING TEMPERATURE
        30°C to 39°C
        -----------------------------------------
        */
        else if (temperature < 40)
        {
            PORTB |= (1 << PB2);
        }

        /*
        -----------------------------------------
        HIGH TEMPERATURE
        40°C and Above
        -----------------------------------------
        */
        else
        {
            PORTB |= (1 << PB4);

            /*
            -------------------------------------
            Check Silence Button
            PD2 uses pull-up resistor

            Pressed = 0
            Released = 1
            -------------------------------------
            */
            if (!(PIND & (1 << PD2)))
            {
                // Buzzer OFF
                PORTD &= ~(1 << PD6);
            }
            else
            {
                // Buzzer ON
                PORTD |= (1 << PD6);
            }
        }

        _delay_ms(200);
    }

    return 0;
}