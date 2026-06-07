/*!
	@file   TEXT_GRAPHIC_FUNC.ino
	@brief  Example file gc9107 display, Library test file: Text,graphics & functions.
	@author Gavin Lyons.
	@details
					Setup for 60x160 display.
					For graphics tests after tests 905 to work
					dislib16_ADVANCED_GRAPHICS_ENABLE
					(in file display16_common_LTSM.hpp from graphics library display16_LTSM)
					must be commented in.
	@note   See USER OPTIONS in SETUP function

	@test
	-# Test 500 RGB color OK?
	-# Test 501 Scroll test
	-# Test 502 Rotate
	-# Test 503 change modes test -> Invert, display on/off and Sleep.
	-# Test 701 Print out fonts with writeCharString
	-# Test 702 Print out fonts with print
	-# Test 703 Misc print class tests(string object, println invert, base nums etc)
	-# Test 902 rectangles
	-# Test 903 Circle
	-# Test 904 Triangles
	-# Test 905 Polygons
	-# Test 906 Dot Gap
	-# Test 907 Quadrilateral
	-# Test 908 Ellipse
	-# Test 909 Draw Arc
*/

// libraries
#include "GC9107_LTSM.hpp"
// Include Fonts that are tested in this example:
#include <fonts_LTSM/FontOrla_LTSM.hpp>
#include <fonts_LTSM/FontPico_LTSM.hpp>
#include <fonts_LTSM/FontRetro_LTSM.hpp>
#include <fonts_LTSM/FontSinclairS_LTSM.hpp>
#include <fonts_LTSM/FontArialRound_LTSM.hpp>
#include <fonts_LTSM/FontDefault_LTSM.hpp>
#include <fonts_LTSM/FontGll_LTSM.hpp>
#include <fonts_LTSM/FontGroTesk_LTSM.hpp>
#include <fonts_LTSM/FontMega_LTSM.hpp>
// Included fonts but currently not tested in this example:
// #include <fonts_LTSM/FontMint_LTSM.hpp>
// #include <fonts_LTSM/FontHallfetica_LTSM.hpp>
// #include <fonts_LTSM/FontGroTeskBig_LTSM.hpp>
// #include <fonts_LTSM/FontInconsola_LTSM.hpp>
// #include <fonts_LTSM/FontSevenSeg_LTSM.hpp>
// #include <fonts_LTSM/FontSixteenSeg_LTSM.hpp>
// #include <fonts_LTSM/FontArialBold_LTSM.hpp>

/// @cond

//  Test timing related defines
#define TEST_DELAY5 5000
#define TEST_DELAY2 2000
#define TEST_DELAY 1000
// Check User options in display16_common_LTSM.hpp
#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif
#ifndef dislib16_ADVANCED_GRAPHICS_ENABLE
#pragma message("gll: dislib16_ADVANCED_GRAPHICS_ENABLE is not defined. It is needed for test 905-909")
#endif

// Init display class object
GC9107_LTSM myTFT;
bool bhardwareSPI = true; // true for Hardware SPI , false for software SPI

void setup(void)
{
	Serial.begin(38400);
	delay(1000);
	Serial.println("Start Bitmap test");
	//*** USER OPTION 0 SPI GPIO + TYPE ***
	int8_t DC_TFT = 5;
	int8_t RST_TFT = 4;
	int8_t CS_TFT = 15;
	if (bhardwareSPI == true)
	{									  // hw spi
		uint32_t TFT_SCLK_FREQ = 8000000; // Spi freq in Hertz
		myTFT.TFTsetupGPIO_SPI(TFT_SCLK_FREQ, RST_TFT, DC_TFT, CS_TFT);
	}
	else
	{								 // sw spi
		uint16_t SWSPICommDelay = 0; // optional SW SPI GPIO delay in uS
		int8_t SDIN_TFT = 26;
		int8_t SCLK_TFT = 27;
		myTFT.TFTsetupGPIO_SPI(SWSPICommDelay, RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
	}
	// ***
	// *** USER OPTION 1 Screen Setup ***
	// Size of display (note 60x160 display has VRAM of 128x160)
	uint16_t TFT_WIDTH  =  60; // width  size in pixels
	uint16_t TFT_HEIGHT = 160; // height size in pixels
	// GRAM memory base mapping resolution, see GC9107_LTSM::GM_memory_base_e enum
	GC9107_LTSM::GM_memory_base_e TFTmemoryBase = GC9107_LTSM::GM_memory_base_e::MEMORY_BASE_GM_128x160;
	myTFT.TFTInitScreen(TFT_WIDTH, TFT_HEIGHT, TFTmemoryBase, GC9107_LTSM::MADCTL_FLAGS_t::RGB);
	// *** USER OPTION 2 Screen offsets in portrait***
	uint8_t X_Offset = 34;
	uint8_t Y_Offset = 0;
	myTFT.TFTsetPanelOffset(X_Offset, Y_Offset);
	//***
	myTFT.TFTGC9107Initialize();
	myTFT.TFTsetRotation(myTFT.Degrees_0);
}

//  *** MAIN loop ***
void loop(void)
{
	Test500();
	Test501();
	Test502();
	Test503();

	Test701();
	Test702();
	Test703();

	Test902();
	Test903();
	Test904();
#ifdef dislib16_ADVANCED_GRAPHICS_ENABLE
	Test905();
	Test906();
	Test907();
	Test908();
	Test909();
#endif
	EndTests();
	while (1){};
}
// *** End OF MAIN **

//   Function Space

void EndTests(void)
{
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTPowerDown(); // Power down device
	Serial.println("End");
}

void Test500(void)
{
	Serial.println("Test 500: Color Test:: Red,green,blue,yellow,white, black background");
	myTFT.setFont(FontMega);
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.setTextColor(myTFT.C_GREEN, myTFT.C_BLACK);
	ColorBlock();
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void ColorBlock(void)
{
	myTFT.fillRoundRect(0, 100, 12, 60, 4, myTFT.C_RED);
	myTFT.fillRoundRect(12, 100, 12, 60, 4, myTFT.C_GREEN);
	myTFT.fillRoundRect(24, 100, 12, 60, 4, myTFT.C_BLUE);
	myTFT.fillRoundRect(36, 100, 12, 60, 4, myTFT.C_YELLOW);
	myTFT.fillRoundRect(48, 100, 12, 60, 4, myTFT.C_WHITE);
}

void Test501(void)
{
	Serial.println("Test 501: Scroll");
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.setFont(FontDefault);
	char teststr0[] = "Scroll";
	myTFT.writeCharString(0, 100, teststr0);

	const uint16_t screenH = 160;
	const uint16_t scrollArea = screenH - 0; // no fixed areas
	myTFT.TFTsetScrollArea(0, scrollArea, 0);

	for (uint16_t y = 0; y < screenH; y++)
	{
		myTFT.TFTsetScrollStart(y);
		delay(20);
	}
	myTFT.TFTsetScrollStart(0); // reset
	myTFT.TFTScrollModeLeave(); // leave scroll mode
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test502()
{
	Serial.println("Test 502: Rotate");
	myTFT.setFont(FontRetro);
	char teststr0[] = "R 0";   // normal
	char teststr1[] = "R 90";  // 90
	char teststr2[] = "R 180"; // 180
	char teststr3[] = "R 270"; // 270

	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTsetRotation(myTFT.Degrees_0);
	myTFT.writeCharString(0, 0, teststr0);
	myTFT.writeCharString(0, 140, teststr0);
	delay(TEST_DELAY2);

	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTsetRotation(myTFT.Degrees_90);
	myTFT.writeCharString(0, 0, teststr1);
	myTFT.writeCharString(0, 40, teststr1);
	delay(TEST_DELAY2);

	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTsetRotation(myTFT.Degrees_180);
	myTFT.writeCharString(0, 0, teststr2);
	myTFT.writeCharString(0, 140, teststr2);
	delay(TEST_DELAY2);

	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTsetRotation(myTFT.Degrees_270);
	myTFT.writeCharString(0, 0, teststr3);
	myTFT.writeCharString(0, 40, teststr3);
	delay(TEST_DELAY2);

	myTFT.TFTsetRotation(myTFT.Degrees_0);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test503()
{
	Serial.println("Test 503: Mode Tests");
	char teststr1[] = "Invert";
	char teststr2[] = "Display";
	char teststr3[] = "Sleep  ";
	ColorBlock();
	myTFT.writeCharString(0, 32, teststr1);
	delay(TEST_DELAY2);

	// Invert on and off
	myTFT.TFTchangeInvertMode(false);
	Serial.println("Test 503-1: Invert on");
	delay(TEST_DELAY5);
	myTFT.TFTchangeInvertMode(true);
	Serial.println("Test 503-2: Invert off ");
	delay(TEST_DELAY5);

	// Display on and off
	myTFT.writeCharString(0, 32, teststr2);
	delay(TEST_DELAY2);
	myTFT.TFTenableDisplay(false);
	Serial.println("Test 503-3: Display off");
	delay(TEST_DELAY2);
	myTFT.TFTenableDisplay(true);
	Serial.println("Test 503-4: Turn Display back on");
	delay(TEST_DELAY2);
	// sleep mode on and off
	myTFT.writeCharString(0, 32, teststr3);
	delay(TEST_DELAY2);
	// 1. sleep mode
	myTFT.TFTsetPowerMode(GC9107_LTSM::PowerState_e::SleepNormalIdleOff);
	Serial.println("Test 503-7: Power mode Sleep ON Idle Off");
	delay(TEST_DELAY5);
	// back to sleep off - normal on - idle off
	myTFT.TFTsetPowerMode(GC9107_LTSM::PowerState_e::NormalIdleOff);
	delay(TEST_DELAY2);
}

void Test701(void)
{

	Serial.println("Test 701: Print out some fonts with writeCharString");
	char teststr1[] = "Default";
	char teststr2[] = "GLL";
	char teststr3[] = "Pico";
	char teststr6[] = "Retro";
	myTFT.fillScreen(myTFT.C_BLACK);
	// Start at ~20px Y offset and use ~20px spacing
	int startY = 0;
	int spacing = 20;
	int x = 0;
	int y = startY;
	myTFT.setFont(FontDefault);
	myTFT.writeCharString(x, y, teststr1);
	y += spacing;
	myTFT.setFont(FontGll);
	myTFT.writeCharString(x, y, teststr2);
	y += spacing;
	myTFT.setFont(FontPico);
	myTFT.writeCharString(x, y, teststr3);
	y += spacing;
	myTFT.setFont(FontRetro);
	myTFT.writeCharString(x, y, teststr6);
	DisplayReset();
}

void Test702(void)
{
	Serial.println("Test 702: Print class methods");
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.setTextColor(myTFT.C_WHITE, myTFT.C_BLACK);
	int x = 0;
	int y = 0;
	int spacing = 25;
	// --- First Block ---
	myTFT.setCursor(x, y);
	myTFT.setFont(FontDefault);
	myTFT.print("Default ");
	myTFT.print(-43);
	y += spacing;
	myTFT.setCursor(x, y);
	myTFT.setFont(FontGll);
	myTFT.print("GLL ");
	myTFT.print(123.284, 1); // print 123.3
	y += spacing;
	myTFT.setCursor(x, y);
	myTFT.setFont(FontPico);
	myTFT.print("pico");
	myTFT.print(747);
	y += spacing;
	myTFT.setCursor(x, y);
	myTFT.setFont(FontRetro);
	myTFT.print("-3.14");
	DisplayReset();
	// --- Second Block ---
	y = 0;
	myTFT.setCursor(x, y);
	myTFT.setFont(FontMega);
	myTFT.print("meg");
	DisplayReset();
	// --- Third Block ---
	y = 0;
	myTFT.setFont(FontOrla);
	myTFT.setCursor(x, y);
	myTFT.print("OA");
	y += spacing;
	myTFT.setCursor(x, y + spacing);
	myTFT.setFont(FontArialRound);
	myTFT.print(-8);
	DisplayReset();
	// --- Final Block ---
	y = 0;
	myTFT.setCursor(x, y);
	myTFT.setFont(FontGroTesk);
	myTFT.println("GRO");
	DisplayReset();
}

void Test703(void)
{
	Serial.println("Test 703: Misc");
	// --- Inverted print fonts 1–6 ---
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.setTextColor(myTFT.C_RED, myTFT.C_YELLOW);
	myTFT.setFont(FontSinclairS);
	myTFT.setCursor(0, 20);
	myTFT.print(-49);
	DisplayReset();
	// --- Inverted print ---
	myTFT.setTextColor(myTFT.C_YELLOW, myTFT.C_RED);
	myTFT.setFont(FontDefault);
	myTFT.setCursor(0, 40);
	myTFT.print("INV");
	DisplayReset();
}

void Test902(void)
{
	Serial.println("Test 902: rectangles");
	DisLib16::Ret_Codes_e returnValue;
	myTFT.drawRectWH(5, 5, 20, 20, myTFT.C_RED);
	returnValue = myTFT.fillRectBuffer(5, 30, 20, 20, myTFT.C_YELLOW);
	if (returnValue != DisLib16::Success)
	{
		Serial.print("Warning: Test TFTfillRectangle, An error occurred returnValue = ");
		Serial.println(returnValue);
	}
	myTFT.fillRect(5, 55, 20, 20, myTFT.C_GREEN);
	myTFT.drawRoundRect(5, 80, 50, 50, 8, myTFT.C_CYAN);
	myTFT.fillRoundRect(5, 135, 40, 20, 6, myTFT.C_WHITE);

	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test903(void)
{
	Serial.println("Test 903: circles");
	myTFT.drawCircle(29, 55, 15, myTFT.C_GREEN);	  // top circle
	myTFT.fillCircle(29, 105, 15, myTFT.C_YELLOW); // bottom circle
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test904(void)
{
	Serial.println("Test 904: Triangles");
	// Triangle 1: pointing down, upper half
	myTFT.drawTriangle(
		 5, 20,  // top-left
		 55, 20, // top-right
		 29, 70, // bottom-centre
		myTFT.C_CYAN);

	// Triangle 2: pointing up, lower half
	myTFT.fillTriangle(
		 5, 145,	// bottom-left
		 55, 145, // bottom-right
		 29, 90,	// top-centre
		myTFT.C_RED);

	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

#ifdef dislib16_ADVANCED_GRAPHICS_ENABLE
void Test905(void)
{
	Serial.println("Test 905: polygons");
	myTFT.fillScreen(myTFT.C_BLACK);
	// drawPolygon( x,  y, sides,  diameter,  rotation, color);
	//  octagon
	myTFT.drawPolygon(30, 80, 8, 20, 0, false, myTFT.C_GREEN);
	delay(TEST_DELAY5);
	// octagon
	myTFT.drawPolygon(30, 80, 8, 20, 0, true, myTFT.C_GREEN);
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test906(void)
{
	Serial.println("Test 906: mesh");
	myTFT.fillScreen(myTFT.C_BLACK);
	// drawDotGrid(x, y, w, h, gap, color)
	myTFT.drawDotGrid(1, 5,  59, 155, 5, myTFT.C_CYAN);
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test907(void)
{
	Serial.println("Test 907: quadrilateral");
	// Shape 1: Rectangle (top)
	int16_t x0 =  5, y0 = 5;
	int16_t x1 =  50, y1 = 5;
	int16_t x2 =  50, y2 = 40;
	int16_t x3 =  5, y3 = 40;
	// Shape 2: Parallelogram (upper-middle)
	int16_t x4 =  20, y4 = 50;
	int16_t x5 =  55, y5 = 50;
	int16_t x6 =  40, y6 = 80;
	int16_t x7 =  5, y7 = 80;
	// Shape 3: Trapezoid wide-top (lower-middle)
	int16_t x8 =  5, y8 = 95;
	int16_t x9 =  55, y9 = 95;
	int16_t x10 =  45, y10 = 125;
	int16_t x11 =  15, y11 = 125;
	// Shape 4: Irregular quad (bottom)
	int16_t x12 =  5, y12 = 135;
	int16_t x13 =  50, y13 = 140;
	int16_t x14 =  45, y14 = 158;
	int16_t x15 =  10, y15 = 155;
	myTFT.drawQuadrilateral(x0, y0, x1, y1, x2, y2, x3, y3, myTFT.C_RED);
	myTFT.fillQuadrilateral(x4, y4, x5, y5, x6, y6, x7, y7, myTFT.C_GREEN);
	myTFT.fillQuadrilateral(x8, y8, x9, y9, x10, y10, x11, y11, myTFT.C_BLUE);
	myTFT.drawQuadrilateral(x12, y12, x13, y13, x14, y14, x15, y15, myTFT.C_YELLOW);
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test908(void)
{
	Serial.println("Test 908: Ellipse");
	myTFT.fillScreen(myTFT.C_BLACK);
	// === Unfilled pass ===
	myTFT.drawEllipse( 15, 55, 12, 25, false, myTFT.C_WHITE);	  // top-left
	myTFT.drawEllipse( 45, 55, 12, 25, false, myTFT.C_RED);	  // top-right
	myTFT.drawEllipse( 15, 120, 14, 12, false, myTFT.C_BLUE);	  // bottom-left
	myTFT.drawEllipse( 45, 120, 14, 12, false, myTFT.C_YELLOW); // bottom-right
	delay(TEST_DELAY5);
	// === Filled pass - same positions ===
	myTFT.drawEllipse( 15, 55, 12, 25, true, myTFT.C_WHITE);
	myTFT.drawEllipse( 45, 55, 12, 25, true, myTFT.C_RED);
	myTFT.drawEllipse( 15, 120, 14, 12, true, myTFT.C_BLUE);
	myTFT.drawEllipse( 45, 120, 14, 12, true, myTFT.C_YELLOW);
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

void Test909(void)
{
	Serial.println("Test 909: Drawing Arc: drawArc");
	myTFT.fillScreen(myTFT.C_BLACK);
	int16_t centerX =  40; // X-coordinate of the circle center
	int16_t centerY = 80;			 // Y-coordinate of the circle center
	int16_t radius = 20;			 // Radius of the circle
	Serial.print("Angle offset default: ");
	Serial.println(myTFT.getArcAngleOffset());
	// drawArc(cx,  cy,  radius, thickness, startAngle, endAngle, color);
	//  _arcAngleOffset is zero default means 0 is positive Axis , Arc drawn counterclockwise
	//  XXX 270 XXX
	//  180 XXX 000
	//  XXX 090 XXX
	//  Draw the full lower half (0° to 180°)
	myTFT.drawArc(centerX, centerY, radius, 4, 0.0f, 180.0f, myTFT.C_GREEN);
	delay(TEST_DELAY2);
	myTFT.fillScreen(myTFT.C_BLACK);
	// Draw the full upper half (180° to 360°)
	myTFT.drawArc(centerX, centerY, radius, 6, 180.0f, 360.0f, myTFT.C_BLUE);
	delay(TEST_DELAY2);
	myTFT.fillScreen(myTFT.C_BLACK);
	// Draw the first quarter (0° to 90°)
	myTFT.drawArc(centerX, centerY, radius, 2, 0.0f, 90.0f, myTFT.C_RED);
	delay(TEST_DELAY);
	myTFT.fillScreen(myTFT.C_BLACK);
	// Draw the second quarter (90° to 180°)
	myTFT.drawArc(centerX, centerY, radius, 2, 90.0f, 180.0f, myTFT.C_YELLOW);
	delay(TEST_DELAY);
	myTFT.fillScreen(myTFT.C_BLACK);
	// Draw the third quarter (180° to 270°)
	myTFT.drawArc(centerX, centerY, radius, 2, 180.0f, 270.0f, myTFT.C_CYAN);
	delay(TEST_DELAY);
	myTFT.fillScreen(myTFT.C_BLACK);
	// Draw the fourth quarter (270° to 360°)
	myTFT.drawArc(centerX, centerY, radius, 2, 270.0f, 360.0f, myTFT.C_MAGENTA);
	delay(TEST_DELAY);
	myTFT.fillScreen(myTFT.C_BLACK);
}

#endif

void DisplayReset(void)
{
	delay(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/// @endcond
// === EOF ===