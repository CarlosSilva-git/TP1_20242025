#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

#define QUEUE_SIZE 128  // Define the size of the queue
#define F_CPU 16000000UL  // Default to 16 MHz


// Queue structure
typedef struct {
    uint8_t buffer[QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;
} Queue;

// Public function prototypes
void USART_Init(uint32_t baudrate);
void USART_Transmit(uint8_t data);
bool USART_ReceiveAvailable(void);
bool USART_GetReceivedData(uint8_t* data);
void USART_PutString(const char* str);
void USART_PutNumber(int32_t num);
void lineFeed();

#endif  // USART_H
