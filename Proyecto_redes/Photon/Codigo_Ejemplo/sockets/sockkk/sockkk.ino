// Ejemplo de uso de sockets con photon
// Notar que se conecta a la red
// pero no a la "cloud" (lol)

// Tomás Herrera C
// PUC


// Para no conectarnos a la inutil cloud de manera auto
SYSTEM_MODE(SEMI_AUTOMATIC);
UDP Udp;
unsigned int port = 80;

int led = D7; 
void setup() {

  pinMode(led, OUTPUT);
  Serial.begin(9600);
  WiFi.connect(); // nos conectamos a wifi
  waitUntil(WiFi.ready);
  Serial.println("connected");
  Udp.begin(port);
}

void loop() {
  char rcv = 0;
  if (Udp.parsePacket() > 0) {
    rcv = Udp.read();
    Udp.flush();
    if (rcv == 'i')  digitalWrite(led, HIGH);
    else digitalWrite(led, LOW);
    Serial.println(rcv);
  }
  // Send ip
  Serial.println(WiFi.localIP());
  delay(1000);
}
