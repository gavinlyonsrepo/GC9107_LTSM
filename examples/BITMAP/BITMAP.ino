/*!
	@file    BITMAP.ino
	@author  Gavin Lyons
	@brief   Example file for gc9107, bitmap tests
	@details Setup for 60x160 display.
			 See USER OPTIONS in setup function
	@test
		-# Test 300 Sprite
		-# Test 301 icons
		-# Test 302 bi-color small image
		-# Test 305 16 bit color image data from a data array
*/

// libraries
#include "GC9107_LTSM.hpp"
// Fonts needed
#include "fonts_LTSM/FontDefault_LTSM.hpp"
// Bitmap data needed
#include "bitmap_test_data_LTSM/Bitmap_TEST_Data_16color2.hpp"
#include "bitmap_test_data_LTSM/Bitmap_TEST_Data_bicolor.hpp"

/// @cond

// Section :: Defines
//  Test timing related defines
#define TEST_DELAY1 1000
#define TEST_DELAY2 2000
#define TEST_DELAY5 5000
#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif

// Screen Values Setup
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
	uint16_t TFT_WIDTH =   60;	// display width  size in pixels
	uint16_t TFT_HEIGHT = 160; // display height size in pixels
	// GRAM memory base mapping resolution, see GC9107_LTSM::GM_memory_base_e enum
	GC9107_LTSM::GM_memory_base_e TFTmemoryBase = GC9107_LTSM::GM_memory_base_e::MEMORY_BASE_GM_128x160;
	myTFT.TFTInitScreen(TFT_WIDTH, TFT_HEIGHT, TFTmemoryBase, GC9107_LTSM::MADCTL_FLAGS_t::RGB);
	// ***
	// *** USER OPTION 2 Screen offsets in portrait***
	uint8_t X_Offset = 34;
	uint8_t Y_Offset = 0;
	myTFT.TFTsetPanelOffset(X_Offset, Y_Offset);
	//***
	myTFT.TFTGC9107Initialize();
}

//  MAIN loop
void loop(void)
{
	// rotate the screen 90 degrees
	myTFT.TFTsetRotation(myTFT.Degrees_90);
	Test300();
	Test302();
	Test305();
	EndTests();
	while (1){};
}
// *** End OF MAIN **

// Function Space
/*!
	@brief  "sprite demo" 32 x 32 with LBLUE background
*/
void Test300(void)
{
	// Test 300-A test 16-bit color Sprite
	// Draw as sprite, without background , 32 X 32 .background color = ST7375_LBLUE
	// Green background screen
	myTFT.fillScreen(myTFT.C_GREEN);
	MILLISEC_DELAY(TEST_DELAY2);
	myTFT.drawSpriteData(0, 0, SpriteTest16, 32, 32, myTFT.C_LBLUE, false);
	MILLISEC_DELAY(TEST_DELAY5);

	// Test 300-B test 16-bit color Sprite
	// Draw as sprite, without background , 32 X 32 .background color = ST7375_LBLUE
	// Bitmap background screen
	myTFT.drawBitmap16Data(60, 0, PosterImage, 80, 48);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.drawSpriteData(60, 0, SpriteTest16, 32, 32, myTFT.C_LBLUE, false);
	myTFT.drawSpriteData(90, 0, SpriteTest16, 32, 32, myTFT.C_LBLUE, false);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test 302 bi-color small image 20x24
*/
void Test302(void)
{

	myTFT.fillScreen(myTFT.C_BLACK);
	char teststr1[] = "Test 302";
	myTFT.writeCharString(5, 10, teststr1);
	myTFT.drawBitmap(0, 30, 40, 16, myTFT.C_CYAN, myTFT.C_BLACK, SunTextImage);
	myTFT.drawBitmap(50, 30, 40, 16, myTFT.C_RED, myTFT.C_BLACK, SunTextImage);
	myTFT.drawBitmap(110, 30, 40, 16, myTFT.C_YELLOW, myTFT.C_RED, SunTextImage);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test305 16 bit color image data from a data array
*/
void Test305(void)
{
	char teststr1[] = "Test 305";
	myTFT.fillScreen(myTFT.C_WHITE);
	myTFT.writeCharString(0, 0, teststr1);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.drawBitmap16Data(0, 0, PosterImage, 80, 48);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Stop testing and shutdown the TFT
*/
void EndTests(void)
{
	char teststr1[] = "Tests over";
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.writeCharString(0, 5, teststr1);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.TFTPowerDown();
	Serial.println("Tests Over");
}

/// @endcond
// === EOF ===