TCPClient client;
byte server[] = {192,168,100,100};
SYSTEM_MODE(SEMI_AUTOMATIC);
volatile int num = 0; 
//client ip: 192.168.1.125
void clean_buffer(uint8_t* buffer, size_t size);
void recieve_on_buffer(uint8_t* buffer);
void shift_buffer(uint8_t* buffer);
uint8_t mem[4] = {48,48,48,48};
uint8_t *buffer = mem;
size_t size = 4;

void setup(){
	Serial.begin(9600);
	WiFi.connect();
	//waitUntil(WiFi.ready);
	//waitFor(Serial.isConnected, 30000);
	char c = Serial.read();
  	while(c != 's'){
    	c = Serial.read();
 	}
	Serial.println("connecting...");
	if(client.connect(server, 80)){
		Serial.println("connected");
	}
	else{
		Serial.println("failed");
	}
}

void loop(){
	if(client.connected()){
		if(client.available()){
			recieve_on_buffer(buffer);
			int new_value = (*buffer-48)*1 + (*(buffer+1)-48)*10 + (*(buffer+2)-48)*100 + (*(buffer+3)-48)*1000; 
			clean_buffer(buffer,size);
			Serial.print("Recibi ");
			Serial.println(new_value);
		}
	}
	else{
		client.connect(server,80);
	}
}

void serialEvent()
{
    num = Serial.parseInt();
    Serial.print("Enviando ");
    Serial.println(num);
    if(client.connected()){
    	num = 10000+num;
    	char out_buffer[6];
    	sprintf(out_buffer,"%d",num);
    	out_buffer[5] = 'S';
   		client.write((uint8_t*)out_buffer,6);
   	}
   	else{
   		client.connect(server,80);
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

