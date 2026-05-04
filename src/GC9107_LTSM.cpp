/*!
	@file   GC9107_LTSM.cpp
	@author Gavin Lyons
	@brief  Source file. Contains driver methods for gc9107 display 
*/

#include "GC9107_LTSM.hpp"
 
/*!
	@brief Constructor for class GC9107_LTSM
*/
GC9107_LTSM :: GC9107_LTSM(){}

/*!
	@brief : Init Hardware SPI
*/
void GC9107_LTSM::TFTHWSPIInitialize(void)
{
	SPI.begin();
}

/*!
	@brief: Call when powering down TFT
	@note  Will switch off SPI 
*/
void GC9107_LTSM::TFTPowerDown(void)
{
	TFTenableDisplay(false);
	if (_resetPinOn == true) {
		DISPLAY16_RST_SetLow;
	}
	DISPLAY16_DC_SetLow;
	DISPLAY16_CS_SetLow;

	if (_hardwareSPI == true) {
		SPI.end();
	}else{
		DISPLAY16_SCLK_SetLow;
		DISPLAY16_SDATA_SetLow;
	}
}

/*!
	@brief Method for Hardware Reset pin control
*/
void GC9107_LTSM ::TFTResetPIN() {
	if (_resetPinOn == false) return;
	DISPLAY16_RST_SetDigitalOutput;
	DISPLAY16_RST_SetHigh;
	MILLISEC_DELAY(5);
	DISPLAY16_RST_SetLow;
	MILLISEC_DELAY(15);
	DISPLAY16_RST_SetHigh;
	MILLISEC_DELAY(150);
}

/*!
	@brief sets up TFT GPIO
	@param CommDelay SW SPI GPIO delay
	@param rst reset GPIO, optional pass -1 to disable, see note
	@param dc data or command GPIO.
	@param cs chip select GPIO 
	@param sclk Data clock GPIO  
	@param din Data to TFT GPIO 
	@details if -1 is passed for reset pin, software reset is used, if LCD has optional reset pin
			Overloaded one of two this one if for software SPI
*/
void GC9107_LTSM::TFTsetupGPIO_SPI(uint16_t CommDelay, int8_t rst, int8_t dc, int8_t cs, int8_t sclk, int8_t din)
{
	TFTSwSpiGpioDelaySet(CommDelay);
	_hardwareSPI = false;
	_display_RST= rst;
	_display_DC = dc;
	_display_CS = cs;
	_display_SDATA = din;
	_display_SCLK = sclk;

	if (_display_RST == -1 ){
		_resetPinOn = false;
	}else{
		_resetPinOn = true;
	}
}

/*!
	@brief sets up TFT GPIO
	@param speed_hz SPI baudrate in hz 
	@param rst reset GPIO, optional pass -1 to disable, see note
	@param dc data or command GPIO.
	@param cs chip select GPIO 
	@param sclk Data clock GPIO  
	@param din Data to TFT GPIO 
	@details if -1 is passed for reset pin, software reset is used, if LCD has optional reset pin
			Overloaded one of two this one if for hardware  SPI
*/
void GC9107_LTSM::TFTsetupGPIO_SPI(uint32_t speed_hz, int8_t rst, int8_t dc, int8_t cs)
{
	_speedSPIHz = speed_hz;
	_hardwareSPI = true;
	_display_RST= rst;
	_display_DC = dc;
	_display_CS = cs;

	if (_display_RST == -1 ){
		_resetPinOn = false;
	}else{
		_resetPinOn = true;
	}
}

/*!
	@brief init routine for GC9107 controller
	@details Calls the appropriate init sequence based on resolution selected
*/
void GC9107_LTSM::TFTGC9107Initialize() 
{
	if (_resetPinOn == true) 
	{
		TFTResetPIN();
	}
	DISPLAY16_DC_SetDigitalOutput;
	DISPLAY16_DC_SetLow;
	DISPLAY16_CS_SetDigitalOutput;
	DISPLAY16_CS_SetHigh;
	if (_hardwareSPI == false)
	{
		DISPLAY16_SCLK_SetDigitalOutput;
		DISPLAY16_SDATA_SetDigitalOutput;
		DISPLAY16_SCLK_SetLow;
		DISPLAY16_SDATA_SetLow;
	}else{
		TFTHWSPIInitialize();
	}
	TFTInitSequence();
}

/*!
	@brief Toggle the invert mode, inverted colours
	@param invert true = invert off , false = invert on
*/
void GC9107_LTSM ::TFTchangeInvertMode(bool invert) {
	if(invert) {
		writeCommand(GC9107_INVOFF);
	} else {
		writeCommand(GC9107_INVON);
	}
}

/*!
	@brief: change rotation of display.
	@param mode display_rotate_e enum
		0 = Normal
		1=  90 rotate
		2 = 180 rotate
		3 =  270 rotate
*/
void GC9107_LTSM::TFTsetRotation(display_rotate_e mode) {
	uint8_t madctl = _colorOrder; // RBG or BGR color order bit from MADCTL_FLAGS_t
	switch (mode) {
		case Degrees_0 :
			_width =_widthStartTFT;
			_height = _heightStartTFT;
			_xstart = _colOffset;
			_ystart = _rowOffset;
			break;
		case Degrees_90:
			madctl |= (MADCTL_FLAGS_t::ML | MADCTL_FLAGS_t::MV | MADCTL_FLAGS_t::MX  );
			_width  =_heightStartTFT;
			_height = _widthStartTFT;
			_xstart = _rowOffset;
			_ystart = _colOffset;
			break;
		case Degrees_180:  
			madctl |= (MADCTL_FLAGS_t::MY | MADCTL_FLAGS_t::MX );
			_width  = _widthStartTFT;
			_height = _heightStartTFT;
			_xstart = _RAM_WIDTH  - _widthStartTFT  - _colOffset;
			_ystart = _RAM_HEIGHT - _heightStartTFT - _rowOffset;
			break;
		case Degrees_270:  
			madctl |= (MADCTL_FLAGS_t::MY |MADCTL_FLAGS_t::MV );
			_xstart = _RAM_HEIGHT - _heightStartTFT - _rowOffset;
			_ystart = _RAM_WIDTH  - _widthStartTFT  - _colOffset;
            _width  = _heightStartTFT;
            _height = _widthStartTFT;
			break;
	}
	writeCommand(GC9107_MADCTL);
	writeData(madctl);
}

/*!
	@brief initialise the variables that define the size of the screen
	@param width_TFT width in pixels
	@param height_TFT height in pixels
	@param memoryBase GRAM memory base mapping resolution, see GM_memory_base_e enum
	@param colorOrder MADCTL color order, RGB or BGR, see MADCTL_FLAGS_t struct
*/
void GC9107_LTSM::TFTInitScreen(uint16_t width_TFT, uint16_t height_TFT,
	GM_memory_base_e memoryBase, uint8_t colorOrder)
{
	_width          = width_TFT;
	_height         = height_TFT;
	_widthStartTFT  = width_TFT;
	_heightStartTFT = height_TFT;
	_memoryBase     = memoryBase;
	switch (_memoryBase)
	{
		case GM_memory_base_e::MEMORY_BASE_GM_128x160:
			_RAM_WIDTH  = 128;
			_RAM_HEIGHT = 160;
		break;
		case GM_memory_base_e::MEMORY_BASE_GM_128x128:
			_RAM_WIDTH  = 128;
			_RAM_HEIGHT = 128;
		break;
		default:
			_RAM_WIDTH  = 128;
			_RAM_HEIGHT = 128;
		break;
	}
	if (colorOrder != MADCTL_FLAGS_t::RGB && colorOrder != MADCTL_FLAGS_t::BGR)
	{
		#ifdef dislib16_DEBUG_MODE_ENABLE
			Serial.println("Warning: TFTInitScreen: Invalid color order, defaulting to RGB");
		#endif
		_colorOrder = MADCTL_FLAGS_t::RGB;
	}else{
		_colorOrder = colorOrder;
	}
}

/*!
	@brief Freq delay used in SW SPI getter, uS delay used in SW SPI method
	@return The  GPIO communications delay in uS
*/
uint16_t GC9107_LTSM::TFTSwSpiGpioDelayGet(void){return _SWSPIGPIODelay;}

/*!
	@brief Freq delay used in SW SPI setter, uS delay used in SW SPI method
	@param CommDelay The GPIO communications delay in uS
*/
void  GC9107_LTSM::TFTSwSpiGpioDelaySet(uint16_t CommDelay){_SWSPIGPIODelay = CommDelay;}

/*!
  @brief SPI displays set an address window rectangle for blitting pixels
  @param  x1 Top left corner x coordinate
  @param  y1  Top left corner y coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
void GC9107_LTSM::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h)
{	
	//if drawing a single pixel we need do this to avoid a blank pixel for this device
	const bool singlePixel =
		((w == x1 + 1) && (h == y1 + 1));
	if (singlePixel) {
		w = x1;
		h = y1;
	}
	// Translate panel coordinates → VRAM coordinates
	x1 += _xstart;
	w  += _xstart;
	y1 += _ystart;
	h  += _ystart;

	uint8_t x1Higher = (x1 >> 8);
	uint8_t x1Lower  = (x1 & 0xFF);
	uint8_t x2Higher = (w >> 8);
	uint8_t x2Lower  = (w &  0xFF);
	uint8_t seqCASET[4]  =  {x1Higher ,x1Lower,x2Higher,x2Lower};
	uint8_t y1Higher = (y1 >> 8); 
	uint8_t y1Lower  = (y1 &  0xFF);
	uint8_t y2Higher = (h >> 8);
	uint8_t y2Lower  = (h &  0xFF);
	uint8_t seqRASET[4]  =  {y1Higher,y1Lower,y2Higher,y2Lower};
	writeCommand(GC9107_CASET); //Column address set
	spiWriteDataBuffer(seqCASET, sizeof(seqCASET));
	writeCommand(GC9107_RASET); //Row address set
	spiWriteDataBuffer(seqRASET, sizeof(seqRASET));
	writeCommand(GC9107_RAMWR); //Write to RAM
}

/*!
	@brief This method defines the Vertical Scrolling Area of the display where:
	@param topFixed describes the Top Fixed Area.
	@param scrollArea describes the Scrolling Area.
	@param bottomFixed describes the Bottom Fixed Area.
*/
void GC9107_LTSM::TFTsetScrollArea(uint16_t topFixed, uint16_t scrollArea, uint16_t bottomFixed) {

	writeCommand(GC9107_VSCRDEF);
	writeData(topFixed >> 8);
	writeData(topFixed & 0xFF);
	writeData(scrollArea >> 8);
	writeData(scrollArea  & 0xFF);
	writeData(bottomFixed >> 8);
	writeData(bottomFixed & 0xFF);
}

/*!
	@brief This method is used together with the setScrollDefinition.
	@param vsp scrolling mode
*/
void GC9107_LTSM::TFTsetScrollStart(uint16_t vsp) {
	writeCommand(GC9107_VSCRSADD);
	writeData(vsp >> 8);
	writeData(vsp & 0xFF);
}

/*! @brief Scroll Mode can be left ,by the Normal Display Mode ON cmd*/
void GC9107_LTSM::TFTScrollModeLeave(void) {writeCommand(GC9107_NORON);}


/*!
	@brief enable /disable display mode
	@param enableDisplay true enable on false disable
	@details Temporarily blank the screen.
	Use Case: Screen blanking, brief off periods without 
	resetting or reinitializing the display.
*/
void GC9107_LTSM::TFTenableDisplay(bool enableDisplay){
	if(enableDisplay) {
		writeCommand(GC9107_DISPON);
		_displayOn = true;
	} else {
		writeCommand(GC9107_DISPOFF);
		_displayOn = false;
	}
}

/*!
	@brief Set the power mode of the display
	@param mode The power state to set
	@details 
		Power states are based on the power control flow chart in the datasheet.
		FIG 89 5.10.2. Power Flow Chart	
*/
void GC9107_LTSM::TFTsetPowerMode(PowerState_e mode) {
	// If already in the desired state or off , skip
	if (_currentPowerState == mode || _displayOn != true) 
	{
		#ifdef dislib16_DEBUG_MODE_ENABLE
			Serial.println("Warning: TFTsetPowerMode: Display already in this state or off");
		#endif
	}
	// Always return to a known base state
	writeCommand(GC9107_SLPOUT);
	MILLISEC_DELAY(_sleepDelay);
	switch (mode) {
		case PowerState_e::NormalIdleOff:
			writeCommand(GC9107_NORON);
			writeCommand(GC9107_IDLEOFF);
			break;
		case PowerState_e::NormalIdleOn:
			writeCommand(GC9107_NORON);
			writeCommand(GC9107_IDLEON);
			break;
		case PowerState_e::PartialIdleOff:
			writeCommand(GC9107_PTLON);
			writeCommand(GC9107_IDLEOFF);
			break;
		case PowerState_e::PartialIdleOn:
			writeCommand(GC9107_PTLON);
			writeCommand(GC9107_IDLEON);
			break;
		case PowerState_e::SleepNormalIdleOff:
			writeCommand(GC9107_NORON);
			writeCommand(GC9107_IDLEOFF);
			writeCommand(GC9107_SLPIN);
			MILLISEC_DELAY(_sleepDelay);
			break;
		case PowerState_e::SleepNormalIdleOn:
			writeCommand(GC9107_NORON);
			writeCommand(GC9107_IDLEON);
			writeCommand(GC9107_SLPIN);
			MILLISEC_DELAY(_sleepDelay);
			break;
		case PowerState_e::SleepPartialIdleOff:
			writeCommand(GC9107_PTLON);
			writeCommand(GC9107_IDLEOFF);
			writeCommand(GC9107_SLPIN);
			MILLISEC_DELAY(_sleepDelay);
			break;
		case PowerState_e::SleepPartialIdleOn:
			writeCommand(GC9107_PTLON);
			writeCommand(GC9107_IDLEON);
			writeCommand(GC9107_SLPIN);
			MILLISEC_DELAY(_sleepDelay);
			break;
	}
	_currentPowerState = mode;
}

/*!
	@brief GC9107 combined init sequence for 128x128 and 128x160 memory base variants.
	@details  Variant-specific registers selected via _memoryBase enum.
*/
void GC9107_LTSM::TFTInitSequence(void)
{
	// === Common preamble - both variants ===
	writeCommand(GC9107_INREGEN1);      // 0xFE - Enable inter-register access
	writeCommand(GC9107_INREGEN2);      // 0xEF

	writeCommand(0xB0);                 // Undocumented
	writeData(0xC0);
	writeCommand(0xB2);                 // Access gate: E8h(b0), E9h(b1), EAh(b2)
	writeData(0x2F);
	writeCommand(0xB3);                 // Access gate: F0h(b0), F1h(b1)
	writeData(0x03);
	writeCommand(0xB6);                 // Access gate: A8h(b0), ACh(b4), ADh(b5)
	writeData(0x19);
	writeCommand(0xB7);                 // Undocumented
	writeData(0x01);

	writeCommand(GC9107_RGB_COMP_PRIN); // 0xAC
	writeData(0xCB);

	// === Variant-specific registers ===
	switch (_memoryBase)
	{
		case GM_memory_base_e::MEMORY_BASE_GM_128x160:
			writeCommand(0xAB); // Undocumented
			writeData(0x07);

			writeCommand(GC9107_MADCTL); // 0x36
			writeData(_colorOrder);

			writeCommand(0xB4); // Undocumented
			writeData(0x00);

			writeCommand(GC9107_FRAME_RATE_SET); // 0xA8
			writeData(0x0C);

			writeCommand(GC9107_VGH_SET);// 0xE8
			writeData(0x23);
			writeCommand(GC9107_VGL_SET); // 0xE9
			writeData(0x47);
			writeCommand(GC9107_VGH_VGL_CLK); // 0xEA
			writeData(0x44);
			// Undocumented registers in datasheet
			writeCommand(0xEB);
			writeData(0xE0);
			writeCommand(0xED);
			writeData(0x03);
			writeCommand(0xC6);
			writeData(0x19);
			writeCommand(0xC7);
			writeData(0x10);

			TFTGammaSet();
			break;

		case GM_memory_base_e::MEMORY_BASE_GM_128x128:
			writeCommand(0xAB); // Undocumented
			writeData(0x0E);

			writeCommand(GC9107_MADCTL); // 0x36
			writeData(_colorOrder);

			writeCommand(0xB4);  // Undocumented
			writeData(0x04);

			writeCommand(GC9107_FRAME_RATE_SET); // 0xA8
			writeData(0x19);

			writeCommand(GC9107_VGH_SET);       // 0xE8
			writeData(0x24);
			writeCommand(GC9107_VGL_SET);       // 0xE9
			writeData(0x48);
			writeCommand(GC9107_VGH_VGL_CLK);   // 0xEA
			writeData(0x22);
			// Undocumented registers in datasheet
			writeCommand(0xC6);
			writeData(0x30);
			writeCommand(0xC7);
			writeData(0x18);

			TFTGammaSet();
			break;
	}

	// === Common registers - identical in both variants ===
	writeCommand(GC9107_COLMOD);        // 0x3A - RGB565
	writeData(0x05);
	writeCommand(0xB8);                 // Undocumented
	writeData(0x08);

	// === Common tail ===
	writeCommand(GC9107_SLPOUT);        // 0x11
	MILLISEC_DELAY(120);
	writeCommand(GC9107_DISPON);        // 0x29
	MILLISEC_DELAY(10);
	TFTsetRotation(Degrees_0);
}

/*! @brief Helper function for TFTinitSequence() Sends out the Gamma Sequence */
void GC9107_LTSM::TFTGammaSet(void) {
	switch (_memoryBase)
	{
	case GM_memory_base_e::MEMORY_BASE_GM_128x160:
		{
		writeCommand(GC9107_GAMMA1);        // 0xF0
		uint8_t GammaOne[] = {0x0B, 0x2F, 0x10, 0x4B, 0x28, 0x3F, 0x3E, 0x60, 0x00, 0x12, 0x12, 0x0F, 0x00, 0x1F};
		spiWriteDataBuffer(GammaOne, sizeof(GammaOne));
		writeCommand(GC9107_GAMMA2);        // 0xF1
		uint8_t GammaTwo[] = {0x0E, 0x3D, 0x2A, 0x40, 0xFA, 0x00, 0x02, 0x60, 0x00, 0x03, 0x13, 0x00, 0x10, 0x1F};
		spiWriteDataBuffer(GammaTwo, sizeof(GammaTwo));
		}
	break;
	case GM_memory_base_e::MEMORY_BASE_GM_128x128:
		{
		writeCommand(GC9107_GAMMA1);        // 0xF0
		uint8_t GammaOneB[] = {0x1F, 0x28, 0x04, 0x3E, 0x2A, 0x2E, 0x20, 0x00, 0x0C, 0x06, 0x00, 0x1C, 0x1F, 0x0F};
		spiWriteDataBuffer(GammaOneB, sizeof(GammaOneB));
		writeCommand(GC9107_GAMMA2);        // 0xF1
		uint8_t GammaTwoB[] = {0x00, 0x2D, 0x2F, 0x3C, 0x6F, 0x1C, 0x0B, 0x00, 0x00, 0x00, 0x07, 0x0D, 0x11, 0x0F};
		spiWriteDataBuffer(GammaTwoB, sizeof(GammaTwoB));
		}
	break;
	}
}

/*!
	@brief Set the panel RAM offsets for displays where the visible area
		is smaller than the VRAM. Portrait X/Y offsets are supplied;
		landscape offsets are derived automatically for all rotations.
	@param colOffset  Columns of dead RAM to the left  of the panel in portrait (0°)
	@param rowOffset  Rows of dead RAM above the panel in portrait (0°)
*/
void GC9107_LTSM::TFTsetPanelOffset(uint8_t colOffset, uint8_t rowOffset)
{
	_colOffset = colOffset;
	_rowOffset = rowOffset;
}
// === EOF ===
