#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// ================= PIN DEFINITIONS =================
#define SERVO_PIN   4
#define STANDBY_PIN 5
#define PWM_A       6   // Left motor PWM
#define PWM_B       18  // Right motor PWM
#define A1_IN       15  // Left motor direction
#define A2_IN       7
#define B1_IN       16  // Right motor direction
#define B2_IN       17
#define MIST_PIN    -1  // Optional

// ================= WIFI =================
const char* ssid = "ESP32-AP";
const char* password = "12345678";

WebServer server(80);

// ================= OBJECTS =================
Servo myservo;
int defaultSpeed = 150;

// ================= MODE =================
// HOLD mode: robot moves only while button is held (stop on release)
// DRIVE mode: robot keeps moving after button press (stop only on 'S')
bool holdMode = true; // default: HOLD mode

// ================= MOTOR FUNCTIONS =================
void standby(bool state) { digitalWrite(STANDBY_PIN, state); }

void motorA_forward(int speed){ digitalWrite(A1_IN,HIGH); digitalWrite(A2_IN,LOW);  ledcWrite(PWM_A, speed); }
void motorA_backward(int speed){ digitalWrite(A1_IN,LOW); digitalWrite(A2_IN,HIGH); ledcWrite(PWM_A, speed); }
void motorA_stop(){ ledcWrite(PWM_A, 0); }

void motorB_forward(int speed){ digitalWrite(B1_IN,HIGH); digitalWrite(B2_IN,LOW);  ledcWrite(PWM_B, speed); }
void motorB_backward(int speed){ digitalWrite(B1_IN,LOW); digitalWrite(B2_IN,HIGH); ledcWrite(PWM_B, speed); }
void motorB_stop(){ ledcWrite(PWM_B, 0); }

void stopAll(){ motorA_stop(); motorB_stop(); }

// ================= SERVO STEERING =================
void steer(String dir){
  if(dir == "LEFT")       myservo.write(140);
  else if(dir == "RIGHT") myservo.write(40);
  else                    myservo.write(90); // CENTER
}

// ================= SERVER HANDLER =================
void handleCommand() {
  String request = server.uri();
  if(request.startsWith("/")) request = request.substring(1);

  int speed = defaultSpeed;

  // Parse speed if provided (e.g., /F:200)
  if(request.indexOf(":") != -1){
    int idx = request.indexOf(":");
    speed = request.substring(idx + 1).toInt();
    request = request.substring(0, idx);
  }

  // ================= MODE SWITCHING =================
  if(request == "HOLD"){
    holdMode = true;
    Serial.println("Mode: HOLD");
    server.send(200, "text/plain", "OK:HOLD");
    return;
  }
  else if(request == "DRIVE"){
    holdMode = false;
    Serial.println("Mode: DRIVE");
    server.send(200, "text/plain", "OK:DRIVE");
    return;
  }

  // ================= MOVEMENT COMMANDS =================
  if(request == "F"){
    steer("CENTER");
    motorA_forward(speed); motorB_forward(speed);
  }
  else if(request == "B"){
    steer("CENTER");
    motorA_backward(speed); motorB_backward(speed);
  }
  else if(request == "L"){
    steer("LEFT");
    motorA_forward(speed/2); motorB_forward(speed);
  }
  else if(request == "R"){
    steer("RIGHT");
    motorA_forward(speed); motorB_forward(speed/2);
  }
  else if(request == "S"){
    // S = Stop / Release — always stops
    stopAll(); steer("CENTER");
  }

  // ================= HOLD MODE: stop on button release =================
  // In HOLD mode, the app sends "S" when the button is released.
  // In DRIVE mode, "S" is only sent by the dedicated STOP button.
  // No extra logic needed here — the app handles which "S" to send.

  // ================= CUSTOM COMMANDS =================
  else if(request == "X"){ Serial.println("Custom X"); }
  else if(request == "Y"){ Serial.println("Custom Y"); }
  else if(request == "Z"){ Serial.println("Custom Z"); }

  server.send(200, "text/plain", "OK");
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_PIN, 500, 2500);
  myservo.write(90);

  // Pins
  pinMode(STANDBY_PIN, OUTPUT);
  pinMode(A1_IN, OUTPUT); pinMode(A2_IN, OUTPUT);
  pinMode(B1_IN, OUTPUT); pinMode(B2_IN, OUTPUT);
  if(MIST_PIN != -1) pinMode(MIST_PIN, OUTPUT);

  // PWM (Core v3.x API)
  ledcAttach(PWM_A, 5000, 8);
  ledcAttach(PWM_B, 5000, 8);

  standby(true);

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Server
  server.onNotFound(handleCommand);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Default mode: HOLD");
}

// ================= LOOP =================
void loop() {
  server.handleClient();
}
