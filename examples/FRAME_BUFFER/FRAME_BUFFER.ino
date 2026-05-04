/*!
	@file    FRAME_BUFFER.ino
	@author  Gavin Lyons
	@brief   Example  file for gc9107 library. Tests frame buffer mode
	@details Setup for 60x160 display. This example is for testing the frame buffer
			 mode of the library,
			 see readme for more details on this mode and how to use it.
	@note    See USER OPTIONS in SETUP function
			 dislib16_ADVANCED_SCREEN_BUFFER_ENABLE must be enabled for this example
			 or it will not compile. It's in display16_common_LTSM.hpp
	@test
		-# 1001 Frame buffer mode.
*/

// libraries
#include "GC9107_LTSM.hpp"
// fonts used
#include <fonts_LTSM/FontArialBold_LTSM.hpp>
/// @cond

#ifndef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is not defined. it is required for this example")
#endif
// Init display class object
GC9107_LTSM myTFT;
bool bhardwareSPI = true; // true for Hardware SPI , false for software SPI

void setup(void)
{
	Serial.begin(38400);
	delay(1000);
	Serial.println("Start Buffer test");
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
	uint16_t TFT_WIDTH =   60; // display width  size in pixels
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
	// set up buffer
	if (myTFT.setBuffer() != DisLib16::Success)
	{
		Serial.println("buffer not set");
		while (1){};
	}
	// set to use pixel mode for text
	myTFT.setTextCharPixelOrBuffer(true);
	Serial.println("Buffer Set");
}

//  MAIN loop
void loop(void)
{
	myTFT.TFTsetRotation(myTFT.Degrees_90);
	Test1001();
	EndTests();
	while (1)
	{
	};
}
// *** End OF MAIN **

//  Function Space
void EndTests(void)
{
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.clearBuffer(myTFT.C_BLACK);
	myTFT.destroyBuffer();
	myTFT.TFTPowerDown();
	printf("TFT: Tests Over\n");
}

void ScreenReset(void)
{
	myTFT.writeBuffer();
	MILLISEC_DELAY(2500);
	myTFT.clearBuffer(myTFT.C_BLACK);
}

void Test1001(void)
{
	printf("Test1\r\n");
	myTFT.clearBuffer(myTFT.C_RED);
	myTFT.writeBuffer();
	MILLISEC_DELAY(1000);
	myTFT.clearBuffer(myTFT.C_GREEN);
	myTFT.writeBuffer();
	MILLISEC_DELAY(1000);
	myTFT.clearBuffer(myTFT.C_BLUE);
	myTFT.writeBuffer();
	MILLISEC_DELAY(1000);

	myTFT.clearBuffer(myTFT.C_BLACK);
	myTFT.setTextColor(myTFT.C_GREEN, myTFT.C_BLACK);
	myTFT.setCursor(15, 15);
	myTFT.setFont(FontArialBold);
	myTFT.print("Buffer");
	myTFT.writeBuffer();
	MILLISEC_DELAY(5000);
}

/// @endcond
// === EOF ===