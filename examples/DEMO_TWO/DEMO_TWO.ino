/*!
	@file   DEMO_TWO.ino
	@brief  Demo: EMV Monitor Bar
	@author Gavin Lyons.
	@details
			Demonstrates the GC9107 60x160 Landscape display as a EMV monitor 
			status bar using fake/simulated data. Hardware SPI
	@test
			-# 402 EMV Monitor Demo
*/

// Section :: libraries
#include "GC9107_LTSM.hpp"
// Fonts needed
#include <fonts_LTSM/FontRetro_LTSM.hpp>


/// @cond

#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif

// Section :: Globals
GC9107_LTSM myTFT;
bool bhardwareSPI = true;  // true for Hardware SPI , false for software SPI

// Section :: Demo Config
#define DEMO_CYCLES       200    // how many main-loop ticks to run
#define TICK_MS           200    // ms per tick

// Section :: Layout constants (landscape 160 x 60)
// Search area 
#define SEARCH_X          26     // left edge of search / signal area
#define SEARCH_W          106    // total width of signal area
#define SEARCH_TOP_Y      10     // EGR bar top y
#define SEARCH_BOT_Y      35     // EAR bar top y
#define BAR_H             14     // height of each signal bar row

// EMV strength zone
#define EMV_RIGHT_X       131    // right edge of EMV zone (SEARCH_X + SEARCH_W - 1)
#define EMV_MIN_W         4      // minimum EMV zone width in pixels
#define EMV_MAX_W         20     // maximum EMV zone width in pixels

// EMV warning light box
#define WARN_X            135
#define WARN_Y            34
#define WARN_W            23
#define WARN_H            12

// Signal bar limits
#define BAR_MIN_W         2
#define BAR_MAX_W         100    // max signal bar width (must fit inside SEARCH_W)

// Timing thresholds (in ticks, 1 tick = TICK_MS ms)
#define EMV_CHANGE_TICKS   5     // grow EMV zone every 1 s

// Fake EGR signal pattern (strength 0-100, 0 = no signal)
static const uint8_t kEGRPattern[] = {
	0, 0, 0, 72, 72, 72, 72, 72, 72, 72, 72, 0,
	0, 0, 0, 0, 45, 45, 45, 45, 45, 45, 0,
	0, 0, 0, 0, 0, 88, 88, 88, 88, 88, 88, 88, 88, 0,
	0, 0, 0, 30, 30, 30, 30, 30, 0,
	0, 0, 0, 0, 0, 55, 55, 55, 55, 0
};
#define EGR_PATTERN_LEN (sizeof(kEGRPattern))

// Fake EAR signal pattern (strength 0-100, 0 = no signal)
static const uint8_t kEARPattern[] = {
	0, 0, 0, 0, 0, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 0,
	0, 0, 0, 0, 0, 0, 90, 90, 90, 90, 90, 90, 0,
	0, 0, 0, 0, 0, 0, 38, 38, 38, 38, 38, 38, 38, 0,
	0, 0, 0, 0, 0, 0, 0, 75, 75, 75, 75, 75, 75, 75, 75, 0
};
#define EAR_PATTERN_LEN (sizeof(kEARPattern))


void setup(void) {
	Serial.begin(38400);
	delay(1000);
	Serial.println("Start Demo 2");
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
	myTFT.fillScreen(myTFT.C_BLACK);
}


//  Section ::  MAIN loop
void loop() {
	Demo();
	EndDemo();
	while (true) {
		delay(1000);
	}
}
// *** End OF MAIN **

void Demo(void) {
	Serial.println("Demo 402: EMV Monitor");

	// Draw labels, borders once
	DrawStatic();

	// State for EMV zone
	uint8_t emvW         = EMV_MIN_W;
	uint8_t emvTickCount = 0;
	// State for EGR bar
	uint8_t egrPatIdx    = 0;
	uint8_t prevEGRW     = 0;
	// State for EAR bar
	uint8_t earPatIdx    = 0;
	uint8_t prevEARW     = 0;

	for (uint8_t step = 0; step < DEMO_CYCLES; step++)
	{
		// --- EMV zone update ---
		emvTickCount++;
		if (emvTickCount >= EMV_CHANGE_TICKS)
		{
			emvTickCount = 0;
			// Grow by 1-3 pixels, wrap back to min when max reached
			uint8_t grow = (uint8_t)(1 + (step % 3));
			emvW += grow;
			if (emvW > EMV_MAX_W) emvW = EMV_MIN_W;
			UpdateEMVZone(emvW);
		}

		// --- Signal bars ---
		uint8_t egrStrength = kEGRPattern[egrPatIdx % EGR_PATTERN_LEN];
		uint8_t earStrength = kEARPattern[earPatIdx % EAR_PATTERN_LEN];
		egrPatIdx++;
		earPatIdx++;
		uint8_t egrW = (egrStrength > 0)
			? (uint8_t)(BAR_MIN_W + ((uint16_t)egrStrength * (BAR_MAX_W - BAR_MIN_W)) / 100)
			: 0;
		uint8_t earW = (earStrength > 0)
			? (uint8_t)(BAR_MIN_W + ((uint16_t)earStrength * (BAR_MAX_W - BAR_MIN_W)) / 100)
			: 0;

		// EMV zone 
		uint8_t emvLeftX = EMV_RIGHT_X - emvW + 1;
		
		bool egrHit = (egrW > 0) && ((SEARCH_X + egrW) >= emvLeftX);
		bool earHit = (earW > 0) && ((SEARCH_X + earW) >= emvLeftX);
		// On a hit, redraw EMV zone once per tick-gate to repair overwritten pixels
		if ((egrHit || earHit) && (emvTickCount == 0)) UpdateEMVZone(emvW);
		UpdateEGRBar(egrW, prevEGRW, egrHit);
		UpdateEARBar(earW, prevEARW, earHit);
		DrawWarnBox(egrHit, earHit, emvW);
		prevEGRW = egrW;
		prevEARW = earW;

		delay(TICK_MS);
	}
}

void EndDemo(void) {
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTPowerDown();
	Serial.println("Demo End");
}

// === Static objects ===
void DrawStatic(void)
{
  DrawTextLabels();
	DrawBorder();
	DrawWarnBox(false, false, 0);
	DrawSearchArea();
}

void DrawTextLabels(void) {
	myTFT.setFont(FontRetro);
	myTFT.setTextColor(myTFT.C_GREEN, myTFT.C_BLACK);
	myTFT.setCursor(2, 10);
	myTFT.print("EGR");
	myTFT.setCursor(2, 34);
	myTFT.print("EAR");
	myTFT.setCursor(135, 16);
	myTFT.print("EMV");
}

void DrawBorder(void) {
	myTFT.drawRectWH(0, 0, 160, 60, myTFT.C_DGREY);
}

/*!
	@brief  Fill the search area background (called once on setup).
	        Draws two separate row backgrounds + gap so bar clears are clean.
*/
void DrawSearchArea(void) {
	// EGR + EAR + GAP row background
	myTFT.fillRectBuffer(SEARCH_X, SEARCH_TOP_Y, SEARCH_W, (BAR_H*2)+11, myTFT.C_BLUE);
}

/*!
	@brief  Draw or flash the EMV warning light box.
	@param  egrHit  EGR bar has entered the EMV zone
	@param  earHit  EAR bar has entered the EMV zone
	@param  emvW current width of EMV area
*/
void DrawWarnBox(bool egrHit, bool earHit, uint8_t emvW) {
	uint16_t color;
	if (egrHit || earHit){
		color = myTFT.C_RED;
}else{
		color = myTFT.C_MAROON;
	}
	myTFT.fillRectBuffer(WARN_X, WARN_Y, WARN_W, WARN_H, color);

}

// === Dynamic updaters ===

/*!
	@brief  Redraw the EMV strength zone (maroon band growing right-to-left).
	        Clears the old zone first if it shrinks on wrap.
	@param  w  Current zone width in pixels
*/
void UpdateEMVZone(uint8_t w) {
	// Clear the entire possible EMV zone area first
	myTFT.fillRectBuffer(EMV_RIGHT_X - EMV_MAX_W + 1, SEARCH_TOP_Y,
	               EMV_MAX_W, BAR_H * 2 + (SEARCH_BOT_Y - SEARCH_TOP_Y - BAR_H), myTFT.C_BLUE);
	// Draw the new zone spanning both bar rows and the gap between them
	uint8_t zoneH = (SEARCH_BOT_Y + BAR_H) - SEARCH_TOP_Y;
	myTFT.fillRectBuffer(EMV_RIGHT_X - w + 1, SEARCH_TOP_Y, w, zoneH, myTFT.C_MAROON);
}

/*!
	@brief  Update EGR signal bar.
	        Clears retreating pixels, draws new fill in correct colour.
	@param  newW    New bar width (0 = no signal)
	@param  prevW   Previous bar width
	@param  hit     True if bar overlaps EMV zone
*/
void UpdateEGRBar(uint8_t newW, uint8_t prevW, bool hit) {
	// Clear bar row over the search area (preserves EMV zone drawn underneath)
	if (prevW > newW && prevW > 0)
	{
		uint8_t clearStart = SEARCH_X + newW;
		uint8_t clearW     = prevW - newW;
		myTFT.fillRectBuffer(clearStart, SEARCH_TOP_Y, clearW, BAR_H, myTFT.C_BLUE);
	}
	if (newW == 0) return;

	uint16_t color;
	if (hit)
		color = myTFT.C_YELLOW;
	else if (/* strength */ newW > (BAR_MAX_W / 2))
		color = myTFT.C_RED;
	else
		color = myTFT.C_MAROON;

	myTFT.fillRectBuffer(SEARCH_X, SEARCH_TOP_Y, newW, BAR_H, color);
}

/*!
	@brief  Update EAR signal bar.
	        Clears retreating pixels, draws new fill in correct colour.
	@param  newW    New bar width (0 = no signal)
	@param  prevW   Previous bar width
	@param  hit     True if bar overlaps EMV zone
*/
void UpdateEARBar(uint8_t newW, uint8_t prevW, bool hit) {
	if (prevW > newW && prevW > 0)
	{
		uint8_t clearStart = SEARCH_X + newW;
		uint8_t clearW     = prevW - newW;
		myTFT.fillRectBuffer(clearStart, SEARCH_BOT_Y, clearW, BAR_H, myTFT.C_BLUE);
	}
	if (newW == 0) return;

	uint16_t color;
	if (hit)
		color = myTFT.C_WHITE;
	else if (newW > (BAR_MAX_W / 2))
		color = myTFT.C_CYAN;
	else
		color = myTFT.C_DCYAN;

	myTFT.fillRectBuffer(SEARCH_X, SEARCH_BOT_Y, newW, BAR_H, color);
}

/// @endcond
