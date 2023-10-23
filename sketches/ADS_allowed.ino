# allowed IDSs
# @shipcod3

#include <mcp2515.h>
#include <SPI.h>

// Define the MCP2515 module settings
#define MCP2515_CS_PIN 10
#define MCP2515_INT_PIN 2

MCP2515 mcp2515(MCP2515_CS_PIN);

// Define the allowed CAN IDs and their corresponding data
unsigned long allowed_ids[] = {0x7DF};
byte allowed_data[][8] = {{0x02, 0x11, 0x01}};

void setup() {
  Serial.begin(9600);
  
  // Initialize the MCP2515 module
  if (mcp2515.begin(MCP_ANY, MCP2515_INT_PIN) != MCP2515::CAN_OK) {
    Serial.println("Error initializing MCP2515...");
    while (1);
  }

  // Set up the MCP2515 module for normal operation
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop() {
  // Check for received CAN messages
  if (mcp2515.readMessage()) {
    // Check if the received message's ID is in the allowed_ids array
    for (int i = 0; i < sizeof(allowed_ids) / sizeof(allowed_ids[0]); i++) {
      if (mcp2515.can_id == allowed_ids[i]) {
        // Check if the received data matches the allowed data
        bool dataMatch = true;
        for (int j = 0; j < 8; j++) {
          if (mcp2515.data[j] != allowed_data[i][j]) {
            dataMatch = false;
            break;
          }
        }

        if (dataMatch) {
          Serial.println("Allowed message received:");
          Serial.print("ID: 0x");
          Serial.print(mcp2515.can_id, HEX);
          Serial.print(" Data: ");
          for (int j = 0; j < 8; j++) {
            Serial.print(mcp2515.data[j], HEX);
            Serial.print(" ");
          }
          Serial.println();
        } else {
          Serial.println("Anomaly detected in message!");
        }
      }
    }
  }
}
