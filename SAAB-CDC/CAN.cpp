/*
 ----------------------------------------------------------------------------------
 CAN.cpp
 CONTROLLER AREA NETWORK (CAN 2.0A STANDARD ID)
 CAN BUS library for Wiring/Arduino - Version 1.1
 ADAPTED FROM http://www.kreatives-chaos.com
 By IGOR REAL (16 - 05 - 2011)
 ----------------------------------------------------------------------------------
 */
/*
 Name:
 Parameters(type):
 Description:
 Example:
 
 */




#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "CAN.h"

#define DEBUGMODE	0


/******************************************************************************
 * Variables
 ******************************************************************************/
CANClass CAN;
CANClass::msgCAN CAN_TxMsg;
CANClass::msgCAN CAN_RxMsg;


/******************************************************************************
 * Constructors
 ******************************************************************************/




/******************************************************************************
 * PUBLIC METHODS
 ******************************************************************************/
void CANClass::begin(uint16_t speed)
{
    
#if (DEBUGMODE==1)
    Serial.println(F("-- Constructor Can(uint16_t speed) --"));
#endif
    
    SET(MCP2515_CS);
    SET_OUTPUT(MCP2515_CS);
    
    RESET(P_SCK);
    RESET(P_MOSI);
    RESET(P_MISO);
    
    SET_OUTPUT(P_SCK);
    SET_OUTPUT(P_MOSI);
    SET_INPUT(P_MISO);
    
    SET_INPUT(MCP2515_INT);
    SET(MCP2515_INT);
    
    // We activate the SPI Arduino as Master and Fosc/2=8 MHz
    SPCR = (1<<SPE)|(1<<MSTR) | (0<<SPR1)|(0<<SPR0);
    SPSR = (1<<SPI2X);
#if (DEBUGMODE==1)
    Serial.println(F("SPI=8 Mhz"));
#endif
    
    
    
    // reset MCP2515 by software reset.
    // After this he is in configuration mode.
    RESET(MCP2515_CS);
    spi_putc(SPI_RESET);
    SET(MCP2515_CS);
    
    // wait a little bit until the MCP2515 has restarted
    _delay_us(10);
    
    
    
    switch(speed)
    {
        case 47:
            /*
             Supposed to use...?
             SJW = 1
             SP% ~= 75
            */
            //Original CNF values
             mcp2515_write_register(CNF1,0xC7);
             mcp2515_write_register(CNF2,0xBE);
             mcp2515_write_register(CNF3,0x04);
            /*
             T1   = 16
             T2   = 5
             BTQ  = 21
             SP%  = 76.19
             SJW  = 4
             Err% = 0
             */
            
            /*
             // Version 1
             mcp2515_write_register(CNF1,0x4B);
             mcp2515_write_register(CNF2,0xF1);
             mcp2515_write_register(CNF3,0x03);
             
             T1   = 14
             T2   = 7
             BTQ  = 21
             SP%  = 66.67
             SJW  = 4
             Err% = 0
             
             
             // Version 2
             mcp2515_write_register(CNF1,0xC7);
             mcp2515_write_register(CNF2,0xFE);
             mcp2515_write_register(CNF3,0x44);
             
             T1   = 16
             T2   = 8
             BTQ  = 24
             SP%  = 66.67
             SJW  = 1
             Err% = 0
             */
            /*
            // Version 3
            mcp2515_write_register(CNF1,0xC7);
            mcp2515_write_register(CNF2,0xBE);
            mcp2515_write_register(CNF3,0x44);
             T1   = 10
             T2   = 4
             BTQ  = 14
             SP%  = 71.4
             SJW  = 2
             Err% = 0
             */
            
#if (DEBUGMODE==1)
            Serial.println(F("Speed = 47.619Kbps"));
#endif
            break;
            
        case 1:
            mcp2515_write_register(CNF1,0x00);
            mcp2515_write_register(CNF2,0x90);
            mcp2515_write_register(CNF3,0x02);
#if (DEBUGMODE==1)
            Serial.println(F("Speed = 1Mbps"));
#endif
            break;
            
        case 500:
            mcp2515_write_register(CNF1,0x01);
            mcp2515_write_register(CNF2,0x90);
            mcp2515_write_register(CNF3,0x02);
#if (DEBUGMODE==1)
            Serial.println(F("Speed = 500Kbps"));
#endif
            break;
            
        case 250:
            mcp2515_write_register(CNF1,0x01);
            mcp2515_write_register(CNF2,0xB8);
            mcp2515_write_register(CNF3,0x05);
#if (DEBUGMODE==1)
            Serial.println(F("Speed = 250Kbps"));
#endif
            break;
            
        case 125:
            mcp2515_write_register(CNF1,0x07);
            mcp2515_write_register(CNF2,0x90);
            mcp2515_write_register(CNF3,0x02);
#if (DEBUGMODE==1)
            Serial.println(F("Speed = 125Kbps"));
#endif
            break;
            
        case 100:
            mcp2515_write_register(CNF1,0x03);
            mcp2515_write_register(CNF2,0xBA);
            mcp2515_write_register(CNF3,0x07);
#if (DEBUGMODE==1)
            Serial.println(F("Speed = 100Kbps"));
#endif
            break;
            
        default:
            mcp2515_write_register(CNF1,0x00);
            mcp2515_write_register(CNF2,0x90);
            mcp2515_write_register(CNF3,0x02);
#if (DEBUGMODE==1)
            Serial.println(F("Speed = Default"));
#endif
            break;
            
    }
    
    
    
    
    //Activate RX Interruption
    mcp2515_write_register(CANINTE,(1<<RX1IE)|(1<<RX0IE)); //The two buffers activated interrupt pin
    
    //Filters
    //Buffer 0: All Messages and Rollover=>If buffer 0 full, send to buffer 1
    mcp2515_write_register(RXB0CTRL,(1<<RXM1)|(1<<RXM0)|(1<<BUKT)); //RXM1 & RXM0 the filter/mask off+Rollover
    //Buffer 1: All Messages
    mcp2515_write_register(RXB1CTRL,(1<<RXM1)|(1<<RXM0)); //RXM1 & RXM0 the filter/mask off
    
    //Clear reception mask bits
    mcp2515_write_register( RXM0SIDH, 0 );
    mcp2515_write_register( RXM0SIDL, 0 );
    mcp2515_write_register( RXM0EID8, 0 );
    mcp2515_write_register( RXM0EID0, 0 );
    mcp2515_write_register( RXM1SIDH, 0 );
    mcp2515_write_register( RXM1SIDL, 0 );
    mcp2515_write_register( RXM1EID8, 0 );
    mcp2515_write_register( RXM1EID0, 0 );
    
    //Turn the LED on the board connected to RX0BF / RX1BF when a msg in the buffer
    mcp2515_write_register( BFPCTRL, 0b00001111 );
    
    //Pass the MCP2515 to normal mode and One Shot Mode 0b00001000
    // OSM switched off
    
    mcp2515_write_register(CANCTRL, 0);
    
    //Initialize buffer
    _CAN_RX_BUFFER.head=0;
    _CAN_RX_BUFFER.tail=0;
    
#if (DEBUGMODE==1)
    Serial.println(F("-- End Constructor Can(uint16_t speed) --"));
#endif
    
    
}
// ----------------------------------------------------------------------------
/*
 Name:send(message)
 Parameters(type):
	message(*mesCAN):message to be sent
 Description:
	It sends through the bus the message passed by reference
 Returns:
	0xFF: if 2515's tx-buffer is full
	0x01: if when 2515's TxBuffer[0] used
	0x02: if when 2515's TxBuffer[1] used
	0x04: if when 2515's TxBuffer[2] used
 Example:
	uint8_t count = 0;
	while(CAN.send(!CAN_TxMsg)==0xFF && count < 15);
 
 */
uint8_t CANClass::send(msgCAN *message)
{
    
    
#if (DEBUGMODE==1)
    Serial.println(F("-- uint8_t CANClass::send(msgCAN *message) --"));
#endif
    
    uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
    
    /* Statusbyte:
     *
     * Bit	Function
     *  2	TXB0CNTRL.TXREQ
     *  4	TXB1CNTRL.TXREQ
     *  6	TXB2CNTRL.TXREQ
     */
    uint8_t address;
    uint8_t t;
    
    if (bit_is_clear(status, 2)) {
        address = 0x00;
    }
    else if (bit_is_clear(status, 4)) {
        address = 0x02;
    }
    else if (bit_is_clear(status, 6)) {
        address = 0x04;
    }
    else {
        // all buffer used => could not send message
        return 0xFF;
    }
    
    RESET(MCP2515_CS);
    spi_putc(SPI_WRITE_TX | address);
    
    spi_putc(message->id >> 3);
    spi_putc(message->id << 5);
    
    spi_putc(0);
    spi_putc(0);
    
    uint8_t length = message->header.length & 0x0f;
    
    if (message->header.rtr) {
        // a rtr-frame has a length, but contains no data
        spi_putc((1<<RTR) | length);
    }
    else {
        // set message length
        spi_putc(length);
        
        // data
        for (t=0;t<length;t++) {
            spi_putc(message->data[t]);
        }
    }
    SET(MCP2515_CS);
    
    _delay_us(1);
    
    // send message
    RESET(MCP2515_CS);
    address = (address == 0) ? 1 : address;
    spi_putc(SPI_RTS | address);
    SET(MCP2515_CS);
    
    
#if (DEBUGMODE==1)
    Serial.println(F("-- END uint8_t CANClass::send(msgCAN *message) --"));
#endif
    
    
    
    return address;
}
// ----------------------------------------------------------------------------
/*
 Name:ReadFromDevice(message)
 Parameters(type):
	message(*msgCAN): Menssage passed by reference to be filled with the new
 message coming fron the 2515 drvier
 Description:
	Receives by parameter a struct msgCAN
 Returns(uint8_t):
	last 3 bits of 2515's status. More in the
 Example:
	if(CAN.ReadFromDevice(&message))
	{
	}
 */
uint8_t CANClass::ReadFromDevice(msgCAN *message)
{
    
    
#if (DEBUGMODE==1)
    Serial.println(F("-- START uint8_t ReadFromDevice(msgCAN *message) --"));
#endif
    
    //	static uint8_t previousBuffer;
    
    // read status
    uint8_t status = mcp2515_read_status(SPI_RX_STATUS);
    uint8_t addr;
    uint8_t t;
    
#if (DEBUGMODE==1)
    Serial.print(F("MCP2515 Status="));
    Serial.println(status,BIN);
    Serial.print(F("Mask to check Buffer="));
    Serial.println( ((status & 0b11000000)>>6)&0b00000011,BIN);
#endif
    
    /* This piece of code sometimes causes us to read from the wrong buffer
     
     if ( (((status & 0b11000000)>>6)&0b00000011) >2 )
     {
     addr=SPI_READ_RX | (previousBuffer++ & 0x01)<<2;
     
     #if (DEBUGMODE==1)
     Serial.println("Dos buffer con datos");
     Serial.print("addr=");
     Serial.println(addr,HEX);
     Serial.print("previousBuffer=");
     Serial.println(previousBuffer,DEC);
     
     #endif
     }
     else
     */
    if (bit_is_set(status,6))
    {
        // message in buffer 0
        addr = SPI_READ_RX;
        
#if (DEBUGMODE==1)
        Serial.println(F("Read From Buffer 0"));
        Serial.print(F("addr="));
        Serial.println(addr,HEX);
#endif
    }
    else if (bit_is_set(status,7))
    {
        // message in buffer 1
        addr = SPI_READ_RX | 0x04;
        
#if (DEBUGMODE==1)
        Serial.println(F("Read From Buffer 1"));
        Serial.print(F("addr="));
        Serial.println(addr,HEX);
#endif
    }
    else {
        // Error: no message available
        return 0;
    }
    
    RESET(MCP2515_CS);
    spi_putc(addr);
    
    // read id
    message->id  = (uint16_t) spi_putc(0xff) << 3;
    message->id |=            spi_putc(0xff) >> 5;
    
    spi_putc(0xff);
    spi_putc(0xff);
    
    // read DLC
    uint8_t length = spi_putc(0xff) & 0x0f;
    
    message->header.length = length;
    message->header.rtr = (bit_is_set(status, 3)) ? 1 : 0;
    
    // read data
    for (t=0;t<length;t++) {
        message->data[t] = spi_putc(0xff);
    }
    SET(MCP2515_CS);
    
    /*
     // clear interrupt flag
     if (bit_is_set(status, 6)) {
     mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
     }
     else {
     mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
     }
     */
    
    
    
#if (DEBUGMODE==1)
    Serial.print(F("Return = "));
    Serial.println((status & 0x07) + 1,DEC);
    Serial.println("-- END uint8_t Can::ReadFromDevice(msgCAN *message) --");
#endif
    
    
    return (status & 0x07) + 1;
    
    
    
}
// ----------------------------------------------------------------------------
/*
 Name: CheckNew()
 Parameters(type):
	None
 Description:
	Polls interrupt bit
 Returns:
	0: if there is not messages waiting in the converter
	1: if there is
 Example:
	if (CAN.ChekNew())
 
 */
uint8_t CANClass::CheckNew(void)
{
    return (!IS_SET(MCP2515_INT));
}
// ----------------------------------------------------------------------------
/*
 Name: SetMode(mode)
 
 Parameters(type):
	uint8_t mode
	
 Description:
	The MCP2515 has five modes of operation. This function configure:
 1. Listen-only mode
 2. Loopback mode
 3. Sleep mode
 4. Normal mode
	
 Returns:
	Nothing
	
 Example:
	
 */
void CANClass::SetMode(uint8_t mode)
{
    uint8_t reg = 0;
    
    if (mode == LISTEN_ONLY_MODE) {
        reg = (0<<REQOP2)|(1<<REQOP1)|(1<<REQOP0);
    }
    else if (mode == LOOPBACK_MODE) {
        reg = (0<<REQOP2)|(1<<REQOP1)|(0<<REQOP0);
    }
    else if (mode == SLEEP_MODE) {
        reg = (0<<REQOP2)|(0<<REQOP1)|(1<<REQOP0);
    }
    else if (mode == NORMAL_MODE) {
        reg = (0<<REQOP2)|(0<<REQOP1)|(0<<REQOP0);
    }
    
    // Set the new mode
    mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), reg);
    // Wait until the mode has been changed
    while ((mcp2515_read_register(CANSTAT) & 0xe0) != reg) {
        // wait for the new mode to become active
    }
    
}
// ----------------------------------------------------------------------------
/*
 Name: SetFilter
 
 Parameters(type):
	uint8_t mode
	
 Description:
 
	
 Returns:
	Nothing
	
 Example:
	
 */
void CANClass::SetFilters(uint16_t *Filters,uint16_t *Masks)
{
    
#if (DEBUGMODE==1)
    Serial.println(F("-- void CANClass::SetFilters(uint16_t *Filters) --"));
#endif
    
    
    //Mask=0xE0=0b11100000  (bits 7-5)
    //Set Config Mode=100 (bits 7-5)
    mcp2515_bit_modify(CANCTRL,0xE0,(1<<REQOP2));
    
    //Wait until changed to Config Mode
    while ((mcp2515_read_register(CANSTAT) & 0xE0) != (1<<REQOP2));
    
    //Buffer RXB0 => Filter 0-1 & Mask 0
    mcp2515_write_register( RXF0SIDH, (uint8_t)(Filters[0]>>3) );
    mcp2515_write_register( RXF0SIDL, (uint8_t)(Filters[0]<<5) );
    
    mcp2515_write_register( RXF1SIDH, (uint8_t)(Filters[1]>>3) );
    mcp2515_write_register( RXF1SIDL, (uint8_t)(Filters[1]<<5) );
    
    //Mask 0
    mcp2515_write_register( RXM0SIDH, (uint8_t)(Masks[0]>>3) );
    mcp2515_write_register( RXM0SIDL, (uint8_t)(Masks[0]<<5) );
    //---------------------------------------------------------------
    
    //Buffer RXB1 => Filter 2-5 & Mask 1
    mcp2515_write_register( RXF2SIDH, (uint8_t)(Filters[2]>>3) );
    mcp2515_write_register( RXF2SIDL, (uint8_t)(Filters[2]<<5) );
    
    mcp2515_write_register( RXF3SIDH, (uint8_t)(Filters[3]>>3) );
    mcp2515_write_register( RXF3SIDL, (uint8_t)(Filters[3]<<5) );
    
    mcp2515_write_register( RXF4SIDH, (uint8_t)(Filters[4]>>3) );
    mcp2515_write_register( RXF4SIDL, (uint8_t)(Filters[4]<<5) );
    
    mcp2515_write_register( RXF5SIDH, (uint8_t)(Filters[5]>>3) );
    mcp2515_write_register( RXF5SIDL, (uint8_t)(Filters[5]<<5) );
    
    // Mask1
    mcp2515_write_register( RXM1SIDH, (uint8_t)(Masks[1]>>3) );
    mcp2515_write_register( RXM1SIDL, (uint8_t)(Masks[1]<<5) );
    
    
    //Buffer configuration
    //Bufer 0
    mcp2515_write_register(RXB0CTRL,(0<<RXM1)|(1<<RXM0)|(1<<BUKT));
    //Bufer 1
    mcp2515_write_register(RXB1CTRL,(0<<RXM1)|(1<<RXM0));
    
    
#if (DEBUGMODE==1)
    Serial.print(F("Filter 0 = "));
    Serial.print(Filters[0],BIN);
    Serial.print(F("-"));
    Serial.println(Filters[0],HEX);
    
    Serial.print(F("Filter 1 = "));
    Serial.print(Filters[1],BIN);
    Serial.print(F("-"));
    Serial.println(Filters[1],HEX);
    
    Serial.print(F("Filter 2 = "));
    Serial.print(Filters[2],BIN);
    Serial.print(F("-"));
    Serial.println(Filters[2],HEX);
    
    Serial.print(F("Filter 3 = "));
    Serial.print(Filters[3],BIN);
    Serial.print(F("-"));
    Serial.println(Filters[3],HEX);
    
    Serial.print(F("Filter 4 = "));
    Serial.print(Filters[4],BIN);
    Serial.print(F("-"));
    Serial.println(Filters[4],HEX);
    
    Serial.print(F("Filter 5 = "));
    Serial.print(Filters[5],BIN);
    Serial.print(F("-"));
    Serial.println(Filters[5],HEX);
    
    Serial.print(F("Mask 0 = "));
    Serial.print(Masks[0],BIN);
    Serial.print(F("-"));
    Serial.println(Masks[0],HEX);
    
    Serial.print(F("Mask 1 = "));
    Serial.print(Masks[1],BIN);
    Serial.print(F("-"));
    Serial.println(Masks[1],HEX);
    
    Serial.print(F("RXB0CTRL = "));
    Serial.println(mcp2515_read_register(RXB0CTRL),BIN);
    Serial.print(F("RXB1CTRL = "));
    Serial.println(mcp2515_read_register(RXB1CTRL),BIN);
    Serial.println(F("----------------------"));
    
    Serial.print(F("RXF0SIDH = "));
    Serial.println(mcp2515_read_register(RXF0SIDH),BIN);
    Serial.print(F("RXF0SIDL = "));
    Serial.println(mcp2515_read_register(RXF0SIDL),BIN);
    Serial.print(F("RXF1SIDH = "));
    Serial.println(mcp2515_read_register(RXF1SIDH),BIN);
    Serial.print(F("RXF1SIDL = "));
    Serial.println(mcp2515_read_register(RXF1SIDL),BIN);
    Serial.print(F("RXF2SIDH = "));
    Serial.println(mcp2515_read_register(RXF2SIDH),BIN);
    Serial.print(F("RXF2SIDL = "));
    Serial.println(mcp2515_read_register(RXF2SIDL),BIN);
    Serial.print(F("RXF3SIDH = "));
    Serial.println(mcp2515_read_register(RXF3SIDH),BIN);
    Serial.print(F("RXF3SIDL = "));
    Serial.println(mcp2515_read_register(RXF3SIDL),BIN);
    Serial.print(F("RXF4SIDH = "));
    Serial.println(mcp2515_read_register(RXF4SIDH),BIN);
    Serial.print(F("RXF4SIDL = "));
    Serial.println(mcp2515_read_register(RXF4SIDL),BIN);
    Serial.print(F("RXF5SIDH = "));
    Serial.println(mcp2515_read_register(RXF5SIDH),BIN);
    Serial.print(F("RXF5SIDL = "));
    Serial.println(mcp2515_read_register(RXF5SIDL),BIN);
    Serial.println(("----------------------"));
    
    Serial.print(F("RXM0SIDH = "));
    Serial.println(mcp2515_read_register(RXM0SIDH),BIN);
    Serial.print(F("RXM0SIDL = "));
    Serial.println(mcp2515_read_register(RXM0SIDL),BIN);
    Serial.print(F("RXM1SIDH = "));
    Serial.println(mcp2515_read_register(RXM1SIDH),BIN);
    Serial.print(F("RXM1SIDL = "));
    Serial.println(mcp2515_read_register(RXM1SIDL),BIN);
    
    
    
#endif
    
    //Normal Operation Mode
    mcp2515_bit_modify(CANCTRL,0xE0,0);
    
#if (DEBUGMODE==1)
    Serial.println(F("-- END void CANClass::SetFilters(uint16_t *Filters) --"));
#endif
    
    
}
// ----------------------------------------------------------------------------






/******************************************************************************
 * PRIVATE METHODS
 ******************************************************************************/

// -------------------------------------------------------------------------
uint8_t CANClass::spi_putc( uint8_t data )
{
    // put byte in send-buffer
    SPDR = data;
    
    // wait until byte was send
    while( !( SPSR & (1<<SPIF) ) );
    
    return SPDR;
}

// -------------------------------------------------------------------------
void CANClass::mcp2515_write_register( uint8_t adress, uint8_t data )
{
    RESET(MCP2515_CS);
    
    spi_putc(SPI_WRITE);
    spi_putc(adress);
    spi_putc(data);
    
    SET(MCP2515_CS);
}
// ----------------------------------------------------------------------------
uint8_t CANClass::mcp2515_read_status(uint8_t type)
{
    uint8_t data;
    
    RESET(MCP2515_CS);
    
    spi_putc(type);
    data = spi_putc(0xff);
    
    SET(MCP2515_CS);
    
    return data;
}
// -------------------------------------------------------------------------
void CANClass::mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
    RESET(MCP2515_CS);
    
    spi_putc(SPI_BIT_MODIFY);
    spi_putc(adress);
    spi_putc(mask);
    spi_putc(data);
    
    SET(MCP2515_CS);
}
// ----------------------------------------------------------------------------
uint8_t CANClass::mcp2515_check_free_buffer(void)
{
    uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
    
    if ((status & 0x54) == 0x54) {
        // all buffers used
        return false;
    }
    
    return true;
}
// ----------------------------------------------------------------------------
uint8_t CANClass::mcp2515_read_register(uint8_t adress)
{
    uint8_t data;
    
    RESET(MCP2515_CS);
    
    spi_putc(SPI_READ);
    spi_putc(adress);
    
    data = spi_putc(0xff);
    
    SET(MCP2515_CS);
    
    return data;
}
// ----------------------------------------------------------------------------
/*
 Name: store(message)
 Parameters(type):
	message(msgCAN*); Message to sotre in circular buffer
 Description:
	it stores the message given in the circular buffer
 Returns:
	None
 Example:
	CAN.store(&CAN_RxMsg);
 
 */
void CANClass::store(msgCAN *message)
{
    
    int i=(_CAN_RX_BUFFER.head+1)%RX_CAN_BUFFER_SIZE;
    
    //Must be stored in the buffer, as long as're from behind the tail
    //We see example if the buffer size is 10:
    //The rest of 1/10=1, meter data CAN_RX_BUFFER.buffer[0]; i=1
    //The rest of 2/10=2, meter data CAN_RX_BUFFER.buffer[1]; i=2
    //...
    //The rest of 10/10=0, BUFFER FULL. Does nothing.
    
    if (i!=_CAN_RX_BUFFER.tail)
    {
        _CAN_RX_BUFFER.buffer[_CAN_RX_BUFFER.head]=*message;
        //increase the current position of the circular buffer
        _CAN_RX_BUFFER.head=i;
        
    }else{
        
        //todo
        
    }
    
    
}
// ----------------------------------------------------------------------------
/*
 Name:available()
 Parameters(type):
	None
 Returns
	0 if there is not available messages in bus
	>0 : How many
 Description:
	Returns how many messages are available in the circular buffer
 Example:
	if(CAN.available() > 0)
	{
 
	}
 
 */
uint8_t CANClass::available(void)
{
    return ((RX_CAN_BUFFER_SIZE+_CAN_RX_BUFFER.head-_CAN_RX_BUFFER.tail)%RX_CAN_BUFFER_SIZE);
}
// ----------------------------------------------------------------------------
/*
 Name:read(message)
 Parameters(type):
	message(*msgCAN): Message passed by reference t be filled with a stored msg
 Description:
	it pops a message from the circular buffer
 Example:
	read(&CAN_TxMsg);
	if(CAN_TxMsg.id > 0)
	{
	}
 
 */
void CANClass::read(msgCAN *message)
{
    
    if (_CAN_RX_BUFFER.head==_CAN_RX_BUFFER.tail)
    {
        //It means no data
        message->id=0;
        
    }else{
        *message=_CAN_RX_BUFFER.buffer[_CAN_RX_BUFFER.tail];
        _CAN_RX_BUFFER.tail=(_CAN_RX_BUFFER.tail+1)%RX_CAN_BUFFER_SIZE;
    }
    
}
// ----------------------------------------------------------------------------