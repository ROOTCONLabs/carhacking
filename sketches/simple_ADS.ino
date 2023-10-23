#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN(10); // CS pin connected to D10

unsigned long allowed_ids[] = {0x7DF, 0x123, 0x456};

void setup() {
  Serial.begin(9600);
  if (CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized");
    CAN.init_Mask(0, 0, 0x7FF); // Filter to allow all IDs
    CAN.init_Filt(0, 0, 0x7FF); // Mask to allow all IDs
  } else {
    Serial.println("Error initializing MCP2515...");
  }
}

void loop() {
  unsigned long canId;
  byte len = 0;
  byte buffer[8];

  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    CAN.readMsgBuf(&len, buffer);
    canId = CAN.getCanId();
    Serial.print("Received message ID: 0x");
    Serial.println(canId, HEX);

    // Check for anomalies in the received message ID
    bool isAllowed = false;
    for (int i = 0; i < sizeof(allowed_ids) / sizeof(allowed_ids[0]); i++) {
      if (canId == allowed_ids[i]) {
        isAllowed = true;
        break;
      }
    }

    if (!isAllowed) {
      Serial.println("Anomaly detected in message ID!");
      // Handle the anomaly here, e.g., log it or trigger an alert
    }
  }
}
