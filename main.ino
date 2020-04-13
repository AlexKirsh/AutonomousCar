//Sonar Sensor init
#include <SR04.h>
#define ECHO_PIN 12
#define TRIG_PIN 13
SR04 sonar_sensor = SR04(ECHO_PIN,TRIG_PIN);
int sonar_distance = 0;
int sonar_readings[1000];
int sonar_index = 0;

//IR Sensor init
#include <SharpIR.h>
#define IRPin A0

/* Model :
  GP2Y0A02YK0F --> 20150
  GP2Y0A21YK0F --> 1080
  GP2Y0A710K0F --> 100500
  GP2YA41SK0F --> 430
*/

#define model 1080
SharpIR ir_sensor = SharpIR(IRPin, model);
int ir_distance;
int ir_readings[1000];
int ir_index = 0;

//DC motor init

#define ENABLE_LM 5
#define FORWARD_LM 6
#define BACKWARD_LM 7

#define ENABLE_RM 10
#define FORWARD_RM 9
#define BACKWARD_RM 8


void setup() {
  
   //Setup DC motors ports
  
    pinMode(ENABLE_RM,OUTPUT);
    pinMode(FORWARD_RM,OUTPUT);
    pinMode(BACKWARD_RM,OUTPUT);
    
    pinMode(ENABLE_LM,OUTPUT);
    pinMode(FORWARD_LM,OUTPUT);
    pinMode(BACKWARD_LM,OUTPUT);

    //Setup bluetooth communication
    Serial1.begin(9600); //Tx1 and Rx1  //Connected to Bluetooth Module HC-05 (Bluetooth 2.0)
	
	Serial.begin(9600);
	
	//Setup LED indicator
	
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	digitalWrite(2,LOW);

}

void loop() {
  //Start connection to PC
 
  if (Serial1.available() > 0) {
    String pc_msg;
    pc_msg = Serial1.readString();
    
    if (pc_msg == "START") {
	  
	  //Initialize timer parameters
	  unsigned long strt_time;
	  unsigned long stop_time;
	  int timer_flag = 1;

      //Loop until PC closes connection
      while (pc_msg != "END") {
		  
		  //LED indicator ON
		digitalWrite(3,HIGH);
		  
		sonar_distance = sonar_sensor.Distance();
		delay(50);

        while (sonar_distance == 0) {
          sonar_distance = sonar_sensor.Distance();  
        }
        
        //If no obstacle in sight, robot may drive
        if (sonar_distance >= 56) {
			
			// Measure starting time
			if (timer_flag) {
				strt_time = millis();
				timer_flag = 0;
			}
			
			//Jump start driving 
		
			analogWrite(ENABLE_RM,110);
			digitalWrite(FORWARD_RM,HIGH);
			digitalWrite(BACKWARD_RM,LOW);
			
			analogWrite(ENABLE_LM,110);
			digitalWrite(FORWARD_LM,HIGH);
			digitalWrite(BACKWARD_LM,LOW);
	  
			delay(500);
			
			//Drive robot forward
			analogWrite(ENABLE_RM,80);
			digitalWrite(FORWARD_RM,HIGH);
			digitalWrite(BACKWARD_RM,LOW);
			
			analogWrite(ENABLE_LM,40);
			digitalWrite(FORWARD_LM,HIGH);
			digitalWrite(BACKWARD_LM,LOW);
          
        }
		
		//LED indicator OFF
	
		digitalWrite(3,LOW);

        while (sonar_distance >= 56) {
			
		//LED indicator ON
		digitalWrite(3,HIGH);

      
		//Collect sensor data
        sonar_distance = sonar_sensor.Distance();
        ir_distance = ir_sensor.distance();
		
		Serial.println("SONAR:");
		Serial.print(sonar_distance);
		Serial.println("IR:");
		Serial.print(ir_distance);


        //Save sensor data
        if (sonar_index < 1000 && ir_index < 1000) {
            sonar_readings[sonar_index] = sonar_distance;
            sonar_index++;
            ir_readings[ir_index] = ir_distance;
            ir_index++;
          }

		//LED indicator OFF
		digitalWrite(3,LOW);
  
		delay(50);
		
		digitalWrite(3,HIGH);
		
		if (sonar_distance == 0) {
			digitalWrite(3,HIGH);
		}
		  
        //If obstacle present, stop robot and send sensor data to PC
          
        if (sonar_distance <= 56) {
            
			//LED indicator ON
			digitalWrite(3,HIGH);
			
			digitalWrite(ENABLE_RM,LOW);
            digitalWrite(ENABLE_LM,LOW);
			
			// Measure stop time
			stop_time = millis();
			timer_flag = 1;
			
            Serial1.print("SONAR:");
            for (int i = 0; i < sonar_index; i++) {
              Serial1.print(sonar_readings[i]);
              Serial1.print(","); 
              sonar_readings[i] = -1;
            }
			sonar_index = 0;
            Serial1.print(";");
            
            Serial1.print("IR:");
            for (int i = 0; i < ir_index; i++) {
              Serial1.print(ir_readings[i]);
              Serial1.print(","); 
              ir_readings[i] = -1;
            }
			ir_index = 0;
            Serial1.print(";");
			
			Serial1.print("TIME:");
			Serial1.print(stop_time - strt_time);
			Serial1.print(";");
			
            Serial1.print("END.");
            Serial1.flush();

            //Wait for PC confirmation
            while (Serial1.available() == 0);
            pc_msg =  Serial1.readString();

            //If confirmation failed, send ERROR to PC and wait for response
            if (pc_msg != "ACK" && pc_msg != "END") {
              Serial1.print("ERROR");
              Serial1.flush();
              while (Serial1.available() == 0);
            }
			
			//LED indicator OFF
			digitalWrite(3,LOW);
            
          }
        }
      }
	  
	  //LED indicator ON
		digitalWrite(2,HIGH);
		digitalWrite(3,LOW);
		delay(10);
		digitalWrite(2,LOW);
		digitalWrite(3,LOW);
		delay(10);
		digitalWrite(2,HIGH);
		digitalWrite(3,LOW);
    }
  }
}
