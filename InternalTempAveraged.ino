/*InternalTempAveraged

   Author: Forrest Lee Erickson
   Date: 20220103
   License: Public domain, Free for all use
   Warranted: This program is designed to kill you
   and render the earth uninhabitable but is not guaranteed to do so

   Brief: Measure internal ADC8 for temperature.
   See ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf section 24.8 (page 256)

   Exponentially weighted moving average see:
   https://en.wikipedia.org/wiki/Exponential_smoothing and
   https://en.wikipedia.org/wiki/Moving_average#Exponentially_weighted_moving_variance_and_standard_deviation

   S0= X0; at t=0
   St = alpha*Xt + (1-alpha)Xt-1 for t > 0

   sigma = Xt -EMAXt-1
   EMVar = )1-alpha)(EMVar-1 + alpha sigma^2)

   User should display data using the Arduino IDE serial plotter feature.
   Note: the two point temprature average is offset.
*/


#define BAUDRATE  115200  // Baud rate of UART in bps

const String PROGRAM_NAME = "InternalTempAveraged";

//Wink constants
const int HIGH_TIME_LED = 900;
const int LOW_TIME_LED = 100;
long lastLEDtime = 0;
long nextLEDchange = 100; //time in ms.

//Main program variables
const int SAMPLE_TIME = 10;  //mSec between samples.
float temperature = 0;
float averageTemp = 0;  // Moving average window
float expMovAvgTemp = 0; //Exponential Moving Average
float varanceEMATemp = 0; //Variance of the EMA
float sigma = 0;
double standardDev = 0;
//float alpha = 0.125;      //
float alpha = 0.0625;      //
const float OFFSET = 5.0; //An offset so that we can distinguish one plot from another
long lastTEMPtime = 0;
long nextTEMPsample = 20; //time in ms.

//Functions

//Average and print some temperature measurements
void updateTemperature(void) {
  if (((millis() - lastTEMPtime) > nextTEMPsample) || (millis() < lastTEMPtime)) {
    lastTEMPtime = millis();
    temperature = analogRead(8);
    averageTemp = (averageTemp + temperature) / 2.0  ; //Two point average
    //St = alpha*Xt + (1-alpha)Xt-1 for t > 0
    expMovAvgTemp = alpha * temperature + (1.0 - alpha) * expMovAvgTemp;
    sigma = temperature - expMovAvgTemp;
    varanceEMATemp = (1.0 - alpha) * (varanceEMATemp + alpha * sigma * sigma) ;
    standardDev = sqrt(varanceEMATemp);
    printTemp2ptAvgEMAStd();
  }// end if
}// end update temperature

//Print graphs of the updated temprature
void printTemp2ptAvgEMAStd(void) {
  Serial.print(temperature);
  Serial.print(", ");
  Serial.print(averageTemp - OFFSET);
  Serial.print(", ");
  Serial.print(expMovAvgTemp );
  Serial.print(", ");
  Serial.print(expMovAvgTemp + standardDev);
  Serial.print(", ");
  Serial.println(expMovAvgTemp - standardDev);

  //  //Print offset varance
  //  Serial.print(expMovAvgTemp - (2.0 * OFFSET)+ varanceEMATemp);
  //  Serial.print(", ");
  //  Serial.println(expMovAvgTemp - (2.0 * OFFSET)- varanceEMATemp);

  // Print offset standardDev
  //  Serial.print(expMovAvgTemp - (2.0 * OFFSET)+ standardDev);
  //  Serial.print(", ");
  //  Serial.println(expMovAvgTemp - (2.0 * OFFSET)- standardDev);

}//end printTemp2ptAvgEMAStd()

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

  //Legend for serial plotter
  Serial.print("Temperature");
  Serial.print(", ");
  Serial.print("Average");
  Serial.print(", ");
  Serial.print("EMA=" + String(alpha) );
  Serial.print(", ");
  //  Serial.println("Varance");
  Serial.print("+StandardDev");
  Serial.print(", ");
  //  Serial.println("Varance");
  Serial.println("-StandardDev");

  averageTemp = analogRead(8);      // Get a first measurement
  expMovAvgTemp = averageTemp;      // For t=0

  //analogReference(INTERNAL); //Setup for internal 1.1V reference for ADC measurement
  //analogReference(EXTERNAL); //Setup for external voltage on Vref pin.

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
}//end setup()

void loop() {
  // put your main code here, to run repeatedly:

  updateTemperature();

  wink();

}//end loop()
