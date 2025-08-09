// Define the analog pins for the sensors
#define MQ7_PIN A0      
#define MQ9_PIN A1      
#define MQ131_PIN A2    
#define MQ136_PIN A3    

// Constants for calibration (to be adjusted based on your setup)
float MQ7_RL = 10.0;     
float MQ7_RO = 10.0;     

float MQ9_RL = 10.0;     
float MQ9_RO = 10.0;     
float MQ131_RL = 10.0;   
float MQ131_RO = 10.0;   

float MQ136_RL = 10.0;   
float MQ136_RO = 10.0;   

void setup() {
  Serial.begin(9600);  // Initialize serial communication for output
}

void loop() {
  // Read analog values from sensors
  int mq7_value = analogRead(MQ7_PIN);      
  int mq9_value = analogRead(MQ9_PIN);      
  int mq131_value = analogRead(MQ131_PIN);  
  int mq136_value = analogRead(MQ136_PIN);  

  
  float mq7_voltage = mq7_value * (5.0 / 1023.0);
  float mq9_voltage = mq9_value * (5.0 / 1023.0);
  float mq131_voltage = mq131_value * (5.0 / 1023.0);
  float mq136_voltage = mq136_value * (5.0 / 1023.0);

  // Convert voltages to resistance values
  float mq7_rs = MQ7_RL * (5.0 - mq7_voltage) / mq7_voltage;
  float mq9_rs = MQ9_RL * (5.0 - mq9_voltage) / mq9_voltage;
  float mq131_rs = MQ131_RL * (5.0 - mq131_voltage) / mq131_voltage;
  float mq136_rs = MQ136_RL * (5.0 - mq136_voltage) / mq136_voltage;

  // Apply calibration formulas for each sensor
  // MQ7 for NO2 in PPB (Nitrogen Dioxide)
  float no2_ppb_mq7 = getGasConcentrationPPB(mq7_rs / MQ7_RO, 102.2, -2.473);  // Example calibration formula for NO2

  // MQ9 for CO in PPM (Carbon Monoxide)
  float co_ppm_mq9 = getGasConcentrationPPM(mq9_rs / MQ9_RO, 116.6020682, -2.769034857);  // Example calibration formula for CO

  // MQ131 for Ozone in PPB (O3)
  float ozone_ppb_mq131 = getGasConcentrationPPB(mq131_rs / MQ131_RO, 9.4783, -1.513);  // Example calibration formula for Ozone

  // MQ136 for SO2 in PPB (Sulfur Dioxide)
  float so2_ppb_mq136 = getGasConcentrationPPB(mq136_rs / MQ136_RO, 102.2, -2.473);  // Example calibration formula for SO2

  // Output the sensor data to the serial monitor
  Serial.print("NO2 (MQ7): ");
  Serial.print(no2_ppb_mq7);
  Serial.println(" PPB");

  Serial.print("CO (MQ9): ");
  Serial.print(co_ppm_mq9);
  Serial.println(" PPM");

  Serial.print("Ozone (MQ131): ");
  Serial.print(ozone_ppb_mq131);
  Serial.println(" PPB");

  Serial.print("SO2 (MQ136): ");
  Serial.print(so2_ppb_mq136);
  Serial.println(" PPB");

  delay(2000);  // Wait for 2 seconds before taking the next reading
}

// Function to calculate gas concentration in PPM (for MQ9 - CO)
float getGasConcentrationPPM(float ratio, float a, float b) {
  return a * pow(ratio, b);  // Using the formula: concentration = a * (Rs/Ro)^b
}

// Function to calculate gas concentration in PPB (for MQ7 - NO2, MQ131 - Ozone, MQ136 - SO2)
float getGasConcentrationPPB(float ratio, float a, float b) {
  return a * pow(ratio, b);  // Using the formula for concentration in PPB
}
