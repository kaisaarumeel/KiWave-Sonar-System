/*
The servo motor moves when the the pin that applies voltage to it ise set to high.
If this pin is high for 2500 microseconds, these corresponds to commanding the servo motor
to spin until it reaches the the angle 180. Moreover, It should be mentioned that the srvo
know which angel which. For example, if it is already at angle 30 and it is commanded to go
to angle 30, It will stay there and will not move 30 degrees move as it is already at 
angle 30. The lowest delay to the servo is 500 microseconds which corresponds to commanding
the servo to move to angle 0. To scale out the delays between 500 and 2500 and mapping some
number in microseconds to an angel the method ConvertAngleToPulse uses a constant b which is
500 indicating the minimum angle which is 0 and a constant a which is 2000/180 and later
is comnined with the wanted angle and b is added to it to get right delay that corresponds 
to the right angle. It should be noted that after experimentaton with the servo, it was 
found out that when the angle 150 is given to servo, It goes to angle 180. This is probably
due to the volatge applied as it is 5.0 v. Hence, another constant was added so that angle
150 corresponds to 180 and the rest of the angles are cuely changed using the expression
150/180. It should be noted that even thou the servo is an analog servo, we use the digital
pins to work with it.
*/

#include "Arduino.h"
#include "KiwiServo.h"

KiwiServo::KiwiServo(int sigPin) {
    _sigPin = sigPin;
    pinMode(_sigPin, OUTPUT); //setting the signal pin as output as we need it to apply
    //volatge to the servo
    _delay = 25; //default value of delays. However, there is a setter that can change this
}

int KiwiServo::convertAngelToPulse(float angle) {
    float a=2000/180;  //Angle mapper
    float b=500;  //minimum delay (corresponds to degree 0)
    float convertedAngle = angle*(150.0/180.0); //converting angles from range 0 to 180 to a 
    //range 0 to 150
    return(((a)*(convertedAngle))+b); //caculating the corresponding microseconds for the angle and 
    //returning it as the delay to be made
}

void KiwiServo::goTo(float angle) {
    digitalWrite(_sigPin, HIGH); //applying voltage to the servo by 
    //setting the signal pin high
    delayMicroseconds(convertAngelToPulse(angle)); //delaying y the right amount of time in
    //microseconds, so that servo goes to the required angle defined in the method argument
    //this time delay could be identidies as a pulse, hence the name convertAngleToPulse
    //is used for converting the angle
    digitalWrite(_sigPin, LOW); //setting the volatge low again, so that the no volatge is 
    //applied to the servo
}

void KiwiServo::spin() {
    //thid for loop spins the servo from 0 to 180
    for(int i=0;i<=180;i++){
      goTo(i);
      delay(_delay);
    }
    //this fro loop spins the servo from 180 to 0
    for(int i=180;i>=0;i--){
      goTo(i);
      delay(_delay); 
    }
}

void KiwiServo::setDelay(int newDelay) { _delay = newDelay; }

int KiwiServo::getDelay() { return _delay; }