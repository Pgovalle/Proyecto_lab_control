// Ejemplo de uso de sockets con photon
// Notar que se conecta a la red
// pero no a la "cloud" (lol)

// Tomás Herrera C
// PUC


// Para no conectarnos a la inutil cloud de manera auto
SYSTEM_MODE(SEMI_AUTOMATIC);
UDP Udp;
unsigned int port = 5005;

int led = D7; 
void setup() {

	IPAddress myAddress(192,168,1,125);
    IPAddress netmask(255,255,255,0);
    IPAddress gateway(192,168,1,1);
    IPAddress dns(192,168,1,1);
    WiFi.setStaticIP(myAddress, netmask, gateway, dns);

    // now let's use the configured IP
    WiFi.useStaticIP();

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
