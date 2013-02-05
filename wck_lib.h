/*=============================================================================*/
/* Example Program */
/* Motion Explanation : If given a force, wCK rotates to the direction of the force and stops */
/* Motion Logic : Read wCK’s position feedback to decide rotation direction and start movement */
/* when there’s any change in position */
/*=============================================================================*/
#define HEADER 0xff
#define NULL 0
#define ROTATE_CCW 3
#define ROTATE_CW 4
/* Start of Function Prototype-----------------------------------------------------*/
/*------------------ Function relating to serial communication ----------------*/
void SendByte(int data); // Send 1 Byte to serial port
void GetByte(int timeout); // Receive 1 Byte from serial port
/*------------------Basic Functions relating to wCK module--------------------------*/
void SendOperCommand(int Data1, int Data2);
void SendSetCommand(int Data1, int Data2, int Data3, int Data4);
int PosSend(int ServoID, int SpeedLevel, int Position);
int PosRead(int ServoID);
int ActDown(int ServoID);
int PowerDown(void);
int Rotation360(int ServoID, int SpeedLevel, int RotationDir);
void SyncPosSend(int LastID, int SpeedLevel, int *TargetArray, int Index);
int BaudrateSet(int ServoID, int NewBaud);
int GainSet(int ServoID, int *NewPgain, int *NewDgain);
int IdSet(int ServoID, int NewId);
int GainRead(int ServoID, int *Pgain, int *Dgain);
int ResolSet(int ServoID, int NewResol);
int ResolRead(int ServoID);
int OverCTSet(int ServoID, int NewOverCT);
int OverCTRead(int ServoID);
int BoundSet(int ServoID, int *NewLBound, int *NewUBound);
int BoundRead(int ServoID, int *LBound, int *UBound);
/*------------------------------------------------------- End of Function Prototype */
