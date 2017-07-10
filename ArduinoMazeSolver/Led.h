#pragma once

void lightLed(byte ledIndex)
{
	digitalWrite(ledPins[ledIndex], HIGH);
}

void turnOffAllLeds()
{
	for (byte i = 0; i < sizeof(ledPins); i++)
	{
		digitalWrite(ledPins[i], LOW);
	}
}

void ledDirection(byte ledDir)
{
	switch (ledDir)
	{
	case diversionChecking:
		lightLed(1);
		break;
	case none:
		lightLed(0);
		lightLed(3);
		break;
	case backward:
		lightLed(2);
		lightLed(3);
		break;
	case left:
		lightLed(2);
		break;
	case forward:
		lightLed(0);
		break;
	case right:
		lightLed(3);
		break;
	}
}

