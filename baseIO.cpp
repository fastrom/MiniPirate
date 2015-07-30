/*
 * This file is part of the Bus Pirate project (http://code.google.com/p/the-bus-pirate/).
 *
 * Written and maintained by the Bus Pirate project.
 *
 * To the extent possible under law, the project has
 * waived all copyright and related or neighboring rights to Bus Pirate. This
 * work is published from United States.
 *
 * For details see: http://creativecommons.org/publicdomain/zero/1.0/.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "baseIO.h"
#include "Strings_PGM_MEM.h"


/*
#include "base.h"
#include "busPirateCore.h"//need access to bpConfig
extern struct _bpConfig bpConfig; //holds persistant bus pirate settings (see base.h) need hardware version info

#if defined (BUSPIRATEV4)
extern BYTE cdc_In_len;
extern BYTE cdc_Out_len;
#endif

//echo ASCII 0 or 1, given unsigned char c
*/
void bpEchoState(unsigned int c) {
    if (c) UART1TX('1');
    else UART1TX('0');
}

//
//
//functions for terminal output of various types and lengths
//
//

//Write a string to the user terminal

void bpWstring(char const *s) {
    char c;
    while ((c = *s++)) UART1TX(c);
}

//write a string to the user terminal, finish with a line break

void bpWline(char const *s) {
    char c;
    while ((c = *s++)) UART1TX(c);
    UART1TX(0x0d);
    UART1TX(0x0a);

}

//output an 8bit/byte binary value to the user terminal

void bpWbin(unsigned char c) {
    unsigned char i, j;
    j = 0b10000000;

    bpWstring("0b");

    for (i = 0; i < 8; i++) {
        if (c & j) {
            UART1TX('1');
        } else {
            UART1TX('0');
        }
        j >>= 1;
    }

}

//output an 32bit/long decimal value to the user terminal

void bpWlongdec(unsigned long l) {
    unsigned long c, m;
    unsigned char j, k = 0;

    c = 100000000;
    for (j = 0; j < 8; j++) {
        m = l / c;
        if (k || m) {
            UART1TX(m + '0');
            l = l - (m * c);
            k = 1;
        }
        c /= 10;
    }
    UART1TX(l + '0');

}

// userfriendly printing of looooonng ints

void bpWlongdecf(unsigned long l) {
    unsigned long int temp;
    int mld, mil;

    mld = 0;
    mil = 0;
    temp = 0;

    if (l >= 1000000) {
        temp = l / 1000000;
        bpWintdec(temp);
        UART1TX(',');
        l %= 1000000;
        if (l < 1000) bpWstring("000,");
        mld = 1;
        mil = 1;
    }
    if (l >= 1000) {
        temp = l / 1000;
        if (temp >= 100) {
            bpWintdec(temp);
        } else if (mld) {
            if (temp >= 10) {
                UART1TX('0'); // 1 leading zero
            } else {
                bpWstring("00");
            }
            bpWintdec(temp);
        } else bpWintdec(temp);
        UART1TX(',');
        l %= 1000;
        mil = 1;
    }
    if (l >= 100) {
        bpWintdec(l);
    } else if (mil) {
        if (l >= 10) {
            UART1TX('0'); // 1 leading zero
        } else {
            bpWstring("00");
        }
        bpWintdec(l);
    } else bpWintdec(l);
}


//output an 16bit/integer decimal value to the user terminal

void bpWintdec(unsigned int i) {
    unsigned int c, m;
    unsigned char j, k = 0;

    c = 10000;
    for (j = 0; j < 4; j++) {
        m = i / c;
        if (k || m) {
            UART1TX(m + '0');
            i = i - (m * c);
            k = 1;
        }
        c /= 10;
    }
    UART1TX(i + '0');
}

//output an 8bit/byte decimal value to the user terminal

void bpWdec(unsigned char c) {
    unsigned char d, j, m, k = 0;

    d = 100;
    for (j = 0; j < 2; j++) {
        m = c / d;
        if (k || m) {
            UART1TX(m + '0');
            c = c - (m * d);
            k = 1;
        }
        d /= 10;
    }
    UART1TX(c + '0');
}

//output an 8bit/byte hex value to the user terminal
const unsigned char HEXASCII[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void bpWhex(unsigned int c) {
    unsigned int b;

    bpWstring("0x");
    b = (c >> 4) & 0x0F;
    UART1TX(HEXASCII[b]);
    b = c & 0x0F;
    UART1TX(HEXASCII[b]);
    return;
}

/*
void bpWhexBuf(unsigned int c) {
    unsigned int b;

    UARTbuf('0');
    UARTbuf('x');
    b = (c >> 4) & 0x0F;
    UARTbuf(HEXASCII[b]);
    b = c & 0x0F;
    UARTbuf(HEXASCII[b]);
    return;
}
*/
// output a 16bit hex value to the user terminal

void bpWinthex(unsigned int c) {
    bpWstring("0x");
    int i;
    for (i = 12; i >= 0; i -= 4) {
        UART1TX(HEXASCII[(c >> i) & 0xF]);
    }
    return;
}

// output a 32bit hex value to the user terminal

void bpWlonghex(unsigned long int c) {
    bpWstring("0x");
    int i;
    for (i = 28; i >= 0; i -= 4) {
        UART1TX(HEXASCII[(c >> i) & 0xF]);
    }
    return;
}

//print an ADC measurement in decimal form

void bpWvolts(const unsigned int adc) {
    unsigned char c;

    // input voltage is divided by two and compared to 3.3V
    // volt      = adc / 1024 * 3.3V * 2
    // centivolt = adc / 1024 * 3.3 * 2 * 100 = adc * 165 / 256
    // This is approximately (adc * 29 / 45), making the calculation
    // fit in an unsigned int. The error is less than 1mV.
    const unsigned int centivolt = (adc * 29) / 45;



    bpWdec(centivolt / 100);

    UART1TX('.');

    c = centivolt % 100;

    if (c < 10) // need extra zero?
        UART1TX('0');

    bpWdec(centivolt % 100);
}

/*

// Read the lower 16 bits from programming flash memory

unsigned int bpReadFlash(unsigned int page, unsigned int addr) {
    unsigned int tblold;
    unsigned int flash;

    tblold = TBLPAG;
    TBLPAG = page;
    flash = __builtin_tblrdl(addr);
    TBLPAG = tblold;

    return flash;
}

// Read the full 24 bits from programming flash memory
unsigned long int bpReadFlashFull(unsigned int page, unsigned int addr) {
    unsigned int tblold;
    unsigned long int flash;

    tblold = TBLPAG;
    TBLPAG = page;
    flash = (__builtin_tblrdh(addr) << 16) | __builtin_tblrdl(addr);
    TBLPAG = tblold;

    return flash;
}

#if defined (BUSPIRATEV2) || defined (BUSPIRATEV1A)
//
//
// Base user terminal UART functions
//
//
//Initialize the terminal UART for the speed currently set in bpConfig.termSpeed
static unsigned int UARTspeed[] = {13332, 3332, 1666, 832, 416, 207, 103, 68, 34,}; //BRG:300,1200,2400,4800,9600,19200,38400,57600,115200

void InitializeUART1(void) { // if termspeed==9 it is custom
    // dunno if the additional settings are needed but justin kaas!
    if (bpConfig.termSpeed != 9) Serial.begin(UARTspeed[bpConfig.termSpeed]); //13332=300, 1666=2400, 416=9600, 34@32mhz=115200....
    U1MODE = 0;
    U1MODEbits.BRGH = 1;
    U1STA = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    IFS0bits.U1RXIF = 0;
}
unsigned char UART1TXRdy(void) {
      return  U1STAbits.UTXBF; 
}

unsigned char UART1RXRdy(void) {
    return U1STAbits.URXDA;
}

//new UART ring buffer
//uses user terminal input buffer to buffer UART output
//any existing user input will be destroyed
//best used for binary mode and sniffers
//static struct _UARTRINGBUF{
static unsigned int writepointer;
static unsigned int readpointer;
//}ringBuf;

void UARTbufSetup(void) {
    //setup ring buffer pointers
    readpointer = 0;
    writepointer = 1;
    bpConfig.overflow = 0;
}

void UARTbufService(void) {
    unsigned int i;
    if (U1STAbits.UTXBF == 0) {//check first for free slot

        i = readpointer + 1;
        if (i == TERMINAL_BUFFER) i = 0; //check for wrap
        if (i == writepointer) return; //buffer empty,
        readpointer = i;
        U1TXREG = bpConfig.terminalInput[readpointer]; //move a byte to UART
    }
}

void UARTbufFlush(void) {
    unsigned int i;

    while (1) {
        i = readpointer + 1;
        if (i == TERMINAL_BUFFER) i = 0; //check for wrap
        if (i == writepointer) return; //buffer empty,

        if (U1STAbits.UTXBF == 0) {//free slot, move a byte to UART
            readpointer = i;
            U1TXREG = bpConfig.terminalInput[readpointer];
        }
    }
}

void UARTbuf(char c) {
    if (writepointer == readpointer) {
        BP_LEDMODE = 0; //drop byte, buffer full LED off
        bpConfig.overflow = 1;
    } else {
        bpConfig.terminalInput[writepointer] = c;
        writepointer++;
        if (writepointer == TERMINAL_BUFFER) writepointer = 0; //check for wrap
    }
}

//get a byte from UART

unsigned char UART1RX(void) {
    while (U1STAbits.URXDA == 0);
    return U1RXREG;
}

void WAITTXEmpty(void) {
    while (U1STAbits.TRMT == 0);
}
//add byte to buffer, pause if full
//uses PIC 4 byte UART FIFO buffer
*/

void UART1TX(char c) {
/*
    if (bpConfig.quiet) return;
    while (U1STAbits.UTXBF == 1); //if buffer is full, wait
    U1TXREG = c;
*/
     Serial.print(c);
}

/*
void UART1Speed(unsigned char brg) {
    U1BRG = brg;
}

unsigned char CheckCommsError(void) {
    return U1STAbits.OERR; //check for user terminal buffer overflow error
}

void ClearCommsError(void) {
    U1STA &= (~0b10); //clear overrun error if exists  
}

// interrupt transfer related stuff
unsigned char __attribute__((section(".bss.filereg"))) * UART1RXBuf;
unsigned int __attribute__((section(".bss.filereg"))) UART1RXToRecv;
unsigned int __attribute__((section(".bss.filereg"))) UART1RXRecvd;
unsigned char __attribute__((section(".bss.filereg"))) * UART1TXBuf;
unsigned int __attribute__((section(".bss.filereg"))) UART1TXSent;
unsigned int __attribute__((section(".bss.filereg"))) UART1TXAvailable;

void UART1TXInt() {
    if (IEC0bits.U1TXIE == 1)
        return;
    if (UART1TXAvailable == UART1TXSent)
        return;

    while (U1STAbits.UTXBF == 1); //if buffer is full, wait

    IFS0bits.U1TXIF = 0;

    IEC0bits.U1TXIE = 1;

    U1TXREG = UART1TXBuf[UART1TXSent];
}

// UART1 ISRs

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    UART1RXBuf[UART1RXRecvd] = U1RXREG;
    UART1RXRecvd++;

    if (UART1RXRecvd == UART1RXToRecv) {
        // disable UART1 RX interrupt
        IEC0bits.U1RXIE = 0;
    }

    IFS0bits.U1RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    UART1TXSent++;
    if (UART1TXSent == UART1TXAvailable) {
        // if everything is sent  disale interrupts
        IEC0bits.U1TXIE = 0;
    } else {
        // feed next byte
        U1TXREG = UART1TXBuf[UART1TXSent];
    }

    IFS0bits.U1TXIF = 0;
}


#endif

#if defined(BUSPIRATEV4) && !defined (BPV4_DEBUG)

extern BDentry *CDC_Outbdp, *CDC_Inbdp;

void UART1TX(char c) {
    if (bpConfig.quiet) return;
    putc_cdc(c);
}

void UARTbuf(char c) {
    UART1TX(c);
}

void WAITTXEmpty(void) {
    WaitInReady();
}

unsigned char UART1TXRdy(void) {
    return 1;
}

//is data available?

unsigned char UART1RXRdy(void) {
    if (cdc_Out_len) return 1;
    if (getOutReady()) return 1;
    return 0;
}

//get a byte from UART

unsigned char UART1RX(void) {
    return getc_cdc();
}

void UARTbufFlush(void) {
    CDC_Flush_In_Now();
}

unsigned char CheckCommsError(void) {
    return 0; //check for user terminal buffer overflow error
}

void UARTbufSetup(void) {
}

void UARTbufService(void) {
}

void ClearCommsError(void) {
}

void InitializeUART1(void) {
}

void UART1Speed(unsigned char brg) {
}
#endif

#if defined(BUSPIRATEV4) && defined (BPV4_DEBUG)

//
//
// Base user terminal UART functions
//
//


//Initialize the terminal UART for the speed currently set in bpConfig.termSpeed
static unsigned int UARTspeed[] = {13332, 3332, 1666, 832, 416, 207, 103, 68, 34,}; //BRG:300,1200,2400,4800,9600,19200,38400,57600,115200

void InitializeUART1(void) { // if termspeed==9 it is custom
    // dunno if the additional settings are needed but justin kaas!
    if (bpConfig.termSpeed != 9) U1BRG = UARTspeed[bpConfig.termSpeed]; //13332=300, 1666=2400, 416=9600, 34@32mhz=115200....
    U1MODE = 0;
    U1MODEbits.BRGH = 1;
    U1STA = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    IFS0bits.U1RXIF = 0;
}
unsigned char UART1TXRdy(void) {
      return  U1STAbits.UTXBF; 
}

unsigned char UART1RXRdy(void) {
    return U1STAbits.URXDA;
}

//new UART ring buffer
//uses user terminal input buffer to buffer UART output
//any existing user input will be destroyed
//best used for binary mode and sniffers
//static struct _UARTRINGBUF{
static unsigned int writepointer;
static unsigned int readpointer;
//}ringBuf;

void UARTbufSetup(void) {
    //setup ring buffer pointers
    readpointer = 0;
    writepointer = 1;
    bpConfig.overflow = 0;
}

void UARTbufService(void) {
    unsigned int i;
    if (U1STAbits.UTXBF == 0) {//check first for free slot

        i = readpointer + 1;
        if (i == TERMINAL_BUFFER) i = 0; //check for wrap
        if (i == writepointer) return; //buffer empty,
        readpointer = i;
        U1TXREG = bpConfig.terminalInput[readpointer]; //move a byte to UART
    }
}

void UARTbufFlush(void) {
    unsigned int i;

    while (1) {
        i = readpointer + 1;
        if (i == TERMINAL_BUFFER) i = 0; //check for wrap
        if (i == writepointer) return; //buffer empty,

        if (U1STAbits.UTXBF == 0) {//free slot, move a byte to UART
            readpointer = i;
            U1TXREG = bpConfig.terminalInput[readpointer];
        }
    }
}

void UARTbuf(char c) {
    if (writepointer == readpointer) {
        BP_LEDMODE = 0; //drop byte, buffer full LED off
        bpConfig.overflow = 1;
    } else {
        bpConfig.terminalInput[writepointer] = c;
        writepointer++;
        if (writepointer == TERMINAL_BUFFER) writepointer = 0; //check for wrap
    }
}

//get a byte from UART

unsigned char UART1RX(void) {
    while (U1STAbits.URXDA == 0);
    return U1RXREG;
}

void WAITTXEmpty(void) {
    while (U1STAbits.TRMT == 0);
}
//add byte to buffer, pause if full
//uses PIC 4 byte UART FIFO buffer

void UART1TX(char c) {
    if (bpConfig.quiet) return;
    while (U1STAbits.UTXBF == 1); //if buffer is full, wait
    U1TXREG = c;
}

void UART1Speed(unsigned char brg) {
    U1BRG = brg;
}

unsigned char CheckCommsError(void) {
    return U1STAbits.OERR; //check for user terminal buffer overflow error
}

void ClearCommsError(void) {
    U1STA &= (~0b10); //clear overrun error if exists  
}

// interrupt transfer related stuff
unsigned char __attribute__((section(".bss.filereg"))) * UART1RXBuf;
unsigned int __attribute__((section(".bss.filereg"))) UART1RXToRecv;
unsigned int __attribute__((section(".bss.filereg"))) UART1RXRecvd;
unsigned char __attribute__((section(".bss.filereg"))) * UART1TXBuf;
unsigned int __attribute__((section(".bss.filereg"))) UART1TXSent;
unsigned int __attribute__((section(".bss.filereg"))) UART1TXAvailable;

void UART1TXInt() {
    if (IEC0bits.U1TXIE == 1)
        return;
    if (UART1TXAvailable == UART1TXSent)
        return;

    while (U1STAbits.UTXBF == 1); //if buffer is full, wait

    IFS0bits.U1TXIF = 0;

    IEC0bits.U1TXIE = 1;

    U1TXREG = UART1TXBuf[UART1TXSent];
}

// UART1 ISRs

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    UART1RXBuf[UART1RXRecvd] = U1RXREG;
    UART1RXRecvd++;

    if (UART1RXRecvd == UART1RXToRecv) {
        // disable UART1 RX interrupt
        IEC0bits.U1RXIE = 0;
    }

    IFS0bits.U1RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    UART1TXSent++;
    if (UART1TXSent == UART1TXAvailable) {
        // if everything is sent  disale interrupts
        IEC0bits.U1TXIE = 0;
    } else {
        // feed next byte
        U1TXREG = UART1TXBuf[UART1TXSent];
    }

    IFS0bits.U1TXIF = 0;
}
#endif
*/

//
// New functions from MiniPirate
//

void printStrDec(char const *str, int dec_nbre, int pad_digits) {
  Serial.print(str);
  while (pad_digits>0 && dec_nbre < pow(10,pad_digits--)) Serial.print('0');
  Serial.print(dec_nbre);
}

void printStrHex(char const *str, int hex_nbre) {
  Serial.print(str);
  bpWhex(hex_nbre);
}

void printStrBin(char const *str, int bin_nbre) {
  Serial.print(str);
  bpWbin(bin_nbre);
}

char pollPeek() {
  while(Serial.available() == false);
  return Serial.peek();
}

boolean isNumberPeek() {
  char p = pollPeek();
  return p >= '0' && p <= '9';
}

boolean isBlankPeek() {
  char p = pollPeek();
  return p == ' ';
}

boolean isNumberOrBlankPeek() {
  return isNumberPeek() || isBlankPeek();
}

char pollSerial() {

  char c=0;
  // send data only when you receive data:
  while(Serial.available() == 0);
  c = Serial.read();
  Serial.print(c);
  return c;
}

char pollLowSerial() {
  return tolower(pollSerial());
}

void pollBlanks() {
 while(isBlankPeek()) {
   pollSerial();
 }
}

//
// Parse BIN (0bXXX), HEX (0xXXX) and DEC (XXX)
//
int pollInt() {

  pollBlanks();
  while(Serial.available() == 0);
  int value = 0;
  if(pollPeek() == '0') {
      pollSerial();
      switch(tolower(pollPeek())) {
        case 'x':
          pollSerial();
          while(   (tolower(pollPeek()) >= '0' && tolower(pollPeek()) <= '9')
                || (tolower(pollPeek()) >= 'a' && tolower(pollPeek()) <= 'f')
               ) {
            value <<= 4;
            char c = pollLowSerial();
            if(c <= '9') {
              value += c - '0';
            } else { // a-f
              value += 10 + c - 'a';
            }
          }
          return value;
          break;

        case 'b':
          pollSerial();
          while(pollPeek() == '0' || pollPeek() == '1') {
            value <<= 1;
            value += (pollSerial() == '1' ? 1 : 0);
          }
          return value;
          break;
         // default: would be octal, no clue if this is required yet.
         // take care about a "pure" zero

        default:
        break;
      }
  }
  if(isNumberPeek()) {
    value = Serial.parseInt();
    Serial.print(value);
  }
  return value;
}

int peekSerial() {

  // Peek data only when you receive data:
  while(Serial.available() == 0);
  return Serial.peek();
}

int pollPin() {

  pollBlanks();
  int pin = 0;
  if(!isNumberPeek()) {
    char c   = pollLowSerial();
    switch(c) {
      case 'a':
        pin += A0;
      case 'd':
        if(!isNumberPeek()) {
          return -1;
        }
    }
  } 
  pin += pollInt();
  if (pin  >= NUM_DIGITAL_PINS)
	  {
	  SERIAL_PRINTLN_PGM ("Invalid pin number!");
	  return -1;
	  }
  
  return pin;
}

void printHighLow(int value) {
	if (value) 
		{  SERIAL_PRINT_PGM("HIGH");    } 
	else 
		{ SERIAL_PRINT_PGM("LOW ");    } 
}

void printInOut(int value) {
	if (value) 
		{ SERIAL_PRINT_PGM(" OUTPUT");    } 
	else 
		{ SERIAL_PRINT_PGM("  INPUT");    }
	}

void printPin(int pin) {
  if(pin >= A0) {
    SERIAL_PRINT_PGM("A");    
    pin -= A0;
  } else {
    SERIAL_PRINT_PGM("D");    
  }
  Serial.print(pin);    
}

void printPorts() {
       for(int i = 0; i < A0; i++) {
         int value = digitalRead(i);

         SERIAL_PRINT_PGM("Value on pin D");
         Serial.print(i);
         if(i < 10) Serial.print(' ');
         // http://garretlab.web.fc2.com/en/arduino/inside/arduino/Arduino.h/portModeRegister.html
         int pin_mode = getPinMode(i);
		 printInOut(pin_mode);
         SERIAL_PRINT_PGM(": ");
         printHighLow(value);

#ifdef digitalPinHasPWM
		 if (digitalPinHasPWM(i)) SERIAL_PRINT_PGM(" PWM");
		 if (digitalPinToInterrupt(i)>-1){
			 SERIAL_PRINT_PGM(" IRQ_");
			 Serial.print(digitalPinToInterrupt(i));
			 }
         Serial.println();
       }
       for(int i = 0; i < NUM_ANALOG_INPUTS; i++) {
         int a_value = analogRead(i);
         int value = digitalRead(A0+i);

         printStrDec("Value on pin A", i);
         if(i < 10) Serial.print(' ');
         int pin_mode = getPinMode(A0+i);
		 printInOut(pin_mode);
         SERIAL_PRINT_PGM(": ");
         printHighLow(value);
         printStrDec(" / ", a_value,3);
		 SERIAL_PRINT_PGM(" / ");
		 extern float VCC;
		 Serial.print(a_value / 1023.0f * VCC);
		 SERIAL_PRINT_PGM("V");
     //    printStrDec(" / ", a_value);
         Serial.println();
#endif // digitalPinHasPWM
       }
}



// Prints all port direction and values in a matrix (concise) format
// Digital values are 8 across and analog are 4 across
void printPortsQuick() {
	for(int i = 0; i < A0; i++) {
		int value = digitalRead(i);
		if (i%8==0) {
			SERIAL_PRINT_PGM("D");
			Serial.print(i);
			SERIAL_PRINT_PGM(": ");
			if(i < 10) Serial.print(' ');
			}
		int pin_mode = getPinMode(i);
		Serial.print(( pin_mode == 0 ? " <" : " >" ));
		Serial.print(value?"1 ":"0 ");
		if (i%8==7) Serial.println();
		}
	Serial.println("");
	for(int i = 0; i < NUM_ANALOG_INPUTS; i++) {
		int a_value = analogRead(i);
		int value = digitalRead(A0+i);
		if (i%4==0) {
			SERIAL_PRINT_PGM("A");
			Serial.print(i);
			SERIAL_PRINT_PGM(": ");
			if(i < 10) Serial.print(' ');
			}
		
		int pin_mode =getPinMode(A0+i);
		Serial.print(( pin_mode == 0 ? " <" : " >" ));
		Serial.print(value?"1":"0");
		printStrDec("(",a_value,3);
/*
		if (a_value<1000) Serial.print('0');
		if (a_value<100) Serial.print('0');
		if (a_value<10) Serial.print('0');
		Serial.print(a_value);
*/
		SERIAL_PRINT_PGM(")");
		if (i%4==3) Serial.println();
		}
	}

