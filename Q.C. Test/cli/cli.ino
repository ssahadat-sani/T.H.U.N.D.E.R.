#include <ESP32Servo.h>

// ================= PIN DEFINITIONS =================

#define SERVO_PIN   4

#define STANDBY_PIN 5

#define PWM_A       6
#define PWM_B       18

#define A1_IN       15
#define A2_IN       7

#define B1_IN       16
#define B2_IN       17

#define MIST_PIN    -1


// ================= OBJECTS =================

Servo myservo;
String cmd;


// ================= MOTOR FUNCTIONS =================

void standby(bool state)
{
  digitalWrite(STANDBY_PIN, state);
}

void motorA_forward(int speed)
{
  digitalWrite(A1_IN, HIGH);
  digitalWrite(A2_IN, LOW);
  ledcWrite(PWM_A, speed);
}

void motorA_backward(int speed)
{
  digitalWrite(A1_IN, LOW);
  digitalWrite(A2_IN, HIGH);
  ledcWrite(PWM_A, speed);
}

void motorA_stop()
{
  ledcWrite(PWM_A, 0);
}

void motorB_forward(int speed)
{
  digitalWrite(B1_IN, HIGH);
  digitalWrite(B2_IN, LOW);
  ledcWrite(PWM_B, speed);
}

void motorB_backward(int speed)
{
  digitalWrite(B1_IN, LOW);
  digitalWrite(B2_IN, HIGH);
  ledcWrite(PWM_B, speed);
}

void motorB_stop()
{
  ledcWrite(PWM_B, 0);
}

void stopAll()
{
  motorA_stop();
  motorB_stop();
}


// ================= HELP FUNCTION =================

void showHelp()
{
  Serial.println();
  Serial.println("===== HELP MENU =====");

  Serial.println("s <angle>   : Servo (0-180)");
  
  Serial.println("af <speed>  : MotorA Forward (0-255)");
  Serial.println("ab <speed>  : MotorA Backward");
  Serial.println("as          : MotorA Stop");

  Serial.println("bf <speed>  : MotorB Forward");
  Serial.println("bb <speed>  : MotorB Backward");
  Serial.println("bs          : MotorB Stop");

  Serial.println("on          : Standby ON");
  Serial.println("off         : Standby OFF");

  Serial.println("m1          : Mist ON");
  Serial.println("m0          : Mist OFF");

  Serial.println("x           : Stop All");

  Serial.println("h           : Help");

  Serial.println("=====================");
  Serial.println();
}


// ================= COMMAND PROCESS =================

void process(String c)
{
  c.trim();

  // HELP
  if(c == "h")
  {
    showHelp();
  }

  // SERVO
  else if(c.startsWith("s "))
  {
    int angle = constrain(c.substring(2).toInt(), 0, 180);
    myservo.write(angle);

    Serial.print("Servo: ");
    Serial.println(angle);
  }

  // MOTOR A
  else if(c.startsWith("af "))
  {
    int speed = constrain(c.substring(3).toInt(), 0, 255);
    motorA_forward(speed);
    Serial.println("MotorA Forward");
  }

  else if(c.startsWith("ab "))
  {
    int speed = constrain(c.substring(3).toInt(), 0, 255);
    motorA_backward(speed);
    Serial.println("MotorA Backward");
  }

  else if(c == "as")
  {
    motorA_stop();
    Serial.println("MotorA Stop");
  }

  // MOTOR B
  else if(c.startsWith("bf "))
  {
    int speed = constrain(c.substring(3).toInt(), 0, 255);
    motorB_forward(speed);
    Serial.println("MotorB Forward");
  }

  else if(c.startsWith("bb "))
  {
    int speed = constrain(c.substring(3).toInt(), 0, 255);
    motorB_backward(speed);
    Serial.println("MotorB Backward");
  }

  else if(c == "bs")
  {
    motorB_stop();
    Serial.println("MotorB Stop");
  }

  // STANDBY
  else if(c == "on")
  {
    standby(true);
    Serial.println("Standby ON");
  }

  else if(c == "off")
  {
    standby(false);
    Serial.println("Standby OFF");
  }

  // MIST
  else if(c == "m1")
  {
    digitalWrite(MIST_PIN, HIGH);
    Serial.println("Mist ON");
  }

  else if(c == "m0")
  {
    digitalWrite(MIST_PIN, LOW);
    Serial.println("Mist OFF");
  }

  // STOP ALL
  else if(c == "x")
  {
    stopAll();
    Serial.println("All Stopped");
  }

  else
  {
    Serial.println("Unknown. Type h for help.");
  }
}


// ================= SETUP =================

void setup()
{
  Serial.begin(115200);

  // Servo timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Servo setup
  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_PIN, 500, 2500);

  // Pin modes
  pinMode(STANDBY_PIN, OUTPUT);

  pinMode(A1_IN, OUTPUT);
  pinMode(A2_IN, OUTPUT);

  pinMode(B1_IN, OUTPUT);
  pinMode(B2_IN, OUTPUT);

  pinMode(MIST_PIN, OUTPUT);

  // PWM setup (ESP32 Core 3.x)
  ledcAttach(PWM_A, 5000, 8);
  ledcAttach(PWM_B, 5000, 8);

  standby(true);

  Serial.println();
  Serial.println("ESP32-S3 CLI Controller Ready");
  Serial.println("Type h for help");
}


// ================= LOOP =================

void loop()
{
  if(Serial.available())
  {
    cmd = Serial.readStringUntil('\n');
    process(cmd);
  }
}
