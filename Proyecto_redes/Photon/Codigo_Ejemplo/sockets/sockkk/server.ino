SYSTEM_MODE(SEMI_AUTOMATIC);
TCPServer server = TCPServer(80);
TCPClient client;
//server ip: 192.168.1.103
uint8_t mem[4] = {48,48,48,48};
uint8_t *buffer = mem;
size_t size = 4;
volatile char new_from_client;
volatile int reference = 0;

void clean_buffer(uint8_t* buffer, size_t size);
void recieve_on_buffer(uint8_t* buffer);
void shift_buffer(uint8_t* buffer);

void setup(){
  WiFi.connect();
  waitUntil(WiFi.ready);
  server.begin();
  Serial.begin(9600);
  waitFor(Serial.isConnected, 30000);//espera 30 seg
  char c = Serial.read();
  while(c != 's'){
    c = Serial.read();
  }
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.subnetMask());
  Serial.println(WiFi.gatewayIP());
}

void loop() {
  if(client.connected()){
    //Serial.println("new connection");
    if(client.available()){
      new_from_client = client.read();
      recieve_on_buffer(buffer);
      int new_value = (*buffer-48)*1 + (*(buffer+1)-48)*10 + (*(buffer+2)-48)*100 + (*(buffer+3)-48)*1000; 
      clean_buffer(buffer,size);
      Serial.print("Recibi ");
      Serial.println(new_value);
      switch (new_from_client){
        case '1':
          //sum_error = 0;
          //reference = new_value;
          server.print(new_value);
          Serial.println("Enviando");
          //Serial.println((int)reference);
          break;
        case 'i':
          //input = new_value;
          break;
        case 'C':
          //control_server_timer.start();
          break;
        case 'S':
          //control_server_stop.stop();
          break;
        default:
          break;
      }
      //server.write(c);
    }
  }
  else {
    client = server.available();
  }
}

void clean_buffer(uint8_t* buffer, size_t size){
  for(volatile int i = 0; i<size; i++){
    *(buffer+i) = 48;
  }
}

void recieve_on_buffer(uint8_t* buffer){
  char stop = client.read();
  while(stop != 'S'){
    shift_buffer(buffer);
    *(buffer) = stop;
    Serial.println(stop);
    (client.available()){
      stop = client.read();
    }
  }
}

void shift_buffer(uint8_t* buffer){
  for(volatile int i = 3; i>0; i--){
    *(buffer+i) = *(buffer+i-1);
  }
}