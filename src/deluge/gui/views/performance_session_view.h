/*
 * Copyright © 2014-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "definitions_cxx.hpp"
#include "gui/views/clip_navigation_timeline_view.h"
#include "hid/button.h"
#include "model/global_effectable/global_effectable.h"
#include "storage/flash_storage.h"

class Editor;
class InstrumentClip;
class Clip;
class ModelStack;
class ModelStackWithThreeMainThings;
class ModelStackWithAutoParam;

struct PadPress {
	bool isActive;
	int32_t xDisplay;
	int32_t yDisplay;
	Param::Kind paramKind;
	int32_t paramID;
};

struct FXColumnPress {
	int32_t previousKnobPosition;
	int32_t currentKnobPosition;
	int32_t yDisplay;
	uint32_t timeLastPadPress;
	bool padPressHeld;
};

struct ParamsForPerformance {
	Param::Kind paramKind;
	ParamType paramID;
	int32_t xDisplay;
	int32_t yDisplay;
	uint8_t rowColour[3];
	uint8_t rowTailColour[3];

	ParamsForPerformance() {
		paramKind = Param::Kind::NONE;
		paramID = kNoSelection;
		xDisplay = kNoSelection;
		yDisplay = kNoSelection;
		rowColour[0] = 0;
		rowColour[1] = 0;
		rowColour[2] = 0;
		rowTailColour[0] = 0;
		rowTailColour[1] = 0;
		rowTailColour[2] = 0;
	}

	ParamsForPerformance(Param::Kind kind, ParamType param, int32_t x, int32_t y, const uint8_t colour[3],
	                     const uint8_t tailColour[3]) {
		paramKind = kind;
		paramID = param;
		xDisplay = x;
		yDisplay = y;
		rowColour[0] = colour[0];
		rowColour[1] = colour[1];
		rowColour[2] = colour[2];
		rowTailColour[0] = tailColour[0];
		rowTailColour[1] = tailColour[1];
		rowTailColour[2] = tailColour[2];
	}
};

const int32_t sizePadPress = sizeof(PadPress);
const int32_t sizeFXPress = sizeof(FXColumnPress);
const int32_t sizeParamsForPerformance = sizeof(ParamsForPerformance);

class PerformanceSessionView final : public ClipNavigationTimelineView, public GlobalEffectable {
public:
	PerformanceSessionView();
	bool opened();
	void focusRegained();

	void graphicsRoutine();
	ActionResult timerCallback();

	//rendering
	bool renderMainPads(uint32_t whichRows, uint8_t image[][kDisplayWidth + kSideBarWidth][3],
	                    uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth], bool drawUndefinedArea = true);
	bool renderSidebar(uint32_t whichRows, uint8_t image[][kDisplayWidth + kSideBarWidth][3],
	                   uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth]);
	void renderViewDisplay();
	void renderOLED(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]);
	// 7SEG only
	void redrawNumericDisplay();
	void setLedStates();

	//button action
	ActionResult buttonAction(deluge::hid::Button b, bool on, bool inCardRoutine);

	//pad action
	ActionResult padAction(int32_t x, int32_t y, int32_t velocity);

	//horizontal encoder action
	ActionResult horizontalEncoderAction(int32_t offset);

	//vertical encoder action
	ActionResult verticalEncoderAction(int32_t offset, bool inCardRoutine);

	//mod encoder action
	void modEncoderAction(int32_t whichModEncoder, int32_t offset);
	void modEncoderButtonAction(uint8_t whichModEncoder, bool on);
	void modButtonAction(uint8_t whichButton, bool on);

	//select encoder action
	void selectEncoderAction(int8_t offset);

	//not sure why we need these...
	uint32_t getMaxZoom();
	uint32_t getMaxLength();

	//public so soundEditor can access it
	void savePerformanceViewLayout();
	void loadPerformanceViewLayout();
	void updateLayoutChangeStatus();
	void resetPerformanceView(ModelStackWithThreeMainThings* modelStack);
	bool defaultEditingMode;
	bool editingParam; //if you're not editing a param, you're editing a value
	bool justExitedSoundEditor;

	//public so Action Logger can access it
	FXColumnPress fxPress[kDisplayWidth];

	//public so midi follow can access it
	ModelStackWithAutoParam* getModelStackWithParam(ModelStackWithThreeMainThings* modelStack, int32_t paramID);

	//public so view.modEncoderAction and midi follow can access it
	PadPress lastPadPress;
	void renderFXDisplay(Param::Kind paramKind, int32_t paramID, int32_t knobPos = kNoSelection);
	bool onFXDisplay;

private:
	//initialize
	void initPadPress(PadPress& padPress);
	void initFXPress(FXColumnPress& columnPress);
	void initLayout(ParamsForPerformance& layout);
	void initDefaultFXValues(int32_t xDisplay);

	//rendering
	void renderRow(uint8_t* image, uint8_t occupancyMask[], int32_t yDisplay = 0);
	bool isParamAssignedToFXColumn(Param::Kind paramKind, int32_t paramID);
	void setCentralLEDStates();

	//pad action
	void normalPadAction(ModelStackWithThreeMainThings* modelStack, int32_t xDisplay, int32_t yDisplay, int32_t on);
	void paramEditorPadAction(ModelStackWithThreeMainThings* modelStack, int32_t xDisplay, int32_t yDisplay,
	                          int32_t on);
	bool isPadShortcut(int32_t xDisplay, int32_t yDisplay);
	bool setParameterValue(ModelStackWithThreeMainThings* modelStack, Param::Kind paramKind, int32_t paramID,
	                       int32_t xDisplay, int32_t knobPos, bool renderDisplay = true);
	void getParameterValue(ModelStackWithThreeMainThings* modelStack, Param::Kind paramKind, int32_t paramID,
	                       int32_t xDisplay, bool renderDisplay = true);
	void padPressAction(ModelStackWithThreeMainThings* modelStack, Param::Kind paramKind, int32_t paramID,
	                    int32_t xDisplay, int32_t yDisplay, bool renderDisplay = true);
	void padReleaseAction(ModelStackWithThreeMainThings* modelStack, Param::Kind paramKind, int32_t paramID,
	                      int32_t xDisplay, bool renderDisplay = true);
	void resetFXColumn(ModelStackWithThreeMainThings* modelStack, int32_t xDisplay);
	bool isParamStutter(Param::Kind paramKind, int32_t paramID);
	void releaseStutter(ModelStackWithThreeMainThings* modelStack);

	/// write/load default values
	void writeDefaultsToFile();
	void writeDefaultFXValuesToFile();
	void writeDefaultFXParamToFile(int32_t xDisplay);
	void writeDefaultFXRowValuesToFile(int32_t xDisplay);
	void writeDefaultFXHoldStatusToFile(int32_t xDisplay);
	void loadDefaultLayout();
	void readDefaultsFromBackedUpFile();
	void readDefaultsFromFile();
	void readDefaultFXValuesFromFile();
	void readDefaultFXParamAndRowValuesFromFile(int32_t xDisplay);
	void readDefaultFXParamFromFile(int32_t xDisplay);
	void readDefaultFXRowNumberValuesFromFile(int32_t xDisplay);
	void readDefaultFXHoldStatusFromFile(int32_t xDisplay);
	void initializeHeldFX(int32_t xDisplay);
	bool successfullyReadDefaultsFromFile;
	bool anyChangesToSave;

	/// backup loaded layout (what's currently in XML file)
	/// backup the last loaded/last saved changes, so you can compare and let user know if any changes
	/// need to be saved
	FXColumnPress backupXMLDefaultFXPress[kDisplayWidth];
	ParamsForPerformance backupXMLDefaultLayoutForPerformance[kDisplayWidth];
	int32_t backupXMLDefaultFXValues[kDisplayWidth][kDisplayHeight];

	int32_t calculateKnobPosForSinglePadPress(int32_t xDisplay, int32_t yDisplay);
	int32_t calculateKnobPosForSelectEncoderTurn(int32_t knobPos, int32_t offset);
	int32_t adjustKnobPosForQuantizedStutter(int32_t yDisplay);

	PadPress firstPadPress;
	ParamsForPerformance layoutForPerformance[kDisplayWidth];
	int32_t defaultFXValues[kDisplayWidth][kDisplayHeight];
	int32_t layoutBank;    //A or B (assign a layout to the bank for cross fader action)
	int32_t layoutVariant; //1, 2, 3, 4, 5 (1 = Load, 2 = Synth, 3 = Kit, 4 = Midi, 5 = CV)

	//backup current layout
	void backupPerformanceLayout();
	bool performanceLayoutBackedUp;
	void logPerformanceViewPress(int32_t xDisplay, bool closeAction = true);
	bool anyChangesToLog();
	FXColumnPress backupFXPress[kDisplayWidth];

	// Members regarding rendering different layouts
private:
	bool sessionButtonActive = false;
	bool sessionButtonUsed = false;
};

extern PerformanceSessionView performanceSessionView;
