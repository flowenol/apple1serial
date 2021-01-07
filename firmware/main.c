#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// this seems to be the upper limit, above that some data is not received
#define BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>

#define DATA_READ_LINE PB0
#define DATA_WRITE_LINE PD7
#define STATUS PD6

#define DATA0 PB1
#define DATA1 PB2
#define DATA2 PB3
#define DATA3 PB4
#define DATA4 PB5
#define DATA5 PD2
#define DATA6 PD3
#define DATA7 PD4

#define BUFFER_SIZE 100

typedef enum {NONE, READ, WRITE} mode_t;
volatile mode_t mode = NONE;

volatile uint8_t data = 0x00;
volatile uint8_t upper_part = 0x00;
volatile uint8_t lower_part = 0x00;

volatile uint8_t data_write_interrupt_count = 0;
volatile uint8_t data_read_interrupt_count = 0;

// receive buffer
volatile uint8_t receive_buffer[BUFFER_SIZE];
volatile int8_t receive_index = -1;

// Registers: PCMSK0, PCMSK1, PCMSK2 :registers that enable or disable pin-change interrupts
// on individual pins

// PCICR : a register where the three least significant bits enable or disable pin change interrupts
// on a range of pins, i.e. {0,0,0,0,0,PCIE2,PCIE1,PCIE0}, where PCIE2 maps to PCMSK2, PCIE1 maps to PCMSK1,
// and PCIE0 maps to PCMSK0.

inline void ready() {
    PORTD |= _BV(STATUS);
}

inline void not_ready() {
    PORTD &= ~(_BV(STATUS));
}

inline void put_data() {
    upper_part = (PORTD & 0xe3) | ((0xe0 & receive_buffer[0])>>3);
    lower_part = (PORTB & 0xc1) | ((0x1f & receive_buffer[0])<<1);

    PORTD = upper_part;
    PORTB = lower_part;
}

void setup() {

    // set input on DATA_READ_LINE
    DDRB &= ~(_BV(DATA_READ_LINE));
    // interrupt on DATA_READ_LINE change
    PCMSK0 |= _BV(PCINT0);
    PCICR |= _BV(PCIE0);

    // set input on DATA_WRITE_LINE
    DDRD &= ~(_BV(DATA_WRITE_LINE));
    // interrupt on DATA_WRITE_LINE change
    PCMSK2 |= _BV(PCINT23);
    PCICR |= _BV(PCIE2);

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    UCSR0A = 0x00;
    UCSR0C = 0x00;
    // 8 bits of data, 1 stop bit, no parity
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
    // TXD pullup
    PORTD |= _BV(PD1);

    // interrupt on DATA_READ_LINE change
    PCMSK0 |= _BV(PCINT0);
    PCICR |= _BV(PCIE0);

    // set output on status
    DDRD |= _BV(STATUS);
    not_ready();

    // enable global interrupts
    sei();
}

// data read line
ISR(PCINT0_vect) {
    if (data_read_interrupt_count == 0) {
        if (mode != READ) {
            mode = READ;

            // enable RXD and it's interrupt
            UCSR0B = _BV(RXEN0) | _BV(RXCIE0);

            // set output on data pins
            DDRD |= _BV(DATA5) | _BV(DATA6) | _BV(DATA7);
            DDRB |= _BV(DATA0) | _BV(DATA1) | _BV(DATA2) | _BV(DATA3) | _BV(DATA4);

            receive_index = -1;
            not_ready();
            data_read_interrupt_count = 1;
            return;
        }

        if (receive_index > -1) {
            uint8_t i;
            for (i = 1; i <= receive_index; i++) {
                receive_buffer[i - 1] = receive_buffer[i];
            }
            receive_index--;

            if (receive_index == -1) {
                not_ready();
            } else {
                // put next byte from receive buffer
                put_data();
            }
        }
        data_read_interrupt_count = 1;
    } else {
        data_read_interrupt_count = 0;
    }
}

// data write line
ISR(PCINT2_vect) {
    upper_part = PINB;
    lower_part = PIND;

    if (data_write_interrupt_count == 0) {
        if (mode == WRITE) {
            upper_part = upper_part >> 1;
            upper_part &= 0x1f;

            lower_part = lower_part << 3;
            lower_part &= 0xe0;

            // set status to not ready
            PORTD &= ~(_BV(STATUS));

            // send byte
            loop_until_bit_is_set(UCSR0A, UDRE0);
            UDR0 = upper_part | lower_part;
        } else {
            if (mode == READ) {
                // zero all outputs
                PORTD &= 0xe3;
                PORTB &= 0xc1;
            }
            mode = WRITE;

            // enable TXD interrupt and it's interrupt
            UCSR0B =  _BV(TXEN0) | _BV(TXCIE0);

            // set input on data pins
            DDRD &= ~(_BV(DATA5) | _BV(DATA6) | _BV(DATA7));
            DDRB &= ~(_BV(DATA0) | _BV(DATA1) | _BV(DATA2) | _BV(DATA3) | _BV(DATA4));

            ready();
        }
        data_write_interrupt_count = 1;
    } else {
        data_write_interrupt_count = 0;
    }
}

ISR(USART_RX_vect)
{
    if (mode == READ) {
        data = UDR0;

        if (receive_index >= BUFFER_SIZE - 1) {
            receive_index = -1;
        }

        receive_buffer[++receive_index] = data;

        // always put first byte from receive buffer
        put_data();

        ready();
    }
}

ISR(USART_TX_vect)
{
    if (mode == WRITE) {
        ready();
    }
}

int main(void) {
    setup();

    for(;;){
        // only interrupt driven
    }
    return 0;   /* never reached */
}
