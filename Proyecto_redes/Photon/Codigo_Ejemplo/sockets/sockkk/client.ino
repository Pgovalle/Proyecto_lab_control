TCPClient client;
byte server[] = {192,168,1,103};
SYSTEM_MODE(SEMI_AUTOMATIC);
volatile int num; 
//client ip: 192.168.1.125

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
}

void serialEvent()
{
    num = Serial.parseInt();
    Serial.println(num);
    if(client.connected()){
    	client.write('r');
   		client.print(num);
   	}
   	else{
   		client.connect(server,80);
   	}
}