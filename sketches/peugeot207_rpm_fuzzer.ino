// ####################################################################
// # Peugeot 207 Instrument Cluster that allows you to go vroom vroom # 
// # RPM PoC Fuzzer for n00bz                                         #
// # author: @shipcod3                                                # 
// # ROOTCON Car Hacking Village                                      #
// # greetz to semrpix, mogul, mintynet and eman0n for the support    #
// ####################################################################
// special love to @carhackingvillage and carfucar

#include <mcp_can.h>
#include <SPI.h>

#define FUNCTIONAL_ID 0x0B6 /* RPM ID*/

// CAN TX Variables
unsigned long prevTx = 0;
unsigned int invlTx = 1000;

// CAN RX Variables
unsigned long rxID;
byte dlc;
byte rxBuf[8];

// CAN Interrupt and Chip Select Pins
#define CAN0_INT 2                              /* Set INT to pin 2 (This rarely changes)   */
MCP_CAN CAN0(10);                                /* Set CS to pin 10 (Old shields use pin 10) */
byte counter = 0;

void setup(){

  Serial.begin(115200);
  while(!Serial);
 
  // Initialize MCP2515 running at 8MHz with a baudrate of 125kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_STDEXT, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("\nMCP2515 Initialized Successfully!");
  else{
    Serial.println("Error Initializing MCP2515... Permanent failure!  Check your code & connections");
    while(1);
  }

  CAN0.setMode(MCP_NORMAL);                      // Set operation mode to normal so the MCP2515 sends acks to received data.

  // Having problems?  ======================================================
  // If you are not receiving any messages, uncomment the setMode line below
  // to test the wiring between the Ardunio and the protocol controller.
  // The message that this sketch sends should be instantly received.
  // ========================================================================
  //CAN0.setMode(MCP_LOOPBACK);

  pinMode(CAN0_INT, INPUT);                          // Configuring pin for /INT input
 
  Serial.println(" ####################################################################");
  Serial.println(" # Peugeot 207 Instrument Cluster that allows you to go vroom vroom #");
  Serial.println(" # RPM PoC Fuzzer for n00bz                                         #");
  Serial.println(" # author: @shipcod3                                                #"); 
  Serial.println(" # ROOTCON Car Hacking Village                                      #");
  Serial.println(" # greetz to semrpix, mogul, mintynet and eman0n for the support    #");
  Serial.println(" ####################################################################");
}

void loop(){

  if(!digitalRead(CAN0_INT)){                         // If CAN0_INT pin is low, read receive buffer
 
    CAN0.readMsgBuf(&rxID, &dlc, rxBuf);             // Get CAN data - removing this will give you an error in sending message
  }
 
  if((millis() - prevTx) >= invlTx){
    prevTx = millis();
    byte txData[8] = {counter, 0x00, counter};
    if(CAN0.sendMsgBuf(FUNCTIONAL_ID, 8, txData) == CAN_OK){
        counter++;
        if (counter > 160) {
        counter = 0;
        }
      Serial.println("Message Sent Successfully!");
    } else {
      Serial.println("Error Sending Message...");
    }
  }
}

/*********************************************************************************************************
  EOF
*********************************************************************************************************/
