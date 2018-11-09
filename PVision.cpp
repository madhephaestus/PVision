// Example of using the PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com
/******************************************************************************
* Includes
******************************************************************************/
#include "PVision.h"
#include <myWire->h>

/******************************************************************************
* Private methods
******************************************************************************/
void PVision::Write_2bytes(byte d1, byte d2)
{
    myWire->beginTransmission(IRslaveAddress);
    myWire->write(d1); myWire->write(d2);
    myWire->endTransmission();
}


/******************************************************************************
* Constructor
******************************************************************************/
PVision::PVision()
{
	Blob1.number = 1;
	Blob2.number = 2;
	Blob3.number = 3;
	Blob4.number = 4;
	blobcount=0;
	IRslaveAddress=0;
	IRsensorAddress=0;
	s=0;
	i=0;
	myWire=NULL;
}

/******************************************************************************
* Public methods
******************************************************************************/
// init the PVision sensor
void PVision::init (TwoWire * inWire)
{
	myWire=inWire;
    IRsensorAddress = 0xB0;
    IRslaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI

    myWire->begin();
    // IR sensor initialize
    Write_2bytes(0x30,0x01); delay(10);
    Write_2bytes(0x30,0x08); delay(10);
    Write_2bytes(0x06,0x90); delay(10);
    Write_2bytes(0x08,0xC0); delay(10);
    Write_2bytes(0x1A,0x40); delay(10);
    Write_2bytes(0x33,0x33); delay(10);
    delay(100);
}

byte PVision::read()
{
    //IR sensor read
    myWire->beginTransmission(IRslaveAddress);
    myWire->write(0x36);
    myWire->endTransmission();

    myWire->requestFrom(IRslaveAddress, 16);        // Request the 2 byte heading (MSB comes first)
    for (i=0;i<16;i++)
    {
       data_buf[i]=0;
    }

    i=0;

    while(myWire->available() && i < 16)
    {
        data_buf[i] = myWire->read();
        i++;
    }

    blobcount = 0;

    Blob1.X = data_buf[1];
    Blob1.Y = data_buf[2];
    s   = data_buf[3];
    Blob1.X += (s & 0x30) <<4;
    Blob1.Y += (s & 0xC0) <<2;
    Blob1.Size = (s & 0x0F);

    // At the moment we're using the size of the blob to determine if one is detected, either X,Y, or size could be used.
    blobcount |= (Blob1.Size < 15)? BLOB1 : 0;

    Blob2.X = data_buf[4];
    Blob2.Y = data_buf[5];
    s   = data_buf[6];
    Blob2.X += (s & 0x30) <<4;
    Blob2.Y += (s & 0xC0) <<2;
    Blob2.Size = (s & 0x0F);

    blobcount |= (Blob2.Size < 15)? BLOB2 : 0;

    Blob3.X = data_buf[7];
    Blob3.Y = data_buf[8];
    s   = data_buf[9];
    Blob3.X += (s & 0x30) <<4;
    Blob3.Y += (s & 0xC0) <<2;
    Blob3.Size = (s & 0x0F);

    blobcount |= (Blob3.Size < 15)? BLOB3 : 0;

    Blob4.X = data_buf[10];
    Blob4.Y = data_buf[11];
    s   = data_buf[12];
    Blob4.X += (s & 0x30) <<4;
    Blob4.Y += (s & 0xC0) <<2;
    Blob4.Size = (s & 0x0F);

    blobcount |= (Blob4.Size < 15)? BLOB4 : 0;

    return blobcount;
}

