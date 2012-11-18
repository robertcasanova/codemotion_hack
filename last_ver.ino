#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0x9E, 0x3E };
IPAddress server_ip(10,1,89,15);
byte my_ip[] = {10,1,90,230};

const int SENSOR = 2;
int t = 40;
String data;

const float A = 3.354016e-03;
const float B = 2.569355e-04;
const float C = 2.626311e-06;
const float D = 0.675278e-07;

int sensorValue = 0;
int v0;
float R = 4700.0;
float logRt, Rt, T;



EthernetClient client;


void setup() {
  Serial.begin(9600);
  
  if(Ethernet.begin(mac) == 0) {
      Serial.println("Ethernet fail");
  }
  Serial.println("Connecting..");
  delay(1000);
  
  if(client.connect(server_ip,8001)) {
     Serial.println("Connected"); 
     
  } else {
     Serial.println("No server: FAIL"); 
  }
}

void postRequest(EthernetClient client, String path, String data) {
  String buf = "POST " + path + " HTTP/1.1";
  Serial.println("Trying to post:");


  client.println(buf);
  client.println("Host: 10.1.89.15:8001");
  client.println("Content-Type: application/json");
  client.println("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_6_8) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11");
  client.println("Content-Length: " + String(data.length()));
  client.println("");
  client.println(data);
  client.println("");
  client.println("");
  
  Serial.println("Post complete");
    
}



void loop() {
  if(client.available()) {
     char c = client.read();
    Serial.print(c); 
  }
  t = (int)temperature(analogRead(SENSOR));
  //Serial.println(t);
  //delay(1000);
  if(t > 30) {
     Serial.println("FIRE");
     Serial.println(t);
     data = "{ \"emergencies\" : { \"id\": 1, \"loc\": { \"lon\" : 45.5569592, \"lat\" : 12.4023351 }, \"type\" : \"fire\", \"level\": \"medium\", \"intensity\": \"42m\", \"timestamp\": \"2012-10-23 10:23:52\"}}" ;     
     postRequest(client,"/sensor",data);
     delay(4000);
  }
}

double temperature(int RawADC) {
  v0 = RawADC;
  Rt = R*( 1023.0 / (float)v0 - 1.0 );
  logRt = log(Rt/R);
  T = 1/(A+B*logRt+C*logRt*logRt+D*logRt*logRt*logRt);
  T -= 273.15;
  T -= 20;
  return T;  
}
