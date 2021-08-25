//Sonar Sensor init
#include <SR04.h>
#define ECHO_PIN 12
#define TRIG_PIN 13
SR04 sonar_sensor = SR04(ECHO_PIN,TRIG_PIN);

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

//DC motor init

#define ENABLE_RM 5
#define FORWARD_RM 6
#define BACKWARD_RM 7

#define ENABLE_LM 10
#define FORWARD_LM 9
#define BACKWARD_LM 8

uint8_t rmotor_pwm = 50;
uint8_t lmotor_pwm = 130;

//Encoder init
#define ENCODER 2

//MPU6050 init (accelerometer and gyroscope)
#include <MPU6050_light.h>
MPU6050 mpu(Wire);


// The distance robot travels after single encoder revolution in cm
// Formula = 2*pi*R/Num. Num = 46, Number of encoder revolution for a full 360 wheel turn. R = 5.25[cm], Wheel radius.
float step_distance = 0.824668;
int16_t encoder_counter = 1;
int16_t current_state;
int16_t last_state;


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
	
	//Setup Encoder
	pinMode(ENCODER,INPUT);
	attachInterrupt(0, motion_update, CHANGE);
	
	//Setup LED indicator
	
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	digitalWrite(3,LOW);
	
	// Setup MPU6050
	byte status = mpu.begin();
	while (status != 0) { } // stop everything if could not connect to MPU6050
	
}

void loop() {
  //Initialize sensor data variables
  int16_t sonar_distance = 0;
  int16_t sonar_readings[1000];
  int16_t sonar_index = 0;
  
  int16_t ir_distance;
  int16_t ir_readings[1000];
  int16_t ir_index = 0;
  
  //Start connection to PC
  if (Serial1.available() > 0) {
    String pc_msg;
    pc_msg = Serial1.readString();
    
    if (pc_msg == "START") {
      //Loop until PC closes connection
      while (pc_msg != "END") { 
 
		sonar_distance = sonar_sensor.Distance();
		delay(50);

        while (sonar_distance == 0) {
          sonar_distance = sonar_sensor.Distance();  
        }
        
        //If no obstacle in sight, robot may drive
        if (sonar_distance >= 56) {
			// Calculate MPU6050 offsets
			delay(1000);
			mpu.calcOffsets();
			
			//Jump start driving 
		
			analogWrite(ENABLE_RM,190);
			digitalWrite(FORWARD_RM,HIGH);
			digitalWrite(BACKWARD_RM,LOW);
			
			analogWrite(ENABLE_LM,245);
			digitalWrite(FORWARD_LM,HIGH);
			digitalWrite(BACKWARD_LM,LOW);
	  
			delay(300);
			
			//Drive robot forward
			analogWrite(ENABLE_RM,rmotor_pwm);
			digitalWrite(FORWARD_RM,HIGH);
			digitalWrite(BACKWARD_RM,LOW);
			
			analogWrite(ENABLE_LM,lmotor_pwm);
			digitalWrite(FORWARD_LM,HIGH);
			digitalWrite(BACKWARD_LM,LOW);
          
        }
		
        while (sonar_distance >= 70|| sonar_distance == 0) {
      
		//Collect sensor data
        sonar_distance = sonar_sensor.Distance();
        ir_distance = ir_sensor.distance();

        //Save sensor data
        if (sonar_index < 1000 && ir_index < 1000) {
            sonar_readings[sonar_index] = sonar_distance;
            sonar_index++;
            ir_readings[ir_index] = ir_distance;
            ir_index++;
          }
		  
        //If obstacle present, stop robot and send sensor data to PC
          
        if (sonar_distance <= 70 && sonar_distance != 0) {
            
			// Stop robot
			digitalWrite(ENABLE_RM,LOW);
            digitalWrite(ENABLE_LM,LOW);
			
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
			
			Serial1.print("DISTANCE:");
			Serial1.print(encoder_counter*step_distance);
			Serial1.print(";");
			
            Serial1.print("END|");
            Serial1.flush();

            //Wait for data from PC
            while (Serial1.available() == 0);
            pc_msg =  Serial1.readString();
			
			//Reset Encoder
			encoder_counter = 0;
			
			//Turn robot
			make_turn(pc_msg);
			
			delay(5000);
			digitalWrite(4,HIGH);
			
			//Measure distance from obstacle (three times for more accuracy)
			int16_t sonar_distance_sum = 0;
			for (int i = 0; i < 3; i++) {
				sonar_distance_sum += sonar_sensor.Distance();
				delay(50);
			}
			//sonar_distance = sonar_distance_avg/3;
			
          }
        }
      }
    }
  }
}

// Function invoked by interrupt for each change in pin2 (encoder)
void motion_update(){
	//Collect encoder data
	current_state = digitalRead(ENCODER);
	
	if (current_state != last_state) {
		encoder_counter++;
	}
	
	last_state = current_state;
	
	//Collect gyroscope data
	mpu.update();
	int gz = mpu.getAngleZ();
	
	if(gz > 2) { //Robot drags to the right, increase left motor power
		lmotor_pwm += 10;
	}
	if(gz < -2) { //Robot drags to the left, decrease left motor power
		lmotor_pwm -= 10;
	}
}

void make_turn(String turn_direction){
	int gz;
	
	// Calculate MPU6050 offsets
	delay(1000);
	mpu.calcOffsets();
	
	//Jump start driving 
		
	analogWrite(ENABLE_RM,190);
	digitalWrite(FORWARD_RM,HIGH);
	digitalWrite(BACKWARD_RM,LOW);
	
	analogWrite(ENABLE_LM,245);
	digitalWrite(FORWARD_LM,HIGH);
	digitalWrite(BACKWARD_LM,LOW);

	delay(100);
	
	if (turn_direction == "L") {
		analogWrite(ENABLE_RM,255);
		digitalWrite(FORWARD_RM,HIGH);
		digitalWrite(BACKWARD_RM,LOW);
		
		analogWrite(ENABLE_LM,255);
		digitalWrite(FORWARD_LM,LOW);
		digitalWrite(BACKWARD_LM,HIGH);
		
		do {
			mpu.update();
			gz = mpu.getAngleZ();
		}while(abs(gz) < 90);
		
		digitalWrite(ENABLE_RM,LOW);
		digitalWrite(ENABLE_LM,LOW);
	}
	
	else {
		analogWrite(ENABLE_RM,255);
		digitalWrite(FORWARD_RM,LOW);
		digitalWrite(BACKWARD_RM,HIGH);
	
		analogWrite(ENABLE_LM,255);
		digitalWrite(FORWARD_LM,HIGH);
		digitalWrite(BACKWARD_LM,LOW);
		
		do {
			mpu.update();
			gz = mpu.getAngleZ();
		}while(abs(gz) < 90);
		
		digitalWrite(ENABLE_RM,LOW);
		digitalWrite(ENABLE_LM,LOW);
	}
}