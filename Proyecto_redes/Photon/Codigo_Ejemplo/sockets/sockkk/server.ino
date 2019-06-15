SYSTEM_MODE(SEMI_AUTOMATIC);
TCPServer server = TCPServer(80);
TCPClient client;
//server ip: 192.168.1.103
uint8_t mem[4] = {48,48,48,48};
uint8_t *buffer = mem;
size_t size = 4;
volatile char new_from_client;
volatile int reference = 0, current_read = 0;
volatile float kp = 1.0, ki = 0, kd = 0, kw = 0, filter = 0.2, kt =1.0;
volatile float last_read = 0, current_error = 0, last_error = 0, reference = 0, sum_error = 0, windup = 0, slope = 0;
volatile float current_slope = 0, last_slope = 0; 
volatile float proportional; 
volatile int output;

void clean_buffer(uint8_t* buffer, size_t size);
void recieve_on_buffer(uint8_t* buffer);
void shift_buffer(uint8_t* buffer);

void control_server(){
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
  server.print(output);
}

Timer control_server_timer(1, control_server);

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
      switch (new_from_client){
        case 'r':
          sum_error = 0;
          reference = new_value;
          break;
        case 'i':
          input = new_value;
          break;
        case 'C':
          control_server_timer.start();
        case 'S':
          control_server_stop.stop();
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