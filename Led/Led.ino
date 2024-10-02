#include <WiFi.h>

int ledPinRed = 14;
int ledPinYellow = 26;
int ledPinGreen = 27;

const char *ssid = "ResiCapilla";
const char *password = "ResidenciaE@";

const char* serverIP = "192.168.0.103";
int serverPort = 8085;

int estado = -1;

class LEDController {
public:
    LEDController(int pin) : ledPin(pin) {}

    void begin() {
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, LOW);
    }

    void show(int state) {
        digitalWrite(ledPin, state);
    }

    void blink(int delayTime, int intensity) {
        analogWrite(ledPin, intensity);  
        delay(delayTime);                
        analogWrite(ledPin, 0);          
        delay(delayTime);                
    }

private:
    int ledPin;
};

class WiFiConnection {
public:
    WiFiConnection(const char *ssid, const char *password) : ssid(ssid), password(password) {}

    void begin() {
        Serial.print("Conectando a WiFi: ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);
        int intentos = 0;

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");

            intentos++;
            if (intentos > 40) {
                Serial.println("\nError: No se pudo conectar al WiFi.");
                return;
            }
        }

        Serial.println("\nConectado a WiFi");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
    }

private:
    const char *ssid;
    const char *password;
};

WiFiConnection wifiConnection(ssid, password);
LEDController redLED(ledPinRed);
LEDController yellowLED(ledPinYellow);
LEDController greenLED(ledPinGreen);

void setup() {
    Serial.begin(115200);
    wifiConnection.begin();

    redLED.begin();
    yellowLED.begin();
    greenLED.begin();
}

void loop() {
    WiFiClient client;
    if (client.connect(serverIP, serverPort)) {
        client.println("GET");  

        String response = client.readStringUntil('\n');
        estado = response.toInt();
        Serial.println("Estado recibido: " + String(estado));

        redLED.show(LOW);
        yellowLED.show(LOW);
        greenLED.show(LOW);

        switch (estado) {
            case 0:  // Estado 0: Apagados
                Serial.println("Estado 0: LEDs apagados.");
                break;
            case 1:  // Estado 1: LED 1
                Serial.println("Estado 1: LED 1 parpadea.");
                redLED.blink(100, 255);  
                break;
            case 2:  // Estado 2: LED 2
                Serial.println("Estado 2: LED 2 parpadea.");
                yellowLED.blink(100, 255);  
                break;
            case 3:  // Estado 3: LED 3
                Serial.println("Estado 3: LED 3 parpadea.");
                greenLED.blink(100, 255);  
                break;
            default:
                Serial.println("Estado no reconocido.");
                break;
        }

        client.stop();  
    } else {
        Serial.println("Conexión fallida al servidor.");
    }

    delay(1000);  
}
