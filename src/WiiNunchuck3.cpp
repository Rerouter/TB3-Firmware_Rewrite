#include <Arduino.h>
#include "WiiNunchuck3.h"
#include "Wire.h"

const boolean USE_NEW_WAY_INIT = true; // see http://wiibrew.org/wiki/Wiimote#The_New_Way.  If set to 0, this library won't work for third party nunchucks
const uint8_t WII_TELEGRAM_LEN  = 6;
const uint8_t WII_NUNCHUCK_TWI_ADR = 0x52;

const boolean DEBUG = false; // Use serial port debugging if defined.

WiiNunchuck3::WiiNunchuck3() {}

void WiiNunchuck3::send_zero()
{
	// I don't know why, but it only works correct when doing this exactly 3 times
	// otherwise only each 3rd call reads data from the controller (cnt will be 0 the other times)
	for (byte i = 0; i < 3; i++)
	{
		Wire.beginTransmission(WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
		// Wire.write (0x00); // sends one byte
		Wire.write((uint8_t)0); // sends one byte
		Wire.endTransmission(); // stop transmitting
	}
}

void WiiNunchuck3::clearTwiInputBuffer(void)
{
	// clear the receive buffer from any partial data
	while (Wire.available())
		Wire.read();
}

void WiiNunchuck3::setpowerpins()
{
// Uses port C (analog in) pins as power & ground for Nunchuck
#define pwrpin PORTC3
#define gndpin PORTC2
	DDRC |= _BV(pwrpin) | _BV(gndpin);
	PORTC &= ~_BV(gndpin);
	PORTC |= _BV(pwrpin);
	delay(100); // wait for things to stabilize
}

void WiiNunchuck3::send_request()
{
	// Send a request for data to the nunchuck
	Wire.beginTransmission(0x52); // transmit to device 0x52
	// Wire.write(0x00);// sends one byte
	Wire.write((uint8_t)0);
	Wire.endTransmission(); // stop transmitting
}

char WiiNunchuck3::decode_byte(char x)
{
// Decode data format that original Nunchuck uses with old init sequence. This never worked with
// other controllers (e.g. wireless Nunchuck from other vendors)
	if (USE_NEW_WAY_INIT){
		return x;
	}
	else{
		x = (x ^ 0x17) + 0x17;
	}
}


int WiiNunchuck3::getData()
{
	// Receive data back from the nunchuck,
	// returns 1 on successful read. returns 0 on failure
	Wire.requestFrom(WII_NUNCHUCK_TWI_ADR, WII_TELEGRAM_LEN); // request data from nunchuck

	for (cnt = 0; (cnt < WII_TELEGRAM_LEN) && Wire.available(); cnt++)
	{
		nunchuck_buf[cnt] = decode_byte(Wire.read()); // receive byte as an integer
	}

	clearTwiInputBuffer();
	if (cnt >= WII_TELEGRAM_LEN)
	{
		send_zero();
		return 1; // success
	}
	return 0; // failure
}

void WiiNunchuck3::init(int power)
{
	// do we need to power the nunchuck?
	if (power)
	{
		setpowerpins();
	}

	// initialize the I2C system, join the I2C bus,
	// and tell the nunchuck we're talking to it
	unsigned short timeout = 0; // never timeout
	Wire.begin();				// join i2c bus as master
// we need to switch the TWI speed, because the nunchuck uses Fast-TWI
// normally set in hardware\libraries\Wire\utility\twi.c twi_init()
// this is the way of doing it without modifying the original files
#define TWI_FREQ_NUNCHUCK 400000L
	TWBR = ((16000000 / TWI_FREQ_NUNCHUCK) - 16) / 2;

	if (USE_NEW_WAY_INIT) {
		// disable encryption
		// look at <http://wiibrew.org/wiki/Wiimote#The_New_Way> at "The New Way"

		unsigned long time = millis();
		byte rc = 1;
		do
		{
			Wire.beginTransmission(WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
			Wire.write((uint8_t)0xF0);					  // sends memory address
			Wire.write((uint8_t)0x55);					  // sends data.
			if (Wire.endTransmission() == 0)			  // stop transmitting
			{
				Wire.beginTransmission(WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
				Wire.write((uint8_t)0xFB);					  // sends memory address
				Wire.write((uint8_t)0x00);					  // sends sent a zero.
				if (Wire.endTransmission() == 0)			  // stop transmitting
				{
					rc = 0;
				}
			}
		} while (rc != 0 && (!timeout || ((millis() - time) < timeout)));
	}
	else {
		// look at <http://wiibrew.org/wiki/Wiimote#The_Old_Way> at "The Old Way"
		Wire.beginTransmission(WII_NUNCHUCK_TWI_ADR); // transmit to device 0x52
													// Wire.write (0x40); // sends memory address
		// Wire.write (0x00); // sends sent a zero.
		Wire.write((uint8_t)64); // this is x40
		Wire.write((uint8_t)0);	 // sends sent a zero.
		Wire.endTransmission();	 // stop transmitting
	}
	// Sometimes the first request seems to get garbage data.
	// Get some data now so when the main program calls getData(), it will get good data.
	getData();
}

void WiiNunchuck3::printData()
{
	// Print the input data we have recieved
	// accel data is 10 bits long
	// so we read 8 bits, then we have to add
	// on the last 2 bits.  That is why I
	// multiply them by 2 * 2
	static int i = 0;
	int joy_x_axis = nunchuck_buf[0];
	int joy_y_axis = nunchuck_buf[1];
	int accel_x_axis = nunchuck_buf[2] * 2 * 2;
	int accel_y_axis = nunchuck_buf[3] * 2 * 2;
	int accel_z_axis = nunchuck_buf[4] * 2 * 2;

	boolean z_button = false;
	boolean c_button = false;

	// byte nunchuck_buf[5] contains bits for z and c buttons
	// it also contains the least significant bits for the accelerometer data
	// so we have to check each bit of byte outbuf[5]
	if ((nunchuck_buf[5] >> 0) & 1)
		z_button = true;
	if ((nunchuck_buf[5] >> 1) & 1)
		c_button = true;

	if ((nunchuck_buf[5] >> 2) & 1)
		accel_x_axis += 2;
	if ((nunchuck_buf[5] >> 3) & 1)
		accel_x_axis += 1;

	if ((nunchuck_buf[5] >> 4) & 1)
		accel_y_axis += 2;
	if ((nunchuck_buf[5] >> 5) & 1)
		accel_y_axis += 1;

	if ((nunchuck_buf[5] >> 6) & 1)
		accel_z_axis += 2;
	if ((nunchuck_buf[5] >> 7) & 1)
		accel_z_axis += 1;

	Serial.print(i, DEC);
	Serial.print("\t");

	Serial.print("joy:");
	Serial.print(joy_x_axis, DEC);
	Serial.print(",");
	Serial.print(joy_y_axis, DEC);
	Serial.print("  \t");

	Serial.print("acc:");
	Serial.print(accel_x_axis, DEC);
	Serial.print(",");
	Serial.print(accel_y_axis, DEC);
	Serial.print(",");
	Serial.print(accel_z_axis, DEC);
	Serial.print("\t");

	Serial.print("but:");
	Serial.print(z_button, DEC);
	Serial.print(",");
	Serial.print(c_button, DEC);

	Serial.print("\r\n"); // newline
	i++;
}

void WiiNunchuck3::clear()
{
    // Explicitly set each byte to zero
    nunchuck_buf[0] = 0;  // Joystick X-axis
    nunchuck_buf[1] = 0;  // Joystick Y-axis
    nunchuck_buf[2] = 0;  // Accelerometer X-axis (high bits)
    nunchuck_buf[3] = 0;  // Accelerometer Y-axis (high bits)
    nunchuck_buf[4] = 0;  // Accelerometer Z-axis (high bits)
    nunchuck_buf[5] = 0xC0;  // No buttons pressed, accelerometer low bits are zero
}


bool WiiNunchuck3::zbutton()
{
	// returns zbutton state: 1=pressed, 0=not pressed
	return !(nunchuck_buf[5] >> 0 & 1);
}

bool WiiNunchuck3::cbutton()
{
    // Returns cbutton state: 1=pressed, 0=not pressed
    return !(nunchuck_buf[5] >> 1 & 1);
}

int WiiNunchuck3::joyx()
{
	// returns value of x-axis joystick
	return nunchuck_buf[0];
}

int WiiNunchuck3::joyy()
{
	// returns value of y-axis joystick
	return nunchuck_buf[1];
}

int WiiNunchuck3::accelx()
{
    // Returns value of x-axis accelerometer
    // The last two bits are packed into the 3rd and 4th bits of nunchuck_buf[5]
    return (nunchuck_buf[2] << 2) | ((nunchuck_buf[5] >> 2) & 0x03);
}

int WiiNunchuck3::accely()
{
    // Returns value of y-axis accelerometer
    // The last two bits are packed into the 5th and 6th bits of nunchuck_buf[5]
    return (nunchuck_buf[3] << 2) | ((nunchuck_buf[5] >> 4) & 0x03);
}

int WiiNunchuck3::accelz()
{
    // Returns value of z-axis accelerometer
    // The last two bits are packed into the first two bits of nunchuck_buf[5]
    return (nunchuck_buf[4] << 2) | ((nunchuck_buf[5] >> 6) & 0x03);
}

int WiiNunchuck3::vibration()
{
	// calculates the "total vibration"
	// does NOT require a getdata() call first
	// takes 10 samples, finds the min and max, figures out the range for each accelerometer and adds them
	int accelerometers[3][2] = {{1024, 0}, {1024, 0}, {1024, 0}}; // [x,y,z][min,max] - min's initialized to 1024, max initialized to 0

	// gather 10 samples of accelerometer data, and record the highest and lowest values
	for (int loop = 0; loop < 10; loop++)
	{
		getData();

		int x = accelx();
		int y = accely();
		int z = accelz();

		if (DEBUG)
		{
			// :DEBUGING:
			Serial.print("X/Y/Z values: ");
			Serial.print(x);
			Serial.print("\t");
			Serial.print(y);
			Serial.print("\t");
			Serial.println(z);
		}

		if (x < accelerometers[0][0])
		{
			accelerometers[0][0] = x;
		}
		if (x > accelerometers[0][1])
		{
			accelerometers[0][1] = x;
		}

		if (y < accelerometers[1][0])
		{
			accelerometers[1][0] = y;
		}
		if (y > accelerometers[1][1])
		{
			accelerometers[1][1] = y;
		}

		if (z < accelerometers[2][0])
		{
			accelerometers[2][0] = z;
		}
		if (z > accelerometers[2][1])
		{
			accelerometers[2][1] = z;
		}
		if (DEBUG)
		{
			// :DEBUGING: print accelerometer min/max
			Serial.print("Accelerometer X min/max ");
			Serial.print(accelerometers[0][0]);
			Serial.print("/");
			Serial.println(accelerometers[0][1]);

			Serial.print("Accelerometer y min/max ");
			Serial.print(accelerometers[1][0]);
			Serial.print("/");
			Serial.println(accelerometers[1][1]);

			Serial.print("Accelerometer z min/max ");
			Serial.print(accelerometers[2][0]);
			Serial.print("/");
			Serial.println(accelerometers[2][1]);
		}
	}

	int xdiff = accelerometers[0][1] - accelerometers[0][0];
	int ydiff = accelerometers[1][1] - accelerometers[1][0];
	int zdiff = accelerometers[2][1] - accelerometers[2][0];

	if (DEBUG)
	{
		// Debugging
		Serial.print("Ranges: ");
		Serial.print(xdiff);
		Serial.print("\t");
		Serial.print(ydiff);
		Serial.print("\t");
		Serial.println(zdiff);

		Serial.print("Final value: ");
		Serial.println(xdiff + ydiff + zdiff);
	}
	return xdiff + ydiff + zdiff;
}

void WiiNunchuck3::calibrate()
{
	// this function returns the average values over 10 samples, to be used for calibrating the nunchuck joystick
	// the joystick must be at rest when this is done
	getData();
	centeredJoyX = joyx();
	centeredJoyY = joyy();

	Serial.print("Calibrated: ");
	Serial.print(centeredJoyX);
	Serial.print("/");
	Serial.println(centeredJoyY);
}

int WiiNunchuck3::digitalx(int threshold)
{
    // Returns -1 for joystick left, 1 for joystick right, 0 for centered
    int calibratedjoyx = joyx() - centeredJoyX;  // Calculate deviation from center
    if (calibratedjoyx < -threshold)     {return -1;}  // Joystick is left
    else if (calibratedjoyx > threshold) {return 1;}  // Joystick is right
    return 0;  // Joystick is centered
}

int WiiNunchuck3::digitaly(int threshold)
{
	// returns 0 for centered, 1 for joystick up, 2 for joystick down
	// threshold is how far from center it has to be
	int calibratedjoyy = joyy() - centeredJoyY;
    if (calibratedjoyy < -threshold)     {return -1;}  // Joystick is left
    else if (calibratedjoyy > threshold) {return 1;}  // Joystick is right
    return 0;  // Joystick is centered
}

// create the object
WiiNunchuck3 Nunchuck;
