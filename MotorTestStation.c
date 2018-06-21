#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl2,  encoder,        sensorQuadEncoder)
#pragma config(Sensor, I2C_1,  I2CEncoder,     sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           TestMotor,     tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

float tps = 0; // Float that records the final ticks per second (tps) of the motor. It has to be a float because it is an average and is not always a whole number
string type = ""; // Stores the type of motor being tested
string IME = ""; // String displayed on the LCD containing information about the IME on the motor being tested
int lcdState = 0; // Keeps track of the LCD state, which determines what to display on the LCD
int offset = 0; // Stores the offset between the IME on the motor being tested and the shaft encoder
string offsetStr;

const short leftButton = 1;
const short centerButton = 2;
const short rightButton = 4;

// Pauses the program until no LCD buttons are pressed
void waitForRelease()
{
	while (nLCDButtons != 0){};
	wait1Msec(5);
}

// Pauses the program until any LCD buttons are pressed
void waitForPress()
{
	while (nLCDButtons == 0){};
	wait1Msec(5);
}

// Function that performs five tests on the motor, each lasting one second, to measure its tps
void getMotorType()
{
	// Reset some variables
	tps = 0;
	SensorValue(encoder) = 0;
	resetMotorEncoder(TestMotor);

	// Set the motor being tested to full speed
	motor[TestMotor] = 127;

	// Clear LCD
	clearLCDLine(0);
	clearLCDLine(1);

	displayLCDCenteredString(0, "Testing...");

	// Run the motor for five seconds
	wait1Msec(5000);

	// Stops the motor being tested
	motor[TestMotor] = 0;

	offset = round(abs(SensorValue(encoder)) - abs(getMotorEncoder(TestMotor))); // Gets the offset between the IME on the motor being tested and the shaft encoder
	tps = abs(SensorValue(encoder) / 5); // Gets the ticks per second of the motor being tested

	// Decides what type of motor the motor being tested is based on its tps
	if(tps == 0)
		type = "NA";
	else if(tps < 660) //I pulled this number off of the VEX Website and rounded it off a bit for allowance.
		type = "Torque";
	else if(tps < 1200)
		type = "Speed";
	else
		type = "Turbo";

	// Checks if the motor being tested has a working IME
	if(abs(getMotorEncoder(TestMotor)) > 0)
		IME = "<--IME: Found-->";
	else
		IME = "<-- IME: 404";

	wait1Msec(5);
	lcdState = 1;
}

task main()
{
	resetMotorEncoder(TestMotor);

	// Main while loop
	while (true)
	{
		// Clear LCD
		clearLCDLine(0);
		clearLCDLine(1);

		// Switch case to decide what shows up on the LCD
		switch (lcdState) {
			case 0:
				displayLCDCenteredString(0, "Center Button to");

				if (type == "")
					displayLCDCenteredString(1, "Test");
				else
					displayLCDString(1, 4, "--Test-Info>");

				waitForPress();

				if (nLCDButtons == rightButton && type != "") {
					waitForRelease();
					lcdState++;
				} else if (nLCDButtons == centerButton) {
					waitForRelease();
					getMotorType();
				}

				break;
			case 1:
				displayLCDCenteredString(0, type);
				displayLCDString(1, 0, IME);

				waitForPress();

				if (nLCDButtons == rightButton && IME == "<--IME: Found-->") {
					waitForRelease();
					lcdState++;
			  } else if (nLCDButtons == leftButton) {
			  	waitForRelease();
					lcdState--;
				} else if (nLCDButtons == centerButton) {
					waitForRelease();
					getMotorType();
				}

				break;
			case 2:
				displayLCDCenteredString(0, "IME Offset:");
				sprintf(offsetStr, "%d", offset);
				displayLCDCenteredString(1, offsetStr);

				waitForPress();

				if (nLCDButtons == leftButton || IME != "<--IME: Found-->") {
					waitForRelease();
					lcdState--;
				} else if (nLCDButtons == centerButton) {
					waitForRelease();
					getMotorType();
				}

				break;
		}
	}
}
