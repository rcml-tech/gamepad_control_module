
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <dinput.h>
#include <process.h>
#include <thread>
#include <string>
#include <vector>

#include <SimpleIni.h>

// GLOBAL_VARIABLES
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// Функция для EnumDevices
BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);

DWORD WINAPI ThreadKeyboardScanner(PVOID pvParam){

	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD fdwSaveOldMode;

	if (hStdin == INVALID_HANDLE_VALUE) {
		throw std::exception();
	}

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
		throw std::exception(); //error
	}

	DWORD fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode)) {
		throw std::exception(); //error
	}

	INPUT_RECORD irInBuf[128];
	DWORD cNumRead;

	while (true)
	{
		if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead))  {
			throw std::exception(); //error
		}
		for (int i = 0; i < cNumRead; ++i) {
			if (irInBuf[i].EventType == KEY_EVENT) {		
				*((bool *)pvParam) = true;
				break;
			}
		}
	}
	
	return(0);
};


// MACRO
#define CALIBRATE_BUTTONS(STRING_MESSAGE,ARRAY_INDEX) \
std::cout << STRING_MESSAGE << std::endl; \
while (true) \
{ \
	if (MessageFromThread){ \
		MessageFromThread = !MessageFromThread; \
	} \
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
	else if (MessageFromThread){\
		ArrayOfBytesOfButtons[ARRAY_INDEX] = 0;\
		break;\
	}\
}\
std::cout << ArrayOfBytesOfButtons[ARRAY_INDEX] << std::endl;\
Sleep(500);
/////


#define CALIBRATE_STICKS(STRING_MESSAGE,ARRAY_INDEX) \
	std::cout << STRING_MESSAGE << std::endl; \
	while (true) \
	{ \
		if (MessageFromThread){ \
			MessageFromThread = !MessageFromThread; \
		} \
 \
		Sleep(1000); \
		joystick->Poll(); \
		joystick->GetDeviceState(sizeof(JState), &JState); \
 \
		if (JState.lRz != 32767){ \
			ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lRz; \
			ArrayOfPositionsOfSticks[ARRAY_INDEX] = 13; \
			std::cout << JState.lRz << " 13"<< std::endl; \
			break; \
		} \
		if (JState.lX != 32767){ \
			ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lX; \
			ArrayOfPositionsOfSticks[ARRAY_INDEX] = 14; \
			std::cout << JState.lX << " 14"<< std::endl; \
			break; \
		} \
		if (JState.lY != 32767){ \
			ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lY; \
			ArrayOfPositionsOfSticks[ARRAY_INDEX] = 15; \
			std::cout << JState.lY << " 15"<< std::endl; \
			break; \
		} \
		if (JState.lZ != 32767){ \
			ArrayOfValuseOfSticks[ARRAY_INDEX] = JState.lZ; \
			ArrayOfPositionsOfSticks[ARRAY_INDEX] = 16; \
			std::cout << JState.lZ << " 16"<< std::endl; \
			break; \
		} \
	};



int main(){

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

	// Устанавливаем формат данных
	if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick))){
		std::cout << "cant Set DATA" << std::endl;
	}

	// 
	DIPROPRANGE dipr;
	DIJOYSTATE JState;
	ZeroMemory(&JState, sizeof(DIJOYSTATE));
	// Сначала очищаем структуру
	ZeroMemory(&dipr, sizeof(DIPROPRANGE));

	dipr.diph.dwSize = sizeof(dipr);
	dipr.diph.dwHeaderSize = sizeof(dipr);
	// Для оси X
	dipr.diph.dwObj = DIJOFS_X;
	dipr.diph.dwHow = DIPH_BYOFFSET; // Смещение в формате данных
	// диапазон значений
	dipr.lMin = -1024;
	dipr.lMax = 1024;
	// Set to X
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);
	// Set to Y
	dipr.diph.dwObj = DIJOFS_Y;
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);

	// Установка мертвой зоны
	DIPROPDWORD dipdw;
	ZeroMemory(&dipdw, sizeof(DIPROPDWORD));

	dipdw.diph.dwObj = DIJOFS_X; // Ось X
	dipdw.dwData = 1000;     // Приблизительно 10% диапазона
	// Dead zone to X
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	// dead zone to Y
	dipdw.diph.dwObj = DIJOFS_Y; // Ось Y
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

	bool MessageFromThread = false;
	DWORD dwThreadID1;
	HANDLE hThread5 = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall *)(void*)) &ThreadKeyboardScanner, &MessageFromThread, 0, (unsigned *)&dwThreadID1);

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



	WCHAR DllPath[MAX_PATH] = { 0 };
	GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, (DWORD)MAX_PATH);

	WCHAR *tmp = wcsrchr(DllPath, L'\\');
	WCHAR wConfigPath[MAX_PATH] = { 0 };

	size_t path_len = tmp - DllPath;

	wcsncpy(wConfigPath, DllPath, path_len);
	wcscat(wConfigPath, L"\\config.ini");

	char ConfigPath[MAX_PATH] = { 0 };
	wcstombs(ConfigPath, wConfigPath, sizeof(ConfigPath));

	CSimpleIniA ini;
	ini.SetMultiKey(true);

	if (ini.LoadFile(ConfigPath) < 0) {
		std::cout << "can't load config file" << std::endl;
		return 1;
	}

	CSimpleIniA::TNamesDepend axis_names_ini;
	ini.GetAllKeys("buttons", axis_names_ini);

	int countPairs = 0;
	int ArrayOfPositions[18];
	for (int i = 0; i < 18; i++){
			if (i < 12)
				{ ArrayOfPositions[i] = ArrayOfBytesOfButtons[i];}
			else if (i>=12 && i<16)
				{ ArrayOfPositions[i] = ArrayOfPositionsOfSticks[(i-12)*2];}
			else{ ArrayOfPositions[i] = i+1; }
	}

	for (
		auto i = axis_names.begin();
		i != axis_names.end();
		++i)
	{
		long temp = 0;
		temp = ini.GetLongValue("buttons", (*i).c_str(), 0);

		ini.Delete("buttons", (*i).c_str(), true);
		ini.SetValue("buttons", (*i).c_str(), std::to_string(ArrayOfPositions[countPairs]).c_str());

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
		countPairs++;
	}

	ini.SaveFile(ConfigPath);

	return 0;
}


BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
	*((GUID *)pvRef) = (pdInst->guidInstance);
	return DIENUM_STOP;
}

