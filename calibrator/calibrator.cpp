#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif	

#include <iostream>

#ifdef _WIN32
	#include <dinput.h>
	#include <process.h>
	#include <thread>
#else
	#include <math.h>
	#include <limits.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <dlfcn.h> 
	#include "joystick.hh"
#endif	

#include <string>
#include <vector>

#include <SimpleIni.h>

// GLOBAL_VARIABLES
#ifdef _WIN32
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
	long zero_sticks_positions[4];

	// EnumDevices
	BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);

	// MACRO
	#define ZERO_CALIBRATION(STRING_MESSAGE) \
		std::cout << STRING_MESSAGE << std::endl; \
		Sleep(2500); \
		joystick->Poll(); \
		joystick->GetDeviceState(sizeof(JState), &JState); \
		zero_sticks_positions[0] = JState.lRz; \
		zero_sticks_positions[1] = JState.lX; \
		zero_sticks_positions[2] = JState.lY; \
		zero_sticks_positions[3] = JState.lZ; \
		Sleep(500);


	#define CALIBRATE_BUTTONS(STRING_MESSAGE,ARRAY_INDEX) \
	std::cout << STRING_MESSAGE << std::endl; \
	for (int j=0;j<300;j++) \
	{ \
		joystick->Poll(); \
		joystick->GetDeviceState(sizeof(JState), &JState); \
	 \
		int button_byte_position = -1; \
		bool is_two_pressed = true; \
		for (int i = 0; i < 12; i++){ \
			if (JState.rgbButtons[i] != 0){ \
				is_two_pressed = false; \
				if (button_byte_position != -1){ \
					is_two_pressed = true; \
					break; \
				} \
				button_byte_position = i + 1; \
			} \
		} \
		if (!is_two_pressed) {\
			ArrayOfBytesOfButtons[ARRAY_INDEX] = button_byte_position;\
			break;\
		}\
		else {\
			ArrayOfBytesOfButtons[ARRAY_INDEX] = 0;\
		}\
		Sleep(20); \
	}\
	std::cout << ArrayOfBytesOfButtons[ARRAY_INDEX] << std::endl;\
	Sleep(500);
	/////


	#define CALIBRATE_STICKS(STRING_MESSAGE,ARRAY_INDEX) \
		std::cout << STRING_MESSAGE << std::endl; \
		for (int j=0;j<7;j++)  \
		{ \
			Sleep(1000); \
			joystick->Poll(); \
			joystick->GetDeviceState(sizeof(JState), &JState); \
	 \
			if (JState.lRz != zero_sticks_positions[0]){ \
				ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lRz; \
				ArrayOfPositionsOfSticks[ARRAY_INDEX] = 13; \
				std::cout << JState.lRz << " 13"<< std::endl; \
				break; \
			} \
			if (JState.lX != zero_sticks_positions[1]){ \
				ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lX; \
				ArrayOfPositionsOfSticks[ARRAY_INDEX] = 14; \
				std::cout << JState.lX << " 14"<< std::endl; \
				break; \
			} \
			if (JState.lY != zero_sticks_positions[2]){ \
				ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lY; \
				ArrayOfPositionsOfSticks[ARRAY_INDEX] = 15; \
				std::cout << JState.lY << " 15"<< std::endl; \
				break; \
			} \
			if (JState.lZ != zero_sticks_positions[3]){ \
				ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lZ; \
				ArrayOfPositionsOfSticks[ARRAY_INDEX] = 16; \
				std::cout << JState.lZ << " 16"<< std::endl; \
				break; \
			} \
		};
#else
// MACROSES
	#define CALIBRATE_BUTTONS(STRING_MESSAGE) \
		std::cout << "Press and Hold -" << STRING_MESSAGE << "- button" << std::endl; \
		for (int i=0;i<250;i++){ \
			if (joystick->sample(&event)) \
			{ \
				if ( event.isButton() && event.value) \
				{ \
					std::cout << "Success" << std::endl; \
					tempmap[STRING_MESSAGE] = event.number+1; \
					break; \
				} \
			} \
			usleep(20000); \
			if (i==249) { \
				std::cout << "button not defined" << std::endl; \
			} \
		}

#define CALIBRATE_STICKS(STRING_MESSAGE,AXIS_NAME,ARRAY_INDEX) \
	std::cout << STRING_MESSAGE << std::endl; \
  is_not_second_axxis = false; \
  is_min_value = false; \
  for (int j=0;j<5;j++){ \
    for (int i=0;i<100;i++){ \
      if (joystick->sample(&event)){ \
        if (event.isAxis() && event.value) \
        { \
          switch(event.number) \
          { \
            case 0: \
            case 1: \
            { \
              is_not_second_axxis = true; \
              if (event.value < 0) { is_min_value=true; } \
              if (!tempmap.count(AXIS_NAME)) \
              { \
                tempmap[AXIS_NAME] = event.number + 13; \
              } \
              TempArrayOfValues[i] = event.value; \
              break; \
            } \
            case 3: \
            case 4: \
            case 5: \
            case 6: \
            { \
              is_not_second_axxis = true; \
              if (event.value < 0) { is_min_value=true; } \
              if (!tempmap.count(AXIS_NAME)) \
              { \
                tempmap[AXIS_NAME] = event.number + 12; \
              } \
              TempArrayOfValues[i] = event.value; \
              break; \
            } \
            default:{ break; } \
          } \
        } \
      } \
      usleep(20000); \
    } \
    if (is_not_second_axxis){ \
      long maxofvalues=0; \
      for (int i=0;i<100;i++){ \
        if (abs(TempArrayOfValues[i]) > maxofvalues ){ \
          maxofvalues = abs(TempArrayOfValues[i]); \
        } \
      } \
      if (is_min_value) { maxofvalues= maxofvalues*(-1); } \
      ArrayOfValuseOfSticks[ARRAY_INDEX] = maxofvalues; \
      for (int i=0;i<100;i++){ \
        TempArrayOfValues[i]=0; \
      } \
      std::cout << "Success, release Stick" << std::endl; \
      for (int i=0; i<100; i++){\
        usleep(20000);\
        joystick->sample(&event);\
      }\
      break; \
    }\
    tempmap[AXIS_NAME] = 0; \
    ArrayOfValuseOfSticks[ARRAY_INDEX] = 0; \
    }; \
    if (!is_not_second_axxis){ \
    	std::cout << "Axis not defined" << std::endl; \
    	usleep(2000000); \
    } 
#endif	

int main(int argc, char* argv[]){

///// start with axis
	std::vector<std::string> axis_names;
	
	axis_names.push_back("Exit");

	axis_names.push_back("B1");
	axis_names.push_back("B2");
	axis_names.push_back("B3");
	axis_names.push_back("B4");

	axis_names.push_back("L1");
	axis_names.push_back("L2");
	axis_names.push_back("R1");
	axis_names.push_back("R2");

	axis_names.push_back("start");
	axis_names.push_back("T1");
	axis_names.push_back("T2");

	axis_names.push_back("RTUD");
	axis_names.push_back("RTLR");
	axis_names.push_back("LTUD");
	axis_names.push_back("LTLR");

	axis_names.push_back("arrowsUD");
	axis_names.push_back("arrowsLR");


#ifdef _WIN32
	LPDIRECTINPUT8 di;
	HRESULT hr;

	// Create a DirectInput device
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL))) {
		return hr;
	}

	LPDIRECTINPUTDEVICE8W joystick;
	ZeroMemory(&joystick, sizeof(LPDIRECTINPUTDEVICE8W));
	GUID JoyGUID;

	if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, callForEnumDevices, &JoyGUID, DIEDFL_ATTACHEDONLY))){
		std::cout << "cant enum" << std::endl;
		return hr;
	};
	if (FAILED(hr = di->CreateDevice(JoyGUID, &joystick, NULL))){
		std::cout << "cant CreateDevice" << std::endl;
		return hr;
	};

	if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick))){
		std::cout << "cant Set DATA" << std::endl;
	}

	// 
	DIPROPRANGE dipr;
	DIJOYSTATE JState;
	ZeroMemory(&JState, sizeof(DIJOYSTATE));
	ZeroMemory(&dipr, sizeof(DIPROPRANGE));

	dipr.diph.dwSize = sizeof(dipr);
	dipr.diph.dwHeaderSize = sizeof(dipr);

	dipr.diph.dwObj = DIJOFS_X;
	dipr.diph.dwHow = DIPH_BYOFFSET; 

	dipr.lMin = -1024;
	dipr.lMax = 1024;
	// Set to X
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);
	// Set to Y
	dipr.diph.dwObj = DIJOFS_Y;
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);

	
	DIPROPDWORD dipdw;
	ZeroMemory(&dipdw, sizeof(DIPROPDWORD));

	dipdw.diph.dwObj = DIJOFS_X; 
	dipdw.dwData = 1000;    
	// Dead zone to X
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	// dead zone to Y
	dipdw.diph.dwObj = DIJOFS_Y; 
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);


	if (FAILED(hr = joystick->Acquire())) {
		std::cout << hr << std::endl;
		std::cout << S_FALSE << " " << DI_OK << std::endl;
		std::cout << DIERR_INVALIDPARAM <<" " << DIERR_NOTINITIALIZED << " " << DIERR_OTHERAPPHASPRIO << std::endl;
		joystick->Unacquire();
		return hr;
	}


	////////
	int ArrayOfBytesOfButtons[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	long ArrayOfValuseOfSticks[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int ArrayOfPositionsOfSticks[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	ZERO_CALIBRATION ("don't touch sticks for 2 seconds");

	CALIBRATE_BUTTONS ("Press and Hold -Exit- button", 0);

	CALIBRATE_BUTTONS ("Press and Hold -B1- button",1);
	CALIBRATE_BUTTONS ("Press and Hold -B2- button",2);
	CALIBRATE_BUTTONS ("Press and Hold -B3- button",3);
	CALIBRATE_BUTTONS ("Press and Hold -B4- button",4);

	CALIBRATE_BUTTONS ("Press and Hold -L1- button",5);
	CALIBRATE_BUTTONS ("Press and Hold -L2- button",6);
	CALIBRATE_BUTTONS ("Press and Hold -R1- button",7);
	CALIBRATE_BUTTONS ("Press and Hold -R2- button",8);

	CALIBRATE_BUTTONS ("Press and Hold -start- button",9);
	CALIBRATE_BUTTONS ("Press and Hold -TL- button",10);
	CALIBRATE_BUTTONS ("Press and Hold -TR- button",11);

	CALIBRATE_STICKS ("Turn Right Stick UP",0);
	CALIBRATE_STICKS ("Turn Right Stick DOWN",1);
	CALIBRATE_STICKS ("Turn Right Stick LEFT",2);
	CALIBRATE_STICKS ("Turn Right Stick RIGHT",3);

	CALIBRATE_STICKS ("Turn Left Stick UP",4);
	CALIBRATE_STICKS ("Turn Left Stick DOWN",5);
	CALIBRATE_STICKS ("Turn Left Stick LEFT",6);
	CALIBRATE_STICKS ("Turn Left Stick RIGHT",7);

	joystick->Unacquire();	
#else
	Joystick *joystick;

	std::string dev("/dev/input/js");
	for (int i=0; i< 10; i++){
		std::string tdev = dev + std::to_string(i);
		std::cout << tdev << " try" << std::endl;
		joystick = new Joystick(tdev);
		if (!joystick->isFound())
		{
			printf("open failed.\n");
			return 1;
		}
		else {
			std::cout << "Press any button on gamepad" << std::endl;
			JoystickEvent event;
			int countIS=0;
			while(true){
				joystick->sample(&event);
				if (!event.isInitialState()){
					break;
				} 
				usleep(2000);
				countIS++;
				if (countIS > 4000){
					break;
				}
			}
			if (countIS < 4000){
				break;
			}
		}
	}

	JoystickEvent event;

/// Теперь по порядку просим нажать кнопки и на основе этого будем делать записи в Массив.
	std::map<std::string, int> tempmap;

	CALIBRATE_BUTTONS ("Exit");

	CALIBRATE_BUTTONS ("B1");
	CALIBRATE_BUTTONS ("B2");
	CALIBRATE_BUTTONS ("B3");
	CALIBRATE_BUTTONS ("B4");

	CALIBRATE_BUTTONS ("L1");
	CALIBRATE_BUTTONS ("L2");
	CALIBRATE_BUTTONS ("R1");
	CALIBRATE_BUTTONS ("R2");

	CALIBRATE_BUTTONS ("start");
	CALIBRATE_BUTTONS ("T1");
	CALIBRATE_BUTTONS ("T2");

////  CALIBRATE_STICKS
	long ArrayOfValuseOfSticks[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	bool is_not_second_axxis= false;
	bool is_min_value = false;

	long TempArrayOfValues[100];
	for (int i=0;i<100;i++){
		TempArrayOfValues[i]=0;
	}

	CALIBRATE_STICKS ("Turn Right Stick UP","RTUD", 0);
	CALIBRATE_STICKS ("Turn Right Stick DOWN","RTUD", 1);
	CALIBRATE_STICKS ("Turn Right Stick LEFT","RTLR", 2);
	CALIBRATE_STICKS ("Turn Right Stick RIGHT","RTLR", 3);

	CALIBRATE_STICKS ("Turn Left Stick UP","LTUD", 4);
	CALIBRATE_STICKS ("Turn Left Stick DOWN","LTUD", 5);
	CALIBRATE_STICKS ("Turn Left Stick LEFT","LTLR", 6);
	CALIBRATE_STICKS ("Turn Left Stick RIGHT","LTLR", 7);

	CALIBRATE_STICKS ("Turn D-pad UP", "arrowsUD",8);
	CALIBRATE_STICKS ("Turn D-pad DOWN", "arrowsUD",9);
	CALIBRATE_STICKS ("Turn D-pad LEFT", "arrowsLR",10);
	CALIBRATE_STICKS ("Turn D-pad RIGHT", "arrowsLR",11);
#endif	


#ifdef _WIN32
	WCHAR DllPath[MAX_PATH] = { 0 };
	GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, (DWORD)MAX_PATH);

	WCHAR *tmp = wcsrchr(DllPath, L'\\');
	WCHAR wConfigPath[MAX_PATH] = { 0 };

	size_t path_len = tmp - DllPath;

	wcsncpy(wConfigPath, DllPath, path_len);
	wcscat(wConfigPath, L"\\config.ini");

	char ConfigPath[MAX_PATH] = { 0 };
	wcstombs(ConfigPath, wConfigPath, sizeof(ConfigPath));
#else
	char buffer[PATH_MAX] ={0};
	char *path = realpath(argv[0],buffer); 
	std::string str(path);

	int dlfound = str.find_last_of("/");
	str = str.substr(0, dlfound);
	str += "/config.ini";

	const char* ConfigPath = str.c_str();
#endif

	CSimpleIniA ini;
	ini.SetMultiKey(true);

	if (ini.LoadFile(ConfigPath) < 0) {
		std::cout << "can't load config file " << ConfigPath << std::endl;
		return 1;
	}

	int countPairs = 0;
#ifdef _WIN32

	int ArrayOfPositions[18];
	for (int i = 0; i < 18; i++){
		if (i < 12)
			{ ArrayOfPositions[i] = ArrayOfBytesOfButtons[i];}
		else if (i>=12 && i<16)
			{ ArrayOfPositions[i] = ArrayOfPositionsOfSticks[(i-12)*2];}
		else{ ArrayOfPositions[i] = i+1; }
	}
#else
#endif

	for (
		auto i = axis_names.begin();
		i != axis_names.end();
		++i)
	{
		ini.Delete("axis", (*i).c_str(), true);
		
#ifdef _WIN32		
		ini.SetValue("axis", (*i).c_str(), std::to_string(ArrayOfPositions[countPairs]).c_str());

		ini.Delete((*i).c_str(), "upper_value", true);
		ini.Delete((*i).c_str(), "lower_value", true);

		if (countPairs > 0 && countPairs < 12)
		{
			ini.SetValue((*i).c_str(), "upper_value", "1");
			ini.SetValue((*i).c_str(), "lower_value", "0");
		}
		else if (countPairs >= 12 && countPairs<16)
		{ 
			ini.SetValue((*i).c_str(), "upper_value", std::to_string(ArrayOfValuseOfSticks[(countPairs - 12)*2]).c_str());
			ini.SetValue((*i).c_str(), "lower_value", std::to_string(ArrayOfValuseOfSticks[(countPairs - 12)*2 +1]).c_str());
		}
		else if (countPairs >= 16)
		{ 
			ini.SetValue((*i).c_str(), "upper_value", "1");
			ini.SetValue((*i).c_str(), "lower_value", "-1");
		}
#else
		ini.SetValue("axis", (*i).c_str(), std::to_string(tempmap[(*i)]).c_str());

		ini.Delete((*i).c_str(), "upper_value", true);
		ini.Delete((*i).c_str(), "lower_value", true);

		if (countPairs > 0 && countPairs < 12)
		{
			ini.SetValue((*i).c_str(), "upper_value", "1");
			ini.SetValue((*i).c_str(), "lower_value", "0");
		}
		else if (countPairs >= 12 )
		{ 
			ini.SetValue((*i).c_str(), "upper_value", std::to_string(ArrayOfValuseOfSticks[(countPairs - 12)*2]).c_str());
			ini.SetValue((*i).c_str(), "lower_value", std::to_string(ArrayOfValuseOfSticks[(countPairs - 12)*2 +1]).c_str());
		}
#endif
		countPairs++;
	}

	ini.SaveFile(ConfigPath);

	return 0;
}

#ifdef _WIN32
	BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
		*((GUID *)pvRef) = (pdInst->guidInstance);
		return DIENUM_STOP;
	}
#endif
