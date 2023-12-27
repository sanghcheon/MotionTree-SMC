#include <SPI.h>
#include "df_can.h"
// "df_can.h" can be downloaded from below
// https://wiki.dfrobot.com/CAN-BUS_Shield_V2__SKU__DFR0370_

const int SPI_CS_PIN = 10;
MCPCAN CAN(SPI_CS_PIN);

unsigned char len = 0;
unsigned char buf[8];
char str[20];

void setup()
{
  Serial.begin(115200);
  int count = 50;
  do {
    CAN.init();
    if (CAN_OK == CAN.begin(CAN_1000KBPS))                  // init can bus : baudrate = 1000k
    {
      Serial.println("CAN BUS Shield init ok");
      break;
    }
    else
    {
      Serial.println("CAN BUS Shield init fail");
      delay(100);
    }

  } while (count--);

  
  
  attachInterrupt(0, MCP2515_ISR, FALLING);
}

void loop()
{
  mode2(0x001, 20000, 2, 0.5);  // CAN_ID, TARGET_POSITION, KP, KD
  delay(5000);

  mode2(0x001, 0, 2, 0.5);      // CAN_ID, TARGET_POSITION, KP, KD
  delay(5000);
}

void mode2(uint16_t can_id, int32_t pos, float kp, float kd)
{
  uint8_t data[8];
  data[0] = 0x02;
  data[1] = 0x01;
  data[2] = (uint8_t) pos;
  data[3] = (uint8_t) (pos >> 8);
  data[4] = (uint8_t) (pos >> 16);
  data[5] = (uint8_t) (pos >> 24);
  data[6] = (uint8_t) (kp * 10);
  data[7] = (uint8_t) (kd * 10);
  
  CAN.sendMsgBuf(can_id, 0, 8, data);

  Serial.print(F("<< CAN Tx:  0x00")); Serial.print(can_id, HEX);
  hexPrint(data[0]);
  hexPrint(data[1]);
  hexPrint(data[2]);
  hexPrint(data[3]);
  hexPrint(data[4]);
  hexPrint(data[5]);
  hexPrint(data[6]);
  hexPrint(data[7]);
  Serial.println();
}

void MCP2515_ISR()
{
  while (CAN_MSGAVAIL == CAN.checkReceive())
  {
    buf[8] = {0, };
    CAN.readMsgBuf(&len, buf);
    Serial.print(F(">> CAN Rx:  0x")); Serial.print(CAN.getCanId(), HEX);

    // print the data
    for (int i = 0; i < len; i++)
    {
      hexPrint(buf[i]);
    }
    Serial.println();
  }
}

void hexPrint(uint8_t data)
{
  if (data <= 15)
  {
    Serial.print(F("  0x0")); Serial.print(data, HEX);
  }
  else
  {
    Serial.print(F("  0x")); Serial.print(data, HEX);
  }
}
