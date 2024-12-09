#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h> // For itoa()
#include "usart.h"

static Queue rxQueue; // Queue for received data

// Internal queue management functions
static bool Queue_Push(Queue *q, uint8_t data);
static bool Queue_Pop(Queue *q, uint8_t *data);
static bool Queue_IsEmpty(const Queue *q);

// USART initialization
void USART_Init(uint32_t baudrate)
{
    // Set baud rate
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;

    // Set baud rate
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Enable RX Complete interrupt
    UCSR0B |= (1 << RXCIE0);

    // Set frame format: 8 data bits, no parity, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Initialize queue
    rxQueue.head = 0;
    rxQueue.tail = 0;
    rxQueue.count = 0;

    // Enable global interrupts
    sei();
}

// USART transmit
void USART_Transmit(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;        // Wait for empty transmit buffer
    UDR0 = data; // Put data into buffer, sends the data
}

// Check if received data is available
bool USART_ReceiveAvailable(void)
{
    return !Queue_IsEmpty(&rxQueue);
}

// Retrieve received data
bool USART_GetReceivedData(uint8_t *data)
{
    return Queue_Pop(&rxQueue, data);
}

// USART receive interrupt service routine
ISR(USART_RX_vect)
{
    uint8_t data = UDR0;        // Read received data
    Queue_Push(&rxQueue, data); // Push data into the queue
}

// Internal queue implementation
static bool Queue_Push(Queue *q, uint8_t data)
{
    if (q->count == QUEUE_SIZE)
    {                 // Check if the queue is full
        return false; // Queue is full
    }
    q->buffer[q->head] = data;
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count++;
    return true;
}

static bool Queue_Pop(Queue *q, uint8_t *data)
{
    if (q->count == 0)
    {                 // Check if the queue is empty
        return false; // Queue is empty
    }
    *data = q->buffer[q->tail];
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count--;
    return true;
}

static bool Queue_IsEmpty(const Queue *q)
{
    return q->count == 0;
}

// Send a null-terminated string over USART
void USART_PutString(const char *str)
{
    while (*str)
    { // Continue until the null terminator is reached
        USART_Transmit(*str++);
    }
}

// Send a number as a string over USART
void USART_PutNumber(int32_t num)
{
    char buffer[12];         // Enough to hold a 32-bit integer (-2147483648 to 2147483647)
    itoa(num, buffer, 10);   // Convert number to string (base 10)
    USART_PutString(buffer); // Send the string
}

void lineFeed()
{
    USART_Transmit('\n');
    USART_Transmit('\r');
}
