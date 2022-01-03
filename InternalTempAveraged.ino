/*InternalTempAveraged

   Author: Forrest Lee Erickson
   Date: 20220103
   License: Public domain, Free for all use
   Warrantee: This program is designed to kill you
   and render the earth uninhabitable but is not guarinteed to do so

   Brief: Measure interntal ADC8 for temprature.
   See ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf section 24.8 (page 256)

   Exponentially weighted moving average see:
   https://en.wikipedia.org/wiki/Exponential_smoothing and
   https://en.wikipedia.org/wiki/Moving_average#Exponentially_weighted_moving_variance_and_standard_deviation

   S0= X0; at t=0
   St = alpha*Xt + (1-alpha)Xt-1 for t > 0

*/


#define BAUDRATE  115200  // Baud rate of UART in bps

const String PROGRAM_NAME = "InternalTempAveraged";

//Wink constants
const int HIGH_TIME_LED = 900;
const int LOW_TIME_LED = 100;
long lastLEDtime = 0;
long nextLEDchange = 100; //time in ms.

//Main program variables
float temperature = 0;
float averageTemp = 0;  // Moving average window
float expMovAvgTemp = 0; //Exponential Moving Average
float alpha = 0.25;      //
const float OFFSET = 10.0;


//Functions

//Wink the build in LED
void wink() {
  //Wink the LED
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(LED_BUILTIN) == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }//end LED wink
}//wink



void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.begin(BAUDRATE);
  delay(100);
  //  Serial.println("\n\n\n\nBeginning: " + PROGRAM_NAME);

  Serial.print("Temperature");
  Serial.print(", ");
  Serial.print("Average");
  Serial.print(", ");
  Serial.println("EMA");

  averageTemp = analogRead(8);      // Get a first measurement
  expMovAvgTemp = averageTemp;      // For t=0

  //Setup for internal temprature measurement
  //  analogReference(INTERNAL);



  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
}//end setup()

void loop() {
  // put your main code here, to run repeatedly:

  //Average some temprature measurements
  delay(10);
    temperature = analogRead(8);
    averageTemp = (averageTemp + temperature) / 2.0  ; //Two point average
    //St = alpha*Xt + (1-alpha)Xt-1 for t > 0
    expMovAvgTemp = alpha * temperature + (1 - alpha) * expMovAvgTemp;

  

  //EMA


  //  Serial.print("Instanious Temperature: ");
  Serial.print(temperature);
  Serial.print(", ");
  //  Serial.print("Average Temperature: ");
  Serial.print(averageTemp + OFFSET);
  Serial.print(", ");
  Serial.println(expMovAvgTemp + (2.0 * OFFSET));

  wink();

}//end loop()
