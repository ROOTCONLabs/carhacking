# This code should help you detect ECU hard resets on the CAN bus using an Arduino Nano and the MCP2515 module.
# Simple PoC for Anomaly Detection System
# @shipcod3

#include <SPI.h>
#include <mcp_can.h>

#define CAN0_INT 2          // Set INT to pin 2
const int SPI_CS_PIN = 10; // Chip Select pin for MCP2515
MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(115200);

  // Initialize MCP2515 CAN controller
  if (CAN.begin(CAN_500KBPS) != CAN_OK) {
    Serial.println("Error initializing MCP2515");
    while (1);
  }

  // Enable receive buffer 0
  CAN.init_Mask(0, 0, 0x7FF); // Filter any incoming message
  CAN.init_Filt(0, 0, 0x7FF); // Filter any incoming message
  CAN.init_Filt(1, 0, 0x7FF); // Filter any incoming message
  CAN.setMode(MCP_NORMAL);    // Set the MCP2515 to normal mode
  Serial.println("MCP2515 initialized");
}

void loop() {
  unsigned char len = 0;
  unsigned char buf[8];

  if (CAN.checkReceive()) {
    CAN.readMsgBuf(&len, buf);
    unsigned long canId = CAN.getCanId();

    if (canId == 0x7DF && buf[0] == 0x02 && buf[1] == 0x11 && buf[2] == 0x01) {
      Serial.println("ECU hard reset detected!");
    }
  }
}
