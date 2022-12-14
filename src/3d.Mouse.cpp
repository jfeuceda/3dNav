#include <Arduino.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <FastLED.h>

#define LED_TYPE	WS2812B
#define COLOR_ORDER	GBR

const int CALIBRATION = 2;
const int BUTTON = 8;
const int NUM_LEDS = 4;
const int DATA_PIN = 6;
const int THRESHOLD = 5;
int BRIGHTNESS = 64;
int vx, vy, vz; 
bool swState, swStatePrev = true;
bool isPanning =false;

CRGB leds[NUM_LEDS];

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;

void setup() 
{
	Serial.begin(9600);
	Wire.begin();
	Mouse.begin();											//iniciando Mouse
	Keyboard.begin();										//iniciando Teclado
	pinMode(BUTTON,INPUT_PULLUP);							//definiendo pin para boton como pullup
	mpu.initialize();
	if (!mpu.testConnection()) {
		while (1);
    }
	delay(1000);											//power-up safety delay 4
	FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
	FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
	mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

	vx = (gx+300)/200;  
	vy = (gy+300)/200;	//se requiere probar los valores 
	vz = (gz+100)/200;	//agregado se requiere probar los valores

	fill_solid(leds, NUM_LEDS, CRGB::Green);
	FastLED.show();

	swState = digitalRead(BUTTON);

	if(swState == HIGH && swStatePrev ==LOW) {
		Serial.println(isPanning);
		isPanning = !isPanning;
		fill_solid(leds, NUM_LEDS, CRGB::Blue);
		FastLED.show();
	}
	swStatePrev = swState;

	if (vx > THRESHOLD || vx < - THRESHOLD) {
		if(!isPanning) Keyboard.press(KEY_LEFT_SHIFT);
		Mouse.press(MOUSE_MIDDLE);
		Mouse.move(vx/CALIBRATION,0,0);
	}
	if (vy > THRESHOLD || vy < - THRESHOLD) {
		if(!isPanning) Keyboard.press(KEY_LEFT_SHIFT);
		Mouse.press(MOUSE_MIDDLE);
		Mouse.move(0,vy/CALIBRATION,0);
	}
	if (vz > THRESHOLD || vz < - THRESHOLD) {
		Mouse.move(0,0,vz/CALIBRATION);
	}
	if (vx <= THRESHOLD && vx >= -THRESHOLD && vy <= THRESHOLD && vy >= -THRESHOLD) {
		Keyboard.releaseAll();
		Mouse.release(MOUSE_MIDDLE);
	}
	delay(20);
}
