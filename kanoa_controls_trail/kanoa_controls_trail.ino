#include <Wire.h>
#include <Servo.h>
//definitions
#define MPU_Address 0x68;

// defining variables
float Acc_rawX, Acc_rawY, Acc_rawZ, Gry_rawX, Gry_rawY, Gyr_rawZ, gain_G;
float rollAngle, rollGyro, rollAccel;
float timePrev, deltaTime, time;
int setpoint, angle, data;
Servo myservo;


void setup() {
// setting up Serial Comms
Serial.begin(9600);
Serial.setTimeout(10);
// intilializaing IMU
Wire.begin();
Wire.beginTransmission(0x68);
Wire.write(0x6B);
Wire.write(0x00);
Wire.endTransmission(true);
//Defining pinouts
myservo.attach(3);
myservo.write(90);

//Defining variables
gain_G = 0.1;
setpoint = 0;
}


// Main Loop
void loop() {
  followAngle(serialAngle(), accelRead());
}

// Functions
void followAngle(int setpoint, float IMUInput){
  int angle = setpoint + IMUInput + 90;
  myservo.write(angle);
  Serial.print("Servo angle: ");
  Serial.println(angle - 90);
}

int serialAngle(){
  if(Serial.available()==0){}
  else{
    String data = Serial.readString();
    int setpoint = data.toFloat();
    //myservo.write(setpoint);
    return setpoint;
    Serial.println("Input angle: ");
    Serial.print(setpoint);
  }
}

float accelRead(){
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,6,true);
  Acc_rawX=(Wire.read()<<8|Wire.read())/4096.0 ; //each value needs two registres
  Acc_rawY=(Wire.read()<<8|Wire.read())/4096.0 ;
  Acc_rawZ=(Wire.read()<<8|Wire.read())/4096.0 ;

  rollAccel = (atan(-1 * Acc_rawX / sqrt(pow(Acc_rawY, 2) + pow(Acc_rawZ, 2))) * 180 / PI) + 1.58; 

  Serial.print("Accel Roll angle: ");
  Serial.println(rollAccel);
  return rollAccel;
}

float gyroRead(){
  timePrev = time;
  time = millis();
  deltaTime = (time - timePrev) / 1000;

  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,4,true);

  Gry_rawX = Wire.read()<<8|Wire.read();
  Gry_rawX = (Gry_rawX/32.8);

  rollGyro = Gry_rawX*deltaTime;
  if(rollGyro > 90){
    rollGyro = 90;
  }
  if(rollGyro < -90){
    rollGyro = -90;
  }
  Serial.print("Gyro Roll angle: ");
  Serial.println(rollGyro);
  return rollGyro;
}