//DC motor init

#define ENABLE_RM 5
#define FORWARD_RM 6
#define BACKWARD_RM 7

#define ENABLE_LM 10
#define FORWARD_LM 9
#define BACKWARD_LM 8


void setup(){
	//Setup DC motors ports
  
    pinMode(ENABLE_RM,OUTPUT);
    pinMode(FORWARD_RM,OUTPUT);
    pinMode(BACKWARD_RM,OUTPUT);
    
    pinMode(ENABLE_LM,OUTPUT);
    pinMode(FORWARD_LM,OUTPUT);
    pinMode(BACKWARD_LM,OUTPUT);
	
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	digitalWrite(2,LOW);
}

void loop() {
	
	digitalWrite(3, HIGH);
	
    delay(2000);
		//Drive robot forward
		/*  	analogWrite(ENABLE_LM,90);
			digitalWrite(FORWARD_RM,HIGH);
			digitalWrite(BACKWARD_RM,LOW); */
			
			analogWrite(ENABLE_LM,255);
			digitalWrite(FORWARD_LM,HIGH);
			digitalWrite(BACKWARD_LM,LOW);

    delay(2000);
		// analogWrite(ENABLE_RM,255);
		// digitalWrite(FORWARD_RM,HIGH);
		// digitalWrite(BACKWARD_RM,LOW);
		
		// analogWrite(ENABLE_LM,255);
		// digitalWrite(FORWARD_LM,LOW);
		// digitalWrite(BACKWARD_LM,HIGH);
		
		//delay(600);
		
		//digitalWrite(3,LOW);
		
		digitalWrite(ENABLE_RM,LOW);
		digitalWrite(ENABLE_LM,LOW);
		
}
