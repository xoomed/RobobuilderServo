#include "Arduino.h"

#define TIME_OUT1 1000
#define TIME_OUT2 1000
#define HEADER 0xff
#define NULL 0
#define ROTATE_CCW 3
#define ROTATE_CW 4

/*------------------ Function relating to serial communication ----------------*/
void SendByte(int data)// Send 1 Byte to serial port
{
  
 Serial.write(data);


}



int GetByte(int timeout)// Receive 1 Byte from serial port
{
  byte retour;
   Serial.setTimeout(timeout);
  if (Serial.available() > 0) {
 
  retour = Serial.read();
  }
  return retour;
}







//////////////////////////////// Definition of Basic Functions ///////////////////////////
/******************************************************************************/
/* Function that sends Operation Command Packet(4 Byte) to wCK module */
/* Input : Data1, Data2 */
/* Output : None */
/******************************************************************************/
void SendOperCommand(int Data1, int Data2)
{
int CheckSum;
CheckSum = (Data1^Data2)&0x7f;
SendByte(HEADER);
SendByte(Data1);
SendByte(Data2);
SendByte(CheckSum);

}
/******************************************************************************/
/* Function that sends Set Command Packet(6 Byte) to wCK module */
/* Input : Data1, Data2, Data3, Data4 */
/* Output : None */
/******************************************************************************/
void SendSetCommand(int Data1, int Data2, int Data3, int Data4)
{
int CheckSum;
CheckSum = (Data1^Data2^Data3^Data4)&0x7f;
SendByte(HEADER);
SendByte(Data1);
SendByte(Data2);
SendByte(Data3);
SendByte(Data4);
SendByte(CheckSum);
}
/*************************************************************************************************/
/* Function that sends Position Move Command to wCK module */
/* Input : ServoID, SpeedLevel, Position */
/* Output : Current */
/*************************************************************************************************/
int PosSend(int ServoID, int SpeedLevel, int Position)
{
int Current;
SendOperCommand((SpeedLevel<<5)|ServoID, Position);
GetByte(TIME_OUT1);
Current = GetByte(TIME_OUT1);
return Current;
}
/************************************************************************************************/
/* Function that sends Position Read Command to wCK module */
/* Input : ServoID */
/* Output : Position */
/************************************************************************************************/
int PosRead(int ServoID)
{
int Position;
SendOperCommand(0xa0|ServoID, NULL);
GetByte(TIME_OUT1);
Position = GetByte(TIME_OUT1);
return Position;
}
/******************************************************************************/
/* Function that sends Passive wCK Command to wCK module */
/* Input : ServoID */
/* Output : Position */
/******************************************************************************/
int ActDown(int ServoID)
{
int Position;
SendOperCommand(0xc0|ServoID, 0x10);
GetByte(TIME_OUT1);
Position = GetByte(TIME_OUT1);
return Position;
}
/*************************************************************************/
/* Function that sends Break wCK Command to wCK module */
/* Input : None */
/* Output : ServoID if succeed, 0xff if fail */
/**************************************************************************/
int PowerDown(void)
{
int ServoID;
SendOperCommand(0xdf, 0x20);
ServoID = GetByte(TIME_OUT1);
GetByte(TIME_OUT1);
if(ServoID<31) return ServoID;
return 0xff; //Receive error
}
/******************************************************************/
/* Function that sends 360 degree Wheel wCK Command */
/* Input : ServoID, SpeedLevel, RotationDir */
/* Return : Rotation Number */
/*****************************************************************/
int Rotation360(int ServoID, int SpeedLevel, int RotationDir)
{
int ServoPos, RotNum;
if(RotationDir==ROTATE_CCW) {
SendOperCommand((6<<5)|ServoID, (ROTATE_CCW<<4)|SpeedLevel);
}
else if(RotationDir==ROTATE_CW) {
SendOperCommand((6<<5)|ServoID, (ROTATE_CW<<4)|SpeedLevel);
}
RotNum = GetByte(TIME_OUT1);
GetByte(TIME_OUT1);
return RotNum;
}
/*****************************************************************************/
/* Function that sends Synchronized Position Move Command to wCK module */
/* Input : LastID, SpeedLevel, *TargetArray, Index */
/* Return : None */
/****************************************************************************/
void SyncPosSend(int LastID, int SpeedLevel, int *TargetArray, int Index)
{
int i;
int CheckSum;
i = 0;
CheckSum = 0;
SendByte(HEADER);
SendByte((SpeedLevel<<5)|0x1f);
SendByte(LastID+1);
while(1) {
if(i>LastID) break;
SendByte(TargetArray[Index*(LastID+1)+i]);
CheckSum = CheckSum ^ TargetArray[Index*(LastID+1)+i];
i++;
}
CheckSum = CheckSum & 0x7f;
SendByte(CheckSum);
}
/********************************************************************/
/* Function that sends Baud rate Set Command to wCK module */
/* Input : ServoID, NewBaud */
/* Return : New Baudrate if succeed, 0xff if fail */
/********************************************************************/
int BaudrateSet(int ServoID, int NewBaud)
{
SendSetCommand((7<<5)|ServoID, 0x08, NewBaud, NewBaud);

GetByte(TIME_OUT2);
if(GetByte(TIME_OUT2)==NewBaud) return NewBaud;
return 0xff;
}
/*********************************************************************/
/* Function that sends Gain Set Command to wCK module */
/* Input : ServoID, *NewPgain, *NewDgain */
/* Return : 1 if succeed, 0 if fail */
/********************************************************************/
int GainSet(int ServoID, int *NewPgain, int *NewDgain)
{
int Data1,Data2;
SendSetCommand((7<<5)|ServoID, 0x09, *NewPgain, *NewDgain);
Data1 = GetByte(TIME_OUT2);
Data2 = GetByte(TIME_OUT2);
if((Data1==*NewPgain) && (Data2==*NewDgain)) return 1;
return 0;
}
/************************************************************/
/* Function that sends ID Set Command to wCK module */
/* Input : ServoID, NewId */
/* Return : New ID if succeed, 0xff if fail */
/***********************************************************/
int IdSet(int ServoID, int NewId)
{
SendSetCommand((7<<5)|ServoID, 0x0C, NewId, NewId);
GetByte(TIME_OUT2);
if(GetByte(TIME_OUT2)==NewId) return NewId;
return 0xff;
}
/******************************************************************/
/* Function that sends Gain Read Command to wCK module */
/* Input : ServoID, *NewPgain, *NewDgain */
/* Return : 1 if succeed, 0 if fail */
/*****************************************************************/
int GainRead(int ServoID, int *Pgain, int *Dgain)
{
SendSetCommand((7<<5)|ServoID, 0x0c, 0, 0);
*Pgain = GetByte(TIME_OUT1);

*Dgain = GetByte(TIME_OUT1);
if((*Pgain>0) && (*Pgain<51) && (*Dgain<101)) return 1;
return 0;
}
/**********************************************************************************/
/* Function that sends Over Load Set Command to wCK module */
/* Input : ServoID, NewOverCT */
/* Return : New Overcurrent Threshold if succeed, 0xff if fail */
/**********************************************************************************/
/*int OverCTSet(int ServoID, int NewOverCT)
{
int Data1;
SendSetCommand((7<<5)|ServoID, 0x0f, NewOverCT, NewOverCT);
sciRxReady(TIME_OUT2);
Data1=sciRxReady(TIME_OUT2);
if(Data1!=0xff) return Data1;
return 0xff;
}
/******************************************************************************/
/* Function that sends Over Load Read Command to wCK module */
/* Input : ServoID */
/* Return : Overcurrent Threshold if succeed, 0xff if fail */
/******************************************************************************/
/*int OverCTRead(int ServoID)
{
int Data1;
SendSetCommand((7<<5)|ServoID, 0x10, 0, 0);
sciRxReady(TIME_OUT1);
Data1=sciRxReady(TIME_OUT1);
if(Data1!=0xff) return Data1;
return 0xff;
}*/
/***********************************************************************/
/* Function that sends Boundary Set Command to wCK module */
/* Input : ServoID, *NewLBound, *NewUBound */
/* Return : 1 if succeed, 0 if fail */
/**********************************************************************/
int BoundSet(int ServoID, int *NewLBound, int *NewUBound)
{
int Data1,Data2;
SendSetCommand((7<<5)|ServoID, 0x11, *NewLBound, *NewUBound);
Data1 = GetByte(TIME_OUT2);
Data2 = GetByte(TIME_OUT2);
if((Data1==*NewLBound) && (Data2==*NewUBound)) return 1;
return 0;
}
/**************************************************************************/
/* Function that sends Boundary Read Command to wCK module */
/* Input : ServoID, *NewLBound, *NewUBound */
/* Return : 1 if succeed, 0 if fail */
/*************************************************************************/
int BoundRead(int ServoID, int *LBound, int *UBound)
{
SendSetCommand((7<<5)|ServoID, 0x12, 0, 0);
*LBound = GetByte(TIME_OUT1);
*UBound = GetByte(TIME_OUT1);
if(*LBound<*UBound) return 1;
return 0;
}
//////////////////////////////// End of Basic Functions Definition /////////////////////////////
