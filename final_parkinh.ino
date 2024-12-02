/*#define BLYNK_TEMPLATE_ID "TMPL2nFy23Wev"
#define BLYNK_TEMPLATE_NAME "Servo control"
#define BLYNK_AUTH_TOKEN "RhuK3n7T7-0OVv3FBmH1aRz3lgFBwsDr"*/
#define BLYNK_TEMPLATE_ID "TMPL2IULBEbxg"
#define BLYNK_TEMPLATE_NAME "Parking lot"
#define BLYNK_AUTH_TOKEN "IGRpl2tmS2mVz7OlrcSqTLln7CPob5So"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// Configuración de Wi-Fi
char ssid[] = "motorola edge 50 pro_8845";     
char pass[] = "zi9kci43ygyfx52";

// Pines de conexión
const int servoPin = 18;
const int servoPin2 = 19;
const int sensorPin = 2;
const int sensorPin2 = 4;

// Objetos Servo
Servo myServo1;
Servo myServo2;

// Variables para estado de servos
bool isOpen_servo1 = false;
bool isOpen_servo2 = false;
bool changedState1 = false;
bool changedState2 = false;

// Variables para sensores
bool remoteControl1 = false;
bool remoteControl2 = false;
int estadoPrevioEntrada = LOW;
int estadoPrevioSalida = LOW;

// Contadores de vehículos
int contadorEntrada = 0; 
int contadorSalida = 0;

// Habilitación del movimiento de servos con Blynk
bool enableServo = false;
bool enableServo2 = false;

// Configuración inicial
void setup() {
  // Conexión a Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Configuración de servos y sensores
  myServo1.attach(servoPin);
  myServo2.attach(servoPin2);
  pinMode(sensorPin, INPUT);
  pinMode(sensorPin2, INPUT);

  Serial.begin(115200);
  Serial.println("Control de Servomotor con Blynk listo");
  Blynk.virtualWrite(V1, "Total entradas: " + String(contadorEntrada));
  Blynk.virtualWrite(V3, "Total Salidas: " +  String(contadorSalida));
  Blynk.virtualWrite(V5, "Autos dentro: " + String(contadorEntrada - contadorSalida));
}

// Callback para el botón virtual de Blynk (Servo 1)
BLYNK_WRITE(V0) {
  enableServo = param.asInt();
  changedState1 = true;
  if (enableServo) {
    Serial.println("Servo 1 habilitado");
    isOpen_servo1 = false;
    remoteControl1 = true;
  } else {
    Serial.println("Servo 1 deshabilitado");
    isOpen_servo1 = true;
    remoteControl1 = false;
  }
}

// Callback para el botón virtual de Blynk (Servo 2)
BLYNK_WRITE(V2) {
  enableServo2 = param.asInt();
  changedState2 = true;
  if (enableServo2) {
    Serial.println("Servo 2 habilitado");
    isOpen_servo2 = false;
    remoteControl2 = true;
  } else {
    Serial.println("Servo 2 deshabilitado");
    isOpen_servo2 = true;
    remoteControl2 = false;
  }
}

// Bucle principal
void loop() {
  Blynk.run();

  // Lógica para el Servo 1
  if (changedState1) {
    moverServo(myServo1, isOpen_servo1);
    changedState1 = false;  
  } 
  
  // Lógica para el Servo 2
  else if (changedState2) {
    moverServo(myServo2, isOpen_servo2);
    changedState2 = false; 
  } 
  
  // Lógica automática con sensores
  else if (!remoteControl1 && !remoteControl2) {
    int sensorValueEntrada = digitalRead(sensorPin);
    int sensorValueSalida = digitalRead(sensorPin2);

    // Verifica detección en sensor de entrada
    if (sensorValueEntrada == LOW && estadoPrevioEntrada == HIGH) {
      // Abre y cierra la pluma automáticamente
      moverServo(myServo1, false);
      while(sensorValueEntrada == LOW){ //Espera hasta que no haya obstaculo
        sensorValueEntrada = digitalRead(sensorPin);
        delay(1000);
      }
      moverServo(myServo1, true);
      contadorEntrada++;
      Blynk.virtualWrite(V1, "Total entradas: " + String(contadorEntrada));
      Blynk.virtualWrite(V5, "Autos dentro: " + String(contadorEntrada - contadorSalida));
    }

    // Verifica detección en sensor de salida
    if (sensorValueSalida == LOW && estadoPrevioSalida == HIGH) {
      // Abre y cierra la pluma automáticamente
      moverServo(myServo2, false);
      while(sensorValueSalida == LOW){ //Espera hasta que no haya obstaculo
        sensorValueSalida = digitalRead(sensorPin2);
        delay(1000);
      }
      moverServo(myServo2, true);
      contadorSalida++;
      Blynk.virtualWrite(V3, "Total Salidas: " +  String(contadorSalida));
      Blynk.virtualWrite(V5, "Autos dentro: " + String(contadorEntrada - contadorSalida));
    }
    // Actualiza estados previos
    estadoPrevioEntrada = sensorValueEntrada;
    estadoPrevioSalida = sensorValueSalida;    
  }
  
}

// Función para mover el servo
void moverServo(Servo &servo, bool abrir) {
  if (abrir) {
    for (int angle = 0; angle <= 90; angle++) {
      servo.write(angle);
      delay(15);
    }
  } else {
    for (int angle = 90; angle >= 0; angle--) {
      servo.write(angle);
      delay(15);
    }
  }
}