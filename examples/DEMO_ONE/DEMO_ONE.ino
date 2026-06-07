/*!
	@file   DEMO_ONE.ino
	@brief  Demo: Fake System Monitor Status Bar
	@author Gavin Lyons.
	@details
			Demonstrates the GC9107 60x160 portrait display as a system monitor
			status bar using fake/simulated data. Showcases the narrow portrait
			form factor with a vertical stack of labelled metric panels.
			Hardware SPI.
	@test
			-# 401 System Monitor Demo
*/

// Section :: libraries
#include "GC9107_LTSM.hpp"
// Fonts needed
#include <fonts_LTSM/FontPico_LTSM.hpp>


/// @cond

#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif

// Section :: Globals
GC9107_LTSM myTFT;
bool bhardwareSPI = true;  // true for Hardware SPI , false for software SPI

// Section :: Demo Config
#define DEMO_CYCLES 20       // how many refresh cycles to run
#define CYCLE_DELAY_MS 2000  // ms between each refresh

// Section :: Layout constants (portrait 60 x 160)
// Display is divided into 5 equal panels, each 32px tall with a 1px divider.
// Total = 5 * 32 = 160px
#define PANEL_H 32       // height of each metric panel in pixels
#define PANEL_W 60       // full display width
#define BAR_X 1          // progress bar left margin
#define BAR_Y_OFFSET 18  // y offset of bar within panel
#define BAR_H 8          // progress bar height
#define BAR_MAX_W 52     // max usable bar width (leaving 7px for border/margin)
#define LABEL_X 1        // x for panel label text
#define VALUE_X 38       // x for value text (right-aligned area)

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
	myTFT.TFTsetRotation(myTFT.Degrees_0);
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

// Tracks previous bar fill per panel (4 panels), used to detect decreases
static uint8_t prevPercent[4] = { 0, 0, 0, 0 };

//  DEMO  – cycles through DEMO_CYCLES refreshes of fake data
void Demo(void) {
	Serial.println("Demo 401: System Monitor");
	myTFT.fillScreen(myTFT.C_BLACK);

	// Reset per-panel state for a fresh run
	for (uint8_t i = 0; i < 4; i++) prevPercent[i] = 0;

	// Draw static chrome (header, labels, bar outlines, dividers) once
	DrawStaticLayout();

	for (uint8_t step = 0; step < DEMO_CYCLES; step++) {
		// --- Gather fake metrics ---
		uint8_t cpu = FakeCPU(step);
		uint8_t ram = FakeRAM(step);
		uint8_t temp = FakeTemp(step);  // 0-100 scaled from 30-90°C
		uint8_t disk = FakeDisk(step);
		// Build display strings
		char cpuStr[8], ramStr[8], tempStr[8], diskStr[8];
		snprintf(cpuStr, sizeof(cpuStr), "%2u", cpu);
		snprintf(ramStr, sizeof(ramStr), "%2u", ram);
		// temp: 0-100 maps to 30-90°C
		snprintf(tempStr, sizeof(tempStr), "%2u", (uint8_t)(30 + (temp * 60) / 100));
		snprintf(diskStr, sizeof(diskStr), "%2u", disk);
		// --- Render dynamic content only ---
		// Panel 0 – CPU
		UpdatePanel(0, cpu, PanelColor(cpu), cpuStr);
		// Panel 1 – RAM
		UpdatePanel(1, ram, PanelColor(ram), ramStr);
		// Panel 2 – TEMP
		UpdatePanel(2, temp, PanelColor(temp), tempStr);
		// Panel 3 – DISK
		UpdatePanel(3, disk, PanelColor(disk), diskStr);

		delay(CYCLE_DELAY_MS);
	}
}

void EndDemo(void) {
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.TFTPowerDown();
	Serial.println("Demo End");
}

// === Drawing helpers ===

// Helper: map 0-100 percent to generateColor's 1-127 range
uint16_t PanelColor(uint8_t percent) {
	uint8_t val = (uint8_t)(1 + ((uint16_t)percent * 126) / 100);
	return generateColor(val);
}

/*!
	@brief  Draw all static chrome once: header, panel labels, bar outlines, dividers.
	        Call once before entering the update loop.
*/
void DrawStaticLayout(void) {
	// --- Header ---
	myTFT.drawRectWH(0, 0, PANEL_W, 1, myTFT.C_WHITE);
	myTFT.setFont(FontPico);
	myTFT.setTextColor(myTFT.C_CYAN, myTFT.C_BLACK);
	myTFT.setCursor(2, 3);
	myTFT.print("SYS MON");
	myTFT.drawRectWH(0, 14, PANEL_W, 1, myTFT.C_WHITE);

	// --- Per-panel static elements ---
	static const char* labels[] = { "CPU", "RAM", "TEMP", "DISK" };
	for (uint8_t i = 0; i < 4; i++) {
		int16_t py = 16 + (int16_t)i * PANEL_H;
		// Bottom divider
		myTFT.drawRectWH(0, py + PANEL_H - 1, PANEL_W, 1, myTFT.C_WHITE);
		// Label
		myTFT.setFont(FontPico);
		myTFT.setTextColor(myTFT.C_WHITE, myTFT.C_BLACK);
		myTFT.setCursor(LABEL_X, py + 3);
		myTFT.print(labels[i]);
		// Bar outline (empty bar, no fill yet)
		myTFT.drawRectWH(BAR_X, py + BAR_Y_OFFSET, BAR_MAX_W + 2, BAR_H, myTFT.C_WHITE);
	}
}

/*!
	@brief  Update only the dynamic parts of a panel (value text + bar fill).
	        Clears the bar interior or value area first when the value decreases.
	@param  panelIndex  0-3, controls vertical position (below 16px header)
	@param  percent     0-100 fill level for progress bar
	@param  barColor    Colour of the progress bar and value text
	@param  valueStr    Formatted value string e.g. "72"
*/
void UpdatePanel(uint8_t panelIndex, uint8_t percent, uint16_t barColor, const char* valueStr) {
	int16_t py = 16 + (int16_t)panelIndex * PANEL_H;

	// --- Value text ---
	// Always clear the value area before redrawing to avoid digit ghosting
	myTFT.fillRect(VALUE_X, py + 3, PANEL_W - VALUE_X, 6, myTFT.C_BLACK);
	myTFT.setFont(FontPico);
	myTFT.setTextColor(barColor, myTFT.C_BLACK);
	myTFT.setCursor(VALUE_X, py + 3);
	myTFT.print(valueStr);

	// --- Progress bar fill ---
	int16_t barY = py + BAR_Y_OFFSET;
	uint8_t newFillW = (percent > 0)
	                     ? (uint8_t)((uint16_t)percent * BAR_MAX_W / 100)
	                     : 0;
	if (newFillW > BAR_MAX_W) newFillW = BAR_MAX_W;

	uint8_t oldFillW = (prevPercent[panelIndex] > 0)
	                     ? (uint8_t)((uint16_t)prevPercent[panelIndex] * BAR_MAX_W / 100)
	                     : 0;
	if (oldFillW > BAR_MAX_W) oldFillW = BAR_MAX_W;

	if (newFillW < oldFillW) {
		// Clear the portion of the bar that is no longer filled
		myTFT.fillRect(BAR_X + 1 + newFillW, barY + 1,
		               oldFillW - newFillW, BAR_H - 2, myTFT.C_BLACK);
	}

	if (newFillW > 0) {
		myTFT.fillRect(BAR_X + 1, barY + 1, newFillW, BAR_H - 2, barColor);
	}

	prevPercent[panelIndex] = percent;
}

//  === Fake data generators ===
uint8_t FakeCPU(uint8_t step) {
	static const uint8_t pattern[] = { 22, 35, 48, 71, 89, 99, 76, 49, 30, 25 };
	return pattern[step % 10];
}

uint8_t FakeRAM(uint8_t step) {
	static const uint8_t pattern[] = { 45, 47, 50, 53, 56, 58, 60, 61, 62, 63 };
	return pattern[step % 10];
}

uint8_t FakeTemp(uint8_t step) {
	static const uint8_t pattern[] = { 28, 35, 45, 58, 72, 82, 78, 65, 50, 40 };
	return pattern[step % 10];
}

uint8_t FakeDisk(uint8_t step) {
	static const uint8_t pattern[] = { 68, 68, 69, 69, 70, 70, 71, 71, 71, 72 };
	return pattern[step % 10];
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

/// @endcond