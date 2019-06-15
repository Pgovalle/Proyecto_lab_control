SYSTEM_MODE(SEMI_AUTOMATIC);
#include <stdlib.h>
int analog_output = A6;
int analog_input = A0;
TCPClient client;
byte server[] = {192,168,1,103};
volatile float kp = 4.0, ki = 0, kd = 0, kw = 0, filter = 0.2, kt =1.0;
volatile float current_read = 0, last_read = 0, current_error = 0, last_error = 0, reference = 0, sum_error = 0, windup = 0, slope = 0;
volatile float current_slope = 0, last_slope = 0; 
volatile float proportional; 
volatile int output;
volatile int flag_server = 0, error_margin = 4095;
volatile int start = 0, allow_control2 = 0;
float dt = 0.001;
uint8_t mem[4] = {48,48,48,48};
size_t size = 4;
volatile uint8_t received_from_server[4];
uint8_t *buffer = mem;
volatile char new_from_client;

//TCPServer server = TCPServer(81);


//WiFi.credentials('labcontrol','beaglebone')
void print_values(){
	Serial.print("------------------\r\n");
	Serial.print("kp=");
	Serial.print(kp);
	Serial.print(" | ki=");
	Serial.print(ki);
	Serial.print(" | kd=");
	Serial.print(kd);
	Serial.print("\r\n");
	Serial.print("Medicion: ");
	Serial.print(current_read);
	Serial.print("\r\n");
	Serial.print("Output: ");
	Serial.print(output);
	Serial.print("\r\n");
	Serial.print("Current error: ");
	Serial.print(current_error);
	Serial.print("\r\n");
	Serial.print("proportional: ");
	Serial.print(proportional);
	Serial.print("\r\n");
	Serial.print("sum_error: ");
	Serial.print(sum_error);
	Serial.print("\r\n");
	Serial.print("slope: ");
	Serial.print(slope);
	Serial.print("\r\n");
}

Timer control1_timer(1, control1);
Timer print_values_timer(1000, print_values);
Timer read_input_timer(1, read_input);
Timer control2_timer(1, control2);


//Timer timer(1, sine);



int led = D7; 
// void sine(){
// 	static int value = 0;
// 	static int up = 1;
// 	value += up ? 1:-1;
// 	if(value>=4095){
// 		up = 0;
// 	}
// 	else if(value<=0){
// 		up = 1;
// 	}
// 	analogWrite(analog_output, value);
// }
void clean_buffer(uint8_t* buffer, size_t size);
void recieve_on_buffer(uint8_t* buffer);
void shift_buffer(uint8_t* buffer);






void control1(){
	last_error = current_error;
	current_error = (-reference + current_read)*kt;
	sum_error += (ki*current_error+windup*kw)*dt;
	proportional = current_error;
	last_slope = current_slope;
	current_slope = ((current_error-last_error)/dt)*filter+(1-filter)*last_slope;
	slope = kd*current_slope;
	output = (int)(sum_error+proportional+slope);
	output = 2095+output;
	if (output>4095) {
		windup = 4095-output;
		output = 4095;
	}
	else if(output<0){
		windup = output-4095;
		output = 0;
	}
	else{
		windup = 0;
	}
	analogWrite(analog_output, output);
	if(allow_control2 == 1 && abs(current_read-reference) >= error_margin){
		control1_timer.stop();
		client.write('C');
		control2_timer.start();
		digitalWrite(led, HIGH);
	}
}

void control2(){
	if(client.available()){
      new_from_client = client.read();
      recieve_on_buffer(buffer);
      output = (*buffer-48)*1 + (*(buffer+1)-48)*10 + (*(buffer+2)-48)*100 + (*(buffer+3)-48)*1000; 
      clean_buffer(buffer,size);
      analogWrite(analog_output, output);
      if(allow_control2 == 0 || current_read-reference <= error_margin){
	    control2_timer.stop();
		control1_timer.start();
		client.write('S');
	  }
    }
    else{
    	control1_timer.start();
    	control2_timer.stop();
    	digitalWrite(led, LOW);
    }
}



void read_input(){
	last_read = current_read;
	current_read = analogRead(analog_input);
	if(control2_timer.isActive()){
		client.write('i');
		client.print((int)current_read);
	}
}
void setup(){
	pinMode(led, OUTPUT);
	Serial.begin(9600);
	waitFor(Serial.isConnected, 30000);
	WiFi.connect();
	int start = Serial.read();
	while(start!='s'){
		start = Serial.read();
	}
	Serial.println("connecting...");
	if(client.connect(server, 80)){
		Serial.println("connected");
	}
	else{
		Serial.println("failed");
	}
	pinMode(analog_output, OUTPUT);
	//timer.start();
	control1_timer.start();
	read_input_timer.start();
	print_values_timer.start();
}

void loop(){
	;
}

void serialEvent(){
	sum_error = 0.0;
	char c = Serial.read();
	switch (c) {
		case 'C':
			if (allow_control2==1){
				allow_control2 = 0;
			} else {
				allow_control2 = 1;
			}
			break;
		case 's':
			start = 1;
			Serial.println("inicio");

			break;
	    case 'p':
	        kp = Serial.parseFloat();
	        break;
	    case 'i':
	    	ki = Serial.parseFloat();
	        break;
	    case 'd':
	    	kd = Serial.parseFloat();
	    	break;
	    case 'r':
	    	reference = Serial.parseFloat();
	    	if(allow_control2 == 1 && client.connected()){
	    		client.write('r');
	    		client.print((int)reference);
	    	}
	    	break;
	    case 'w':
	    	kw = Serial.parseFloat();
	    	break;
	    case 'f':
	    	filter = Serial.parseFloat();
	    	break;
	    case 't':
	    	kt = Serial.parseFloat();
	    	break;
	    case 'm':
	    	error_margin = Serial.parseInt();
	    	break;
	    default:
	    	break;
	      // do something
	}
}

void clean_buffer(uint8_t* buffer, size_t size){
  for(volatile int i = 0; i<size; i++){
    *(buffer+i) = 48;
  }
}

void recieve_on_buffer(uint8_t* buffer){
  while(client.available()){
    shift_buffer(buffer);
    *(buffer) = client.read();
  }
}

void shift_buffer(uint8_t* buffer){
  for(volatile int i = 3; i>0; i--){
    *(buffer+i) = *(buffer+i-1);
  }
}