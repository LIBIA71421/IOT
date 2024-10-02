#include <WiFi.h>
#include <Arduino.h>

class WiFiConnection {
public:
    WiFiConnection(const char *ssid, const char *password);
    void begin();

private:
    const char *ssid;
    const char *password;
};

WiFiConnection::WiFiConnection(const char *ssid, const char *password)
    : ssid(ssid), password(password) {}

void WiFiConnection::begin() {
    Serial.print("Conectando a WiFi: ");
    Serial.println(ssid);  

    WiFi.begin(ssid, password);
    int intentos = 0;  
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");  
        
        intentos++;
        if (intentos > 45) {  
            Serial.println("\nError: No se pudo conectar al WiFi.");
            return;  
        }
    }

    Serial.println("\nConectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

class UltrasonicSensor {
public:
    UltrasonicSensor(int trigPin, int echoPin);
    void begin();
    long readDistance();

private:
    int trigPin;
    int echoPin;
};

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin)
    : trigPin(trigPin), echoPin(echoPin) {}

void UltrasonicSensor::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

long UltrasonicSensor::readDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    long distance = duration * 0.034 / 2;
    return distance;
}

int trigPin = 11;
int echoPin = 10;

const char *ssid = "ResiCapilla";
const char *password = "ResidenciaE@";
WiFiConnection wifiConnection(ssid, password);

UltrasonicSensor sensor(trigPin, echoPin);

WiFiClient client;
const char* serverIP = "192.168.0.105";
int serverPort = 8085;

void setup() {
    Serial.begin(115200);
    wifiConnection.begin();
    sensor.begin();
}

void loop() {
    long distance = sensor.readDistance();
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");

    int estado = 0;

    if (distance >= 200 && distance < 300) {
        estado = 3;  
    } else if (distance >= 50 && distance < 200) {
        estado = 2;  
    } else if (distance >= 2 && distance < 50) {
        estado = 1;  
    } else {
        estado = 0;  
    }

    if (client.connect(serverIP, serverPort)) {
        client.print("distance=" + String(estado) + "\n");
        Serial.println("Estado enviado: " + String(estado));
        client.stop();
    }

    delay(1000);
}