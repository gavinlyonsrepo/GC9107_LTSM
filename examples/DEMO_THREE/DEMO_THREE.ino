/*!
	@file   DEMO_THREE.ino
	@brief  Demo: Flight Simulator
	@author Gavin Lyons.
	@details
			Demonstrates the GC9107 60x160 portrait display as a flight simulator.
			Hardware SPI
	@test
			-# 403 Flight Simulator
*/

// Section :: libraries
#include "GC9107_LTSM.hpp"
// Fonts needed
#include <fonts_LTSM/FontDefault_LTSM.hpp>

/// @cond

#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif

// Section :: Globals
GC9107_LTSM myTFT;
bool bhardwareSPI = true;      // true for Hardware SPI , false for software SPI
uint16_t SCREEN_WIDTH = 60;    // Screen width in pixels
uint16_t SCREEN_HEIGHT = 160;  // Screen height in pixels

/// @cond

// Demo
void ControlPanel(uint16_t countLimit = 150);
// Demo 1-1
void gaugeDemo(uint16_t count = 0);
void drawPointerHelper(int16_t val, uint8_t x, uint8_t y, uint8_t r, uint16_t color);
void drawGaugeMarkers(uint8_t centerX, uint8_t centerY, uint8_t radius, int startAngle, int endAngle, float scaleFactor);
void drawPointer(const int16_t &val, const int16_t &oldVal, uint8_t x, uint8_t y, uint8_t r, uint16_t color, uint16_t bcolor);
// Demo 1-2
void arcGauge(uint16_t count = 0);
// Demo 1-3
// Gauge parameters
const int GAUGE_WIDTH = 8;
const int GAUGE_HEIGHT = 39;
const int GAUGE_X_START = 100;
const int GAUGE_Y_START = 5;
const int GAUGE_SPACING = 20;
// Store previous values to prevent redundant redraws
float prevVal1 = -1, prevVal2 = -1, prevVal3 = -1;
void updateGauges(float phase);
void drawGauge(int x, int y, uint16_t color, float value);
void drawGaugeDemo(uint16_t count = 0);
void drawGaugeBorder(int x, int y);

void setup(void) {
	Serial.begin(38400);
	delay(1000);
	Serial.println("Start Demo 1");
	//*** USER OPTION 0 SPI GPIO + TYPE ***
	int8_t DC_TFT = 5;
	int8_t RST_TFT = 4;
	int8_t CS_TFT = 15;
	if (bhardwareSPI == true) {          // hw spi
		uint32_t TFT_SCLK_FREQ = 8000000;  // Spi freq in Hertz
		myTFT.TFTsetupGPIO_SPI(TFT_SCLK_FREQ, RST_TFT, DC_TFT, CS_TFT);
	} else {                        // sw spi
		uint16_t SWSPICommDelay = 0;  // optional SW SPI GPIO delay in uS
		int8_t SDIN_TFT = 26;
		int8_t SCLK_TFT = 27;
		myTFT.TFTsetupGPIO_SPI(SWSPICommDelay, RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
	}
	// ***
	// *** USER OPTION 1 Screen Setup ***
	// Size of Display (note 60x160 display has VRAM of 128x160)
	uint16_t TFT_WIDTH = 60;    // width  size in pixels
	uint16_t TFT_HEIGHT = 160;  // height size in pixels
	// GRAM memory base mapping resolution, see GC9107_LTSM::GM_memory_base_e enum
	GC9107_LTSM::GM_memory_base_e TFTmemoryBase = GC9107_LTSM::GM_memory_base_e::MEMORY_BASE_GM_128x160;
	myTFT.TFTInitScreen(TFT_WIDTH, TFT_HEIGHT, TFTmemoryBase, GC9107_LTSM::MADCTL_FLAGS_t::RGB);
	// *** USER OPTION 2 Screen offsets in portrait***
	uint8_t X_Offset = 34;
	uint8_t Y_Offset = 0;
	myTFT.TFTsetPanelOffset(X_Offset, Y_Offset);
	// ***
	myTFT.TFTGC9107Initialize();
	myTFT.TFTsetRotation(myTFT.Degrees_270);
	myTFT.fillScreen(myTFT.C_DGREY);
}

//  Section ::  MAIN loop
void loop() {
	ControlPanel(100);
	EndDemo();
}
// *** End OF MAIN **

//  Section ::  Function Space

// --- Utilities + Misc functions ---
void EndDemo(void) {
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTPowerDown();
	Serial.println("Demo End");
}

// Demo

void ControlPanel(uint16_t countLimit) {
	Serial.print("Demo ends at: ");
	Serial.println(countLimit);
	// setup
	uint16_t count = 0;
	myTFT.fillScreen(myTFT.C_DGREY);
	myTFT.setFont(FontDefault);
	myTFT.setTextColor(myTFT.C_GREEN, myTFT.C_DGREY);
	drawScrews(3, myTFT.C_BROWN, 4);

	while (count < countLimit) {
		gaugeDemo(count);      // Left
		arcGauge(count);       // center
		drawGaugeDemo(count);  // right
		count++;
	}
	myTFT.fillScreen(myTFT.C_BLACK);
	Serial.println("Demo Over");
}

// Demo 1-1
void gaugeDemo(uint16_t count) {
	static int16_t currentValue = 150;
	static int16_t oldValue = 150;
	const int16_t minValue = 1;
	const int16_t maxValue = 255;
	uint8_t x = 25;
	uint8_t y = 25;
	uint8_t radius = 20;
	char buffer[10];
	// Draw Gauge on first pass
	if (count == 0) {
		myTFT.fillCircle(x, y, radius, myTFT.C_BLACK);
		myTFT.drawCircle(x, y, radius, myTFT.C_RED);
		drawGaugeMarkers(x, y, radius, 150, 390, 1.3);
		drawGaugeMarkers(x, y, radius, 165, 375, 1.1);
	}
	// Random number setup
	// Initialize random generator , optional
	// randomSeed(analogRead(A0));
	int step = random(-10, 11);  // Arduino built-in RNG
	currentValue += step;
	currentValue = constrain(currentValue, minValue, maxValue);
	if (oldValue != currentValue) {
		drawPointer(currentValue, oldValue, x, y, radius, myTFT.C_GREEN, myTFT.C_BLACK);
		oldValue = currentValue;
	}
	myTFT.setCursor(14, 50);
	sprintf(buffer, "%03d", currentValue);
	myTFT.print(buffer);
}

// Demo 1-1 & Demo 1-2
void drawGaugeMarkers(uint8_t centerX, uint8_t centerY, uint8_t radius, int startAngle, int endAngle, float scaleFactor) {
	int angle;
	// Loop through the specified angle range, drawing ticks every 30 degrees
	for (angle = startAngle; angle <= endAngle; angle += 30) {
		float angleRad, innerX, innerY, outerX, outerY;
		// Convert degrees to radians
		angleRad = angle * (std::numbers::pi / 180);
		// inner marker position
		innerX = cos(angleRad) * (radius / scaleFactor);
		innerY = sin(angleRad) * (radius / scaleFactor);
		// outer marker position
		outerX = cos(angleRad) * radius;
		outerY = sin(angleRad) * radius;
		// Draw marker line from inner to outer radius
		myTFT.drawLine(1 + centerX + innerX, 1 + centerY + innerY,
		               1 + centerX + outerX, 1 + centerY + outerY,
		               myTFT.C_WHITE);
	}
}

// Demo 1-1 & Demo 1-2
void drawPointer(const int16_t &currentValue, const int16_t &oldValue, uint8_t x, uint8_t y, uint8_t r, uint16_t colour, uint16_t bcolour) {
	uint16_t i;
	// If the current value is increasing
	if (currentValue > oldValue) {
		// Incrementally move the pointer from oldValue to currentValue
		for (i = oldValue; i <= currentValue; i++) {
			// Erase the previous pointer position
			drawPointerHelper(i - 1, x, y, r, bcolour);
			// Draw the pointer at the new position
			drawPointerHelper(i, x, y, r, colour);
			// Introduce a small delay for smooth animation
			if ((currentValue - oldValue) < 128) {
				delay(20);
			} else {
				delay(30);  // Slower update for small changes
			}
		}
	} else  // If the current value is decreasing
	{
		// Decrementally move the pointer from oldValue to currentValue
		for (i = oldValue; i >= currentValue; i--) {
			// Erase the previous pointer position
			drawPointerHelper(i + 1, x, y, r, bcolour);
			// Draw the pointer at the new position
			drawPointerHelper(i, x, y, r, colour);
			// Adjust delay based on the difference to control animation speed
			if ((oldValue - currentValue) >= 128) {
				delay(20);  // Faster update for large changes
			} else {
				delay(30);  // Slower update for small changes
			}
		}
	}
}

//  Demo 1-1 & Demo 1-2
void drawPointerHelper(int16_t value, uint8_t centerX, uint8_t centerY, uint8_t radius, uint16_t pointerColor) {
	float angleRad, pointerX, pointerY;
	int16_t minValue = 0;
	int16_t maxValue = 255;
	int startAngle = 150;  // Gauge starting angle in degrees
	int endAngle = 240;    // Gauge ending angle in degrees
	// Ensure the value stays within the valid range
	if (value > maxValue) value = maxValue;
	if (value < minValue) value = minValue;
	// Map value to an angle in radians based on the gauge range
	angleRad = (((float)(uint16_t)(value - minValue) / (float)(uint16_t)(maxValue - minValue) * endAngle)
	            + startAngle)
	           * (std::numbers::pi / 180);
	// Calculate pointer tip coordinates
	pointerX = cos(angleRad) * (radius / 1.35);
	pointerY = sin(angleRad) * (radius / 1.35);
	// Draw the pointer line from the center to the calculated tip position
	myTFT.drawLine(centerX, centerY, 1 + centerX + pointerX, 1 + centerY + pointerY, pointerColor);
	// Draw a small circle to represent the pointer base
	myTFT.fillCircle(centerX, centerY, 2, pointerColor);
}

// Demo 1-2
void arcGauge(uint16_t count) {
	static int16_t currentValue = 150;
	static int16_t oldValue = 150;
	char buffer[10];
	// Draw Gauge
	uint8_t x = 70;
	uint8_t y = 25;
	uint8_t radius = 20;
	const int16_t minValue = 1;
	const int16_t maxValue = 255;
	if (count == 0) {
		myTFT.fillCircle(x, y, radius, myTFT.C_BLACK);
		myTFT.drawArc(x, y, radius, 5, 150.0f, 180.0f, myTFT.C_RED);
		myTFT.drawArc(x, y, radius, 5, 181.0f, 250.0f, myTFT.C_ORANGE);
		myTFT.drawArc(x, y, radius, 5, 251.0f, 320.0f, myTFT.C_YELLOW);
		myTFT.drawArc(x, y, radius, 5, 321.0f, 35.0f, myTFT.C_GREEN);
		drawGaugeMarkers(x, y, radius, 150, 390, 1.3);
		drawGaugeMarkers(x, y, radius, 165, 375, 1.1);
		myTFT.drawCircle(x, y, radius, myTFT.C_RED);
	}
	// Random number setup
	// Initialize random generator , optional
	// randomSeed(analogRead(A0));
	int step = random(-10, 11);  // Arduino built-in RNG
	currentValue += step;
	currentValue = constrain(currentValue, minValue, maxValue);
	myTFT.setCursor(0, 100);
	if (oldValue != currentValue) {
		drawPointer(currentValue, oldValue, x, y, radius, myTFT.C_GREEN, myTFT.C_BLACK);
		oldValue = currentValue;
	}
	sprintf(buffer, "%03d", currentValue);
	myTFT.setCursor(60, 50);
	myTFT.print(buffer);
}


// Demo 1-3
void drawGaugeBorder(int x, int y) {  // Draw border once
	myTFT.drawRectWH(x - 2, y - 2, GAUGE_WIDTH + 4, GAUGE_HEIGHT + 4, myTFT.C_RED);
}

void drawGaugeDemo(uint16_t count) {
	// Draw borders once
	if (count == 0) {

		myTFT.setCursor(GAUGE_X_START, GAUGE_Y_START + GAUGE_HEIGHT + 5);
		myTFT.print("V");
		drawGaugeBorder(GAUGE_X_START, GAUGE_Y_START);

		myTFT.setCursor(GAUGE_X_START + GAUGE_SPACING, GAUGE_Y_START + GAUGE_HEIGHT + 5);
		myTFT.print("L");
		drawGaugeBorder(GAUGE_X_START + GAUGE_SPACING, GAUGE_Y_START);

		myTFT.setCursor(GAUGE_X_START + 2 * GAUGE_SPACING, GAUGE_Y_START + GAUGE_HEIGHT + 5);
		myTFT.print("P");
		drawGaugeBorder(GAUGE_X_START + 2 * GAUGE_SPACING, GAUGE_Y_START);

		prevVal1 = -1, prevVal2 = -1, prevVal3 = -1;
	}
	static float phase = 0;
	updateGauges(phase);
	phase += 0.1;
}

// Gauge update
void drawGauge(int x, int y, float value, float prevVal) {
	int fillHeight = static_cast<int>(GAUGE_HEIGHT * value);
	int prevFillHeight = (prevVal < 0) ? 0 : static_cast<int>(GAUGE_HEIGHT * prevVal);

	if (fillHeight == prevFillHeight) return;  // nothing to update

	// If shrinking, clear the difference
	if (fillHeight < prevFillHeight) {
		int clearHeight = prevFillHeight - fillHeight;
		myTFT.fillRectBuffer(
		  x,
		  y + (GAUGE_HEIGHT - prevFillHeight),
		  GAUGE_WIDTH,
		  clearHeight,
		  myTFT.C_BLACK);
	}
	// If growing, draw gradient for the new part
	if (fillHeight > prevFillHeight) {
		int growHeight = fillHeight - prevFillHeight;
		for (int i = 0; i < growHeight; i++) {
			uint8_t val = map(static_cast<int>(prevFillHeight + i), 0, GAUGE_HEIGHT - 1, 1, 127);
			uint16_t color = generateColor(val);
			myTFT.fillRectBuffer(
			  x,
			  y + (GAUGE_HEIGHT - fillHeight) + (growHeight - 1 - i),
			  GAUGE_WIDTH,
			  1,
			  color);
		}
	}
}

// Update all gauges
void updateGauges(float phase) {
	static bool firstPass = true;
	float val1, val2, val3;
	if (firstPass) {  // on first pass fill gauge interiors and text areas black
		myTFT.fillRectBuffer(GAUGE_X_START, GAUGE_Y_START, GAUGE_WIDTH, GAUGE_HEIGHT, myTFT.C_BLACK);
		myTFT.fillRectBuffer(GAUGE_X_START + GAUGE_SPACING, GAUGE_Y_START, GAUGE_WIDTH, GAUGE_HEIGHT, myTFT.C_BLACK);
		myTFT.fillRectBuffer(GAUGE_X_START + 2 * GAUGE_SPACING, GAUGE_Y_START, GAUGE_WIDTH, GAUGE_HEIGHT, myTFT.C_BLACK);
		firstPass = false;
		return;
	} else {  // Calculate values (0 → 1)
		val1 = (std::sin(phase) + 1) / 2;
		val2 = (std::cos(phase) + 1) / 2;
		val3 = ((std::sin(phase) * std::cos(phase)) + 1) / 2;
	}
	// --- Gauge 1 ---
	if (val1 != prevVal1) {
		drawGauge(GAUGE_X_START, GAUGE_Y_START, val1, prevVal1);
		prevVal1 = val1;
	}
	// --- Gauge 2 ---
	if (val2 != prevVal2) {
		drawGauge(GAUGE_X_START + GAUGE_SPACING, GAUGE_Y_START, val2, prevVal2);
		prevVal2 = val2;
	}
	// --- Gauge 3 ---
	if (val3 != prevVal3) {
		drawGauge(GAUGE_X_START + 2 * GAUGE_SPACING, GAUGE_Y_START, val3, prevVal3);
		prevVal3 = val3;
	}
}

/*!
	@brief This function is designed to return a color in the 16-bit 
		RGB format based on the input value, creating a smooth transition 
		between colors in different stages. 
	@details The gradient is calculated by 
		cycling through different color transitions as val increases, 
		allowing for smooth visual effects such as changing hues 
		or creating rainbow-like effects on an LCD. 1-127 will
		This will create a gradient color between blue and red based on the value.
*/
uint16_t generateColor(uint8_t value) {
	// Clamp to 0–127
	value = (value > 127) ? 127 : value;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	uint8_t segment = value / 32;
	switch (segment) {
		case 0:
			red = 0;
			green = 2 * (value % 32);
			blue = 31;
			break;
		case 1:
			red = 0;
			green = 63;
			blue = 31 - (value % 32);
			break;
		case 2:
			red = value % 32;
			green = 63;
			blue = 0;
			break;
		case 3:
			red = 31;
			green = 63 - 2 * (value % 32);
			blue = 0;
			break;
	}
	return (red << 11) + (green << 5) + blue;
}

/*!
	@brief  Draw a decorative screw in each corner of the panel.
	        Each screw is a small circle with a cross-slot drawn across it.
	@param  radius     Radius of the screw head in pixels
	@param  color      Colour of the screw head and slot
	@param  margin     Distance from screen edge to screw centre
*/
void drawScrews(uint8_t radius, uint16_t color, uint8_t margin) {
	uint8_t x1 = margin;
	uint8_t x2 = SCREEN_HEIGHT - 1 - margin;  // 160 wide in landscape
	uint8_t y1 = margin;
	uint8_t y2 = SCREEN_WIDTH - 1 - margin;  // 60 tall in landscape

	uint8_t corners[4][2] = {
		{ x1, y1 },  // top-left
		{ x2, y1 },  // top-right
		{ x1, y2 },  // bottom-left
		{ x2, y2 }   // bottom-right
	};

	for (uint8_t i = 0; i < 4; i++) {
		uint8_t cx = corners[i][0];
		uint8_t cy = corners[i][1];
		// Screw head
		myTFT.fillCircle(cx, cy, radius, myTFT.C_DGREY);
		myTFT.drawCircle(cx, cy, radius, color);
		// Cross slot
		myTFT.drawLine(cx - radius + 1, cy, cx + radius - 1, cy, color);  // horizontal
		myTFT.drawLine(cx, cy - radius + 1, cx, cy + radius - 1, color);  // vertical
	}
}
/// @endcond
