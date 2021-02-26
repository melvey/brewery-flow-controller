#include "usart.h"
#include "standard.h"


// SPI Data Register Definitions
#define DDR_SPI   DDRB
#define DD_MOSI   DDB3
#define DD_MISO   DDB4
#define DD_SCK    DDB5

// Output Rate Timer Related Constants
#define MIN_PPS_RATE  1000
#define MAX_PPS_RATE  30000
#define DEFAULT_PPS 12000

#define TIMER_FUDGE_FACTOR  128
#define DEFAULT_TIMEOUT  TIMER_FUDGE_FACTOR + 65002 //64202 // 12 kpps
#define BUFFER_SIZE 1280

volatile unsigned int timer_start_value = 0;
volatile int bufferHead = 0;
volatile int bufferTail = 0;
volatile char dataBuffer[BUFFER_SIZE];
char dataBack;
int status_counter = 0;
volatile char bufferWrapped = FALSE;
volatile int bytesRead = 0;
volatile char sendDataFlag;




#define LEDINIT ddrc.bit4 = 1; portc.bit4 = 1;
#define LEDBIT ddrc.bit4

ISR(TIMER1_OVF_vect)
{  
  // Reset the counter
  TCNT1 = timer_start_value;

  /*if(bufferSize() >= 5){
    char byteRead;
    unsigned int xSend;
    unsigned int ySend;
    byteRead = dataBuffer[bufferTail];
    xSend = byteRead << 8;
    byteRead = dataBuffer[bufferTail+1];
    xSend = xSend |(byteRead & 0b0000000011111111);
    byteRead = dataBuffer[bufferTail+2];
    ySend = byteRead << 8;
    byteRead = dataBuffer[bufferTail+3];
    ySend = ySend |(byteRead & 0b0000000011111111);
    byteRead = dataBuffer[bufferTail+4];
    OutputPoint(xSend, ySend, byteRead);
    bufferTail += 5;
    bytesRead += 5;
//    if ((bytesRead >= 100)){
//    bytesRead -= 100;
//    sendDataFlag = TRUE;
//    }
    if(bufferTail >= BUFFER_SIZE){
      bufferTail = 0;
      if(bufferWrapped){
        bufferWrapped = FALSE;
      }
      
    }
    if(bufferTail == BUFFER_SIZE-640){
        sendDataFlag = TRUE;
      }

    
  } */
}

ISR(USART_RX_vect){
  char dataRead = UDR0;
  bufferHead++;
  
  if(bufferHead == 1279){
    bufferHead = 0;
    sendDataFlag = TRUE;
  }
    
}

int bufferSize(void){
  if(bufferWrapped){
    return (bufferHead + BUFFER_SIZE - bufferTail);
  }
  else{
    return (bufferHead - bufferTail);
  }
}

void setup()
{
  status_counter = 0;
  //bUseRateTimer = TRUE;
  //output_flag = 0;
  timer_start_value = 0;
  Init();
  USART_Init();
  SetOutputRate(12000);
  USART_Flush();
  bufferHead = 0;
  bufferTail = 0;
  bufferWrapped = FALSE;
  sendDataFlag = FALSE;
  USART_WriteString("DDS Test\n");
  //USART_WriteCRLF();
  dataBack = TRUE;
  bytesRead = 0;
  delay(1000);
  //UCSR0B |= (1 << RXCIE0);
  
  //USART_WriteString("OK\n");
  USART_Write(250);
  
}


void loop(){ 

  //while(bufferSize() > 640) asm("nop");
  
//  if ((bytesRead == 100)){
//    bytesRead = 0;
//    USART_WriteString("more\n");
//    //delay(5);
//  }
//
  if (sendDataFlag == TRUE){
    sendDataFlag = FALSE;
    //bytesRead = 0;
    //USART_WriteString("more\n");
    USART_Write(100);
    USART_Write(100);
    USART_Write(100);
    USART_Write(150);
    //delay(5);
  }
// 
    
    
    
}


void InitPorts(void)
{
  // Initialize our SPI port to output to our DAC
  ddrb.bit2 = 1;
  portb.bit2 = 1;
  InitSPI();

  // LDAC - disable
  ddrb.bit1 = 1;
  portb.bit1 = 1;

  // Blanking Pin
  ddrb.bit0 = 1;
  portb.bit0 = 1;

  LEDINIT;
  
  // Turn the LED ON
  LEDBIT = 1;
}

void InitSPI(void)
{
   DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK);
   // Enable SPI Master and Set Clock rate to CLK / 2 (I.E. 16MHz)
   // and MSB first and Clock Polarity is Rising on the Leading Edge (Mode 0).
   SPCR |= (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (0 << SPR0);
   SPSR |= (1 << SPI2X); //We need the fast 2X mode.
}

void InitTimers(void)
{
  // Intialize the Timer1

  // Stop the timer
  TCCR1A = 0x00;
  TCCR1B = 0x00;

  // Set the initial timeout value to the default output rate
  SetOutputRate(DEFAULT_PPS);

  // Configure the Timer1 Overflow Interrupt to be ON
  TIMSK1 = 0b00000001;

  // Select the IO clock with NO prescalar
  TCCR1A = 0x00;
  TCCR1B = 0x01;
}

void Init(void)
{
  status_counter = 0; 
  //output_flag = 0;
  timer_start_value = 0;

  // Turn Off Interrupts
  cli();
  // Initlialize our IO ports
  InitPorts();
  // Initialize our Hardware Timers
  InitTimers(); 
  // Turn ON Interrupts
  sei();
}

void SetOutputRate(unsigned int pps)
{
  // Bounds check the points per second
  if (pps > MAX_PPS_RATE) pps = MAX_PPS_RATE;
  if (pps < MIN_PPS_RATE) pps = MIN_PPS_RATE;
  
  // Convert points per second to thousands of points per second
  pps /= 1000;

  // Calculate the timer timeout value from the
  // number of points per second we are supposed
  // to display
  // Timer ceiling - (Clock Rate in kHZ / PPS) + Fudge Factor (found with scope)
  timer_start_value = 0xFFFF - (16000 / pps) + TIMER_FUDGE_FACTOR;
  
  // Set the timer count register
  TCNT1 = timer_start_value;
}
  
void OutputPoint(unsigned int x, unsigned int y, int showLaser)
{
  unsigned int spi_word;
  

  // Scale the X and Y to 12 bit numbers
  x = 65536-x;
  x = x >> 4;
  y = y >> 4;
  
  // Bounds check X and Y
  if (x > 4095) x = 4095;
  if (y > 4095) y = 4095;
  
  //// Wait for the output flag
  //while (!output_flag) asm("nop");
  //// Reset the output flag
  //output_flag = 0;
  
  // Show Laser 
  portb.bit0 = showLaser;        
        
  //// Pull the LDAC line high
  portb.bit1 = 1;
  
  // Load DAC A
  spi_word = 0b0001000000000000 | (y & 0x0FFF);
  // Pull down chip select so the DAC knows we are taling to it
  portb.bit2 = 0;
  // Upper Byte of the command
  SPDR = spi_word >> 8;
  while (!(SPSR & (1 << SPIF)));
  // Lower Byte of the command
  SPDR = spi_word & 0x00FF;
  while (!(SPSR & (1 << SPIF)));
  // Pull up chip select
  portb.bit2 = 1;
  
  // Load DAC B and update the output (assign a 0 to output)
  spi_word = 0b1001000000000000 | (x & 0x0FFF);
  // Pull down chip select so the DAC knows we are taling to it
  portb.bit2 = 0;
  // Upper Byte of the command
  SPDR = spi_word >> 8;
  while (!(SPSR & (1 << SPIF)));
  // Lower Byte of the command
  SPDR = spi_word & 0x00FF;
  while (!(SPSR & (1 << SPIF)));
  // Pull up chip select
  portb.bit2 = 1;


  //// Pull the LDAC line LOW to load the DAC's values
  portb.bit1 = 0;
  /*USART_WriteString("Point sent: x=");
  USART_WriteUnsignedInt(x);
  USART_WriteString(", y=");
  USART_WriteUnsignedInt(y);
  USART_WriteString("\n");*/

  // Do the Status LED
  status_counter++;
  if (status_counter < 500)
  {
    // Reset the LED to flip
    LEDBIT = 1;
  }
  else
  {
    LEDBIT = 0;
  } 
  if (status_counter > 1000) status_counter = 0;
}


