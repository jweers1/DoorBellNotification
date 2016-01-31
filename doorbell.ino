/* Function prototypes -------------------------------------------------------*/
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);
int ledstatus;

int doorBell=LOW;
int lastDoorBell = LOW;

long TimeOfNotification;
TCPClient myTCP;
int sentOnce;

/* This function is called once at start up ----------------------------------*/
void setup()
{
	//Setup the Tinker application here

	//Register all the Tinker functions
	Particle.function("digitalread", tinkerDigitalRead);
	Particle.function("digitalwrite", tinkerDigitalWrite);
	Particle.function("analogread", tinkerAnalogRead);
	Particle.function("analogwrite", tinkerAnalogWrite);
	//Particle.publish("Johns Photon is Online w/ custome Tinker.");
	Particle.function("LedOn",johnsLedOn);
	Particle.function("LedOff",johnsLedOff);

    Particle.variable("LedStatus",ledstatus);
	Particle.variable("DoorBell",doorBell);

	Particle.publish("Doorbell","online with doorbell v1 + tinker");
	pinMode(4, INPUT);

	//Serial.begin(9600);

}

void sendToProwl()
{

   if (myTCP.connect("api.prowlapp.com", 80))
   {
        //Serial.println(F("Outgoing Notification"));
        myTCP.write("POST http://api.prowlapp.com/publicapi/add?apikey=<key>a&event=Doorbell%20Pressed&priority=1&application=ParticleDoorbell&description=Doorbell%20Pressed&Content Type: application/x-www-form-urlencoded\r\n\r\n");
        myTCP.stop();
        Particle.publish("Prowl","Sending Prowl Notification");
    } else {
        Particle.publish("Prowl","unable to connect");
    }
    TimeOfNotification = (millis()/1000);
    sentOnce = 1;

}

int johnsLedOn(String pin){
pinMode(7, OUTPUT);
digitalWrite(7, HIGH);
ledstatus=HIGH;
Particle.publish("LED","on");
return 1;
}

int johnsLedOff(String pin){
pinMode(7, OUTPUT);
digitalWrite(7, LOW);
ledstatus=LOW;
Particle.publish("LED","off");
return 1;
}


/* This function loops forever --------------------------------------------*/
void loop()
{
	//This will run in a loop
  doorBell=digitalRead(4);
  if (doorBell != lastDoorBell){
        if (doorBell == LOW) {
              Particle.publish("Doorbell Status", "LOW");
        } else {
              Particle.publish("Doorbell Status", "HIGH");
        }

		if (doorBell==HIGH) {
		    sendToProwl();
			johnsLedOn("");
		} else {
			johnsLedOff("");
		}
  }

	//Serial.print("doorbell=");
	//Serial.println(doorBell);
	//Serial.print("lastDoorBell=");
	//Serial.println(lastDoorBell);
	lastDoorBell=doorBell;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerDigitalRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerDigitalWrite(String command)
{
	bool value = 0;
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(command.substring(3,7) == "HIGH") value = 1;
	else if(command.substring(3,6) == "LOW") value = 0;
	else return -2;

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;
	}
	else return -3;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogRead
 * Description    : Reads the analog value of a pin
 * Input          : Pin
 * Output         : None.
 * Return         : Returns the analog value in INT type (0 to 4095)
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerAnalogRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		return -3;
	}
	else if (pin.startsWith("A"))
	{
		return analogRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogWrite
 * Description    : Writes an analog value (PWM) to the specified pin
 * Input          : Pin and Value (0 to 255)
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerAnalogWrite(String command)
{
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	String value = command.substring(3);

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;
	}
	else return -2;
}
