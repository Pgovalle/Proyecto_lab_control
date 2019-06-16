SYSTEM_MODE(SEMI_AUTOMATIC);
TCPServer server = TCPServer(800);
TCPClient client;
//server ip: 192.168.100.100
uint8_t mem[4] = {48,48,48,48};
uint8_t *buffer = mem;
size_t size = 4;
float dt = 0.001;
volatile char new_from_client;
volatile int  current_read = 0;
volatile float kp = 6.0, ki = 0, kd = 0.02, kw = 0, filter = 0.2, kt =2.0;
volatile float last_read = 0, current_error = 0, last_error = 0, reference = 1600, sum_error = 0, windup = 0, slope = 0;
volatile float current_slope = 0, last_slope = 0; 
volatile float proportional; 
volatile int output = 0, last_output = 0;
int led = D7;

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
  Serial.println("control..");
}

Timer control_server_timer(1000, control_server);

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
  client = server.available();
  pinMode(led, OUTPUT);
  //control_server_timer.start();
}

void loop() {
  if(client.connected()){
    //Serial.println("new connection");
    if(client.available()){
      Serial.print("Server recieve ");
      new_from_client = client.read();
      Serial.println(new_from_client);
      recieve_on_buffer(buffer);
      int new_value = (*buffer-48)*1 + (*(buffer+1)-48)*10 + (*(buffer+2)-48)*100 + (*(buffer+3)-48)*1000; 
      clean_buffer(buffer,size);
      switch(new_from_client){
        case '1':
          current_read = (float)new_value;
          Serial.print("input");
          Serial.println(current_read);
          break;
        case '2':
          sum_error = 0;
          reference = (float)new_value;
          Serial.print("reference");
          Serial.println(reference);
          break;
        case '3':
          Serial.println("starting online control");
          control_server_timer.start();
          digitalWrite(led, HIGH);
          break;
        case '4':
          Serial.println("ending online control");
          control_server_timer.stop();
          digitalWrite(led, LOW);
          break;
        default:
          break;
      }
      //server.write(c);
    }
    if(output!=last_output){
      last_output = output;
      char out_buffer[5];
      if(output>4095){
        sprintf(out_buffer,"%d",4095);
      }
      else if(output<0){
        sprintf(out_buffer,"%d",0);
      }
      else{
        sprintf(out_buffer,"%d",output);
      }
      out_buffer[4] = 'S';
      server.write((uint8_t*)out_buffer, 5,100);
      Serial.println("Server sent");
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
    //Serial.println(stop);
    if(client.available()){
      stop = client.read();
    }
  }
}

void shift_buffer(uint8_t* buffer){
  for(volatile int i = 3; i>0; i--){
    *(buffer+i) = *(buffer+i-1);
  }
}