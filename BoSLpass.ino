#include <SoftwareSerial.h>

#define SIMCOM_7000 // SIM7000A/C/E/G
#define BAUDRATE 9600 // MUST be below 19200 (for stability) but 9600 is more stable

// For SIM7000 BoSL board
#define PWRKEY 4
#define DTR 5 // Connect with solder jumper
#define BOSL_RX 9// Microcontroller RX
#define BOSL_TX 8 // Microcontroller TX


SoftwareSerial simCom = SoftwareSerial(BOSL_RX, BOSL_TX);
 
//Declaring variables to store results
int a;
unsigned long previous;
uint8_t x;
bool answer;
char response[100];
char cmd[100];
String msg;



void setup() {
  bool answer;
  Serial.begin(BAUDRATE);
  simCom.begin(BAUDRATE);

  Serial.println("initialising sim");
 

   simOn();
   simInit();
 // answer = sendATcmd(F("AT+CGNSINF"), "+CGNSINF: 1,1",1000);

}

    uint8_t i = 0;
    bool Scmd = 0;
    
void loop() {
  
  i = 0;
  Scmd = 0;
  memset(cmd, '\0', 100);
  delay(100);
  msg = "";
  
  
  while(Serial.available() != 0){
      Scmd = 1;
      cmd[i] += (char)Serial.read();

      i++;
      delay(1);
  }
  
  if (Scmd){
      simCom.write(cmd);
      simCom.println();
      Serial.print("sending: ");
      Serial.println(cmd);
      
      Scmd = 0;
   }
  
  while(simCom.available() != 0){
      Serial.print((char)simCom.read());
  }
   
}



bool sendATcmd(String ATcommand, char* expected_answer1, unsigned int timeout){
    a=0;
    do{a++;
    
    Serial.println(ATcommand);
    answer=0;
    
    previous = 0;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    while( simCom.available() > 0) {
        simCom.read();    // Clean the input buffer
    }
    
    simCom.println(ATcommand);    // Send the AT command 


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(simCom.available() != 0){    
            response[x] = simCom.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
        }    
            
            
        
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < timeout)); 

    if (expected_answer1 == "0"){
                answer = 1;
            }
            
    Serial.println(response);
    
    
    }while(answer == 0 && a < 5);
    
     a = 0;
     return answer;
}



void simInit(){
    
  simOn();
  
      sendATcmd(F("AT+IPR=9600"),"OK",1000);
      
      sendATcmd(F("ATE0"),"OK",1000);
      
      sendATcmd(F("AT&W0"),"OK",1000);
  
}

void simOn() {
	pinMode(PWRKEY, OUTPUT);
	pinMode(BOSL_TX, OUTPUT);
	digitalWrite(BOSL_TX, HIGH);
	pinMode(BOSL_RX, INPUT_PULLUP);


	digitalWrite(PWRKEY, LOW);
	// See spec sheets for your particular module
	delay(100); // For SIM7000

	digitalWrite(PWRKEY, HIGH);
}


///////////////////////////////////////////////////////////////
void simOff() {
	//  TX / RX pins off to save power

	digitalWrite(BOSL_TX, LOW);
	digitalWrite(BOSL_RX, LOW);

	digitalWrite(PWRKEY, LOW);
	// See spec sheets for your particular module
	delay(3000); // For SIM7000
	digitalWrite(PWRKEY, HIGH);
}
