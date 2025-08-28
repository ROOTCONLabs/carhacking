// code originally from https://www.synacktiv.com/en/publications/how-to-voltage-fault-injection
// with minor modifications from @shipcod3
// compatible with Arduino devices like Uno, Nano, ATMega 2560, etc

String PASSWORD = "P@s5w0rd890!@"; // will be printed at the GLITCH

bool checkPass(String buffer) {
  for (int i = 0; i < PASSWORD.length(); i++) {
    if (buffer[i] != PASSWORD[i]) {
      return false;
    }
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Password:");
}

void loop() {
  if (Serial.available() > 0) {
    char pass[PASSWORD.length()];
    Serial.readBytesUntil('\n', pass, PASSWORD.length());
    bool correct = checkPass(pass);
      if (correct) {
      Serial.println("Logged in!");
      Serial.println(PASSWORD);
      Serial.flush(); 
      exit(0);
    } else {
      Serial.println("Incorrect password.");
      Serial.println("Password:");
    }
  }
}
