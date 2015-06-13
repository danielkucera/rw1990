#define F_CPU 1000000UL	// Oscillator frequency.
#define BAUD 9600
#define MYUBRR (1150000UL / 16 / BAUD ) - 1 

#include <util/delay.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/setbaud.h>
#include <avr/io.h>


#define GO_LO DDRB |= 0x01; PORTB&=0xFE
#define GO_HI DDRB &= 0xFE; PORTB|=0x01 

char data[64];

void uart_init(void) {
 	/*Set baud rate */ 
	UBRRH = (unsigned char)( MYUBRR >> 8); 
	UBRRL = (unsigned char) MYUBRR; 
	/* Enable receiver and transmitter   */
	UCSRA = 0;
	UCSRB = (1<<RXEN)|(1<<TXEN); 
	UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
	/* Frame format: 8data, No parity, 1stop bit */ 
	UCSRC = (3<<UCSZ0);  
}

unsigned char serialCheckTxReady(void)
{
	return( UCSRA & _BV(UDRE) ) ;		// nonzero if transmit register is ready to receive new data.
}


void serialWrite(unsigned char DataOut)
{
	while (serialCheckTxReady() == 0)		// while NOT ready to transmit 
	{;;} 
	_delay_us(200);
	UDR = DataOut;
}

int sendByte(char byte){

	int bit;

	for(bit=0; bit<8; bit++){
		if (byte & 1){
			GO_LO;
			_delay_us(8);
			GO_HI;
			_delay_us(44);
		} else {
			GO_LO;
			_delay_us(44);
			GO_HI;
			_delay_us(8);
		}

		byte = byte >> 1;

	}
return 0;
}

int writeByte(char byte){

	int bit;

	for(bit=0; bit<8; bit++){
		if (byte & 1){
			GO_LO;
			_delay_us(60);
			GO_HI;
			_delay_ms(10);

		} else {
			GO_LO;
//			_delay_us(8);
			GO_HI;
			_delay_ms(10);
		}

		byte = byte >> 1;

	}
return 0;
}

char numToHex(int num){

	if ((num >= 0) && (num < 10)){
		return num + '0';
	}

	if ((num >= 9) && (num < 16)){
		return num - 10 + 'A';
	}

	return '?';

}

int recvByte(){

	int bit;
	char byte=0;

	for(bit=0; bit<8; bit++){
		GO_LO;
		_delay_us(8);
		GO_HI;
		_delay_us(8);
	
		byte = (byte >> 1) + (PINB & 0x01)*128;

		_delay_us(32);

	}

	return byte;

}

int sendR(){
	GO_LO;
	_delay_us(1024);
	GO_HI;
}

int waitP(){

	while (bit_is_set(PINB,0)){
	}
	while (!bit_is_set(PINB,0)){
	}

	serialWrite('P');
	serialWrite('\r');
	serialWrite('\n');

}

int main(void)
{

	int rcvByte = 0;
	uart_init();

	DDRB &= 0xFE; //set PB0 to input
	GO_HI;

	sendR();
	waitP();

	sendByte(0x33);

	for (rcvByte=0; rcvByte < 8; rcvByte++){
		data[rcvByte] = recvByte();
	}

	for (rcvByte=0; rcvByte < 8; rcvByte++){
		serialWrite(numToHex(data[rcvByte]/16));
		serialWrite(numToHex(data[rcvByte]%16));
		data[rcvByte] = 0;
	}
	serialWrite('\r');
	serialWrite('\n');

	_delay_ms(16);
	sendR();
	waitP();

	// table 3

	sendByte(0xD5);

	writeByte(0x01);
	writeByte(0x02);
	writeByte(0x03);
	writeByte(0x04);
	writeByte(0x05);
	writeByte(0x06);
	writeByte(0x07);
	writeByte(0x08);


	sendR();
	waitP();

	serialWrite('Z');
	serialWrite('\r');
	serialWrite('\n');

	sendR();
	waitP();

	sendByte(0x33);

	for (rcvByte=0; rcvByte < 8; rcvByte++){
		data[rcvByte] = recvByte();
	}

	for (rcvByte=0; rcvByte < 8; rcvByte++){
		serialWrite(numToHex(data[rcvByte]/16));
		serialWrite(numToHex(data[rcvByte]%16));
		data[rcvByte] = 0;
	}
	serialWrite('\r');
	serialWrite('\n');

}
