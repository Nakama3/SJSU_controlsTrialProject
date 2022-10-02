#include <Wire.h>
#include <Servo.h>

// defining variables
float Gry_rawX, rollGyro;
float timePrev, deltaTime, time;
int setpoint, angle, Kr; 
Servo myservo;


void setup() {
//Setting up Serial Comms
Serial.begin(9600);
Serial.setTimeout(10);

Wire.begin();
Wire.beginTransmission(0x68);
Wire.write(0x6B);
Wire.write(0x00);
Wire.endTransmission(true);
//Setting pinouts
myservo.attach(3);
myservo.write(90); //Initial servo position 

//Setting variables
Kr = 2; //Roll gain
}

/*
The main loop function iterates the time value and runs followAngle with parameters
of the terminal input and the roll angle from the gryoscope.
*/
void loop() {
  timePrev = time;
  time = millis();
  deltaTime = (time - timePrev) / 1000;

  followAngle(serialAngle(), gyroRead());
}

/*
Follow Angle takes a defined setpoint and the angle produced by the gryoscope
and outputs the angle for the servo stay level relative to the gryoscopic motion
*/
void followAngle(int setpoint, float gyroInput){ 
  int angle = setpoint - gyroInput + 90;
  myservo.write(angle);
  Serial.print("Servo angle: ");
  Serial.println(angle - 90);
}

/*
serialAngle takes in a string from the serial terminal and casts it into an int as an offset angle
that is returned as an int.
--Note that the value of 0 that is read from the serial terminal is ignored
*/
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

/*
gyroRead reads the x-axis gryo value from the MPU6050 over I2C and integrates the read rad/sec value
into degrees. This value is then returned as a float.
*/
float gyroRead(){
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,4,true);

  Gry_rawX = Wire.read()<<8|Wire.read();
  Gry_rawX = Kr*(Gry_rawX/32.8);

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