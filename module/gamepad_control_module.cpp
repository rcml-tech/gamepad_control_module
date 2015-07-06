#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif	

#include <stdlib.h>
#include <time.h>
#include <string>
#include <map>
#include <stdarg.h>

#include <iostream>

#ifdef _WIN32
	#include <dinput.h>
#else
	#include <unistd.h>
	#include <fcntl.h>
	#include <dlfcn.h> 
	#include "joystick.hh"
#endif

#include <module.h>
#include <control_module.h>
#include <SimpleIni.h>
#include "gamepad_control_module.h"

// GLOBAL VARIABLES
unsigned int COUNT_AXIS = 18;

bool is_error_init = false;

// All buttons
#ifdef _WIN32
	bool is_Button_was_changed[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

// MACROSES
#ifdef _WIN32
	#define SEND_BUTTON_CHANGE(index_of_button)\
	if (JState.rgbButtons[index_of_button] != 0){\
	(*sendAxisState)(index_of_button + 1, 1);\
	is_Button_was_changed[index_of_button] = true;\
	}\
	else {\
		if (is_Button_was_changed[index_of_button]){\
			(*sendAxisState)(index_of_button + 1, 0);\
			is_Button_was_changed[index_of_button] = false;\
		}\
	}
#endif


inline const char *copyStrValue(const char *source) {
	char *dest = new char[strlen(source) + 1];
	strcpy(dest, source);
	return dest;
}

#ifdef _WIN32
	BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
		*((GUID *)pvRef) = (pdInst->guidInstance);
		return DIENUM_STOP;
	}
#endif

void GamepadControlModule::execute(sendAxisState_t sendAxisState) {
	//srand(time(NULL));
#ifdef _WIN32	
	HRESULT hr;
	if (FAILED(hr = joystick->Acquire())) {
		return;
	}
	try {
		while (true) {
			joystick->Poll();
			joystick->GetDeviceState(sizeof(JState), &JState);
			for (auto i = axis_bind_map.begin(); i != axis_bind_map.end(); i++){
				switch (i->first)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12: // binary axis
				{
					if (JState.rgbButtons[i->first - 1] != 0 && i->second=="Exit"){
						throw std::exception();
					}
					if (JState.rgbButtons[i->first - 1] != 0){
						(*sendAxisState)(axis_names[i->second], axis[axis_names[i->second]]->upper_value);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], axis[axis_names[i->second]]->lower_value);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 13: // .lRz
				{
					if (JState.lRz != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lRz);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 14: // .lX
				{
					if (JState.lX != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lX);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 15: // .lY
				{
					if (JState.lY != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lY);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 16: // .lZ
				{
					if (JState.lZ != 32767){
						(*sendAxisState)(axis_names[i->second], JState.lZ);
						is_Button_was_changed[i->first - 1] = true;
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 32767);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 17:{ // Arrows UD
					if (JState.rgdwPOV[0] != 4294967295){
						switch (JState.rgdwPOV[0]){
						case 0:
						case 4500:
						case 4500 * 7:{
							(*sendAxisState)(axis_names[i->second], 1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						case 3 * 4500:
						case 4 * 4500:
						case 5 * 4500:{
							(*sendAxisState)(axis_names[i->second], -1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						default:{break; }
						}
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 0);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				case 18:{ // Arrows LR
					if (JState.rgdwPOV[0] != 4294967295){
						switch (JState.rgdwPOV[0]){
						case 4500:
						case 2 * 4500:
						case 3 * 4500:{
							(*sendAxisState)(axis_names[i->second], 1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						case 5 * 4500:
						case 6 * 4500:
						case 7 * 4500:{
							(*sendAxisState)(axis_names[i->second], -1);
							is_Button_was_changed[i->first - 1] = true;
							break;
						}
						default:{ break; }
						}
					}
					else {
						if (is_Button_was_changed[i->first - 1]){
							(*sendAxisState)(axis_names[i->second], 0);
							is_Button_was_changed[i->first - 1] = false;
						}
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
	catch (...){}
	joystick->Unacquire();
#else
	try{
		JoystickEvent event;
		while(true){
			joystick->sample(&event);
			if (!event.isInitialState()){
				break;
			} 
		}

		while (true)
		{
			// Restrict rate
			usleep(1000);

			// Attempt to sample an event from the joystick
			if (joystick->sample(&event))
			{

				if (event.isButton())
				{
					switch(event.number){
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:						
						case 10:
						case 11:
						{
							if (axis_bind_map.count(event.number+1)){
								if (axis_bind_map[event.number+1] == "Exit" &&  event.value){
									throw std::exception();
								}
								(*sendAxisState)(axis_names[axis_bind_map[event.number+1]], event.value);
							}
							break;
						}
						default:{ break;}
					}
			    }
				else if (event.isAxis())
				{
					switch(event.number){
						case 0:
						case 1:
						{
							if (axis_bind_map.count(event.number + 13)){
								(*sendAxisState)(axis_names[axis_bind_map[event.number + 13]], event.value);
							}
							break;
						}
						case 3:
						case 4:
						case 5:
						case 6:
						{
							if (axis_bind_map.count(event.number + 12)){
								(*sendAxisState)(axis_names[axis_bind_map[event.number + 12]], event.value);
							}
							break;
						}
						default:{ break;}
					}
			    }
			}
		}

	}
	catch(...){}
#endif
}


void GamepadControlModule::prepare(colorPrintfModule_t *colorPrintf_p, colorPrintfModuleVA_t *colorPrintfVA_p) {
	srand(time(NULL));
	this->colorPrintf_p = colorPrintfVA_p;

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
	Dl_info PathToSharedObject;
	void * pointer = reinterpret_cast<void*> (getControlModuleObject);
	dladdr(pointer, &PathToSharedObject);
	std::string dltemp(PathToSharedObject.dli_fname);

	int dlfound = dltemp.find_last_of("/");

	dltemp = dltemp.substr(0, dlfound);
	dltemp += "/config.ini";

	const char* ConfigPath = dltemp.c_str();
#endif

	CSimpleIniA ini;
	ini.SetMultiKey(true);

	if (ini.LoadFile(ConfigPath) < 0) {
		colorPrintf(ConsoleColor(ConsoleColor::yellow), "Can't load '%s' file!\n", ConfigPath);
		is_error_init = true;
		return;
	}

#ifndef _WIN32
	const char* tempInput;
	tempInput = ini.GetValue("options","input_path",NULL);
	if (tempInput == NULL) {
		colorPrintf(ConsoleColor(ConsoleColor::yellow), "Can't recieve path to input device");
		is_error_init = true;
		return;
	}
	InputDevice.assign(tempInput);
#endif

	int axis_id = 1;
	CSimpleIniA::TNamesDepend axis_names_ini;
	ini.GetAllKeys("axis", axis_names_ini);
	for (
		CSimpleIniA::TNamesDepend::const_iterator i = axis_names_ini.begin();
		i != axis_names_ini.end();
		++i)
	{
		long temp = 0;
		temp = ini.GetLongValue("axis", i->pItem, 0);

		if (temp){
			std::string col_axis(i->pItem);
			std::pair<int, std::string> tempPair(temp, col_axis);
			axis_bind_map.insert(tempPair);

			axis_names[col_axis] = axis_id;
			axis[axis_id] = new AxisData();
			axis[axis_id]->axis_index = axis_id;
			axis[axis_id]->name = copyStrValue(col_axis.c_str());
			axis[axis_id]->upper_value = ini.GetLongValue(i->pItem, "upper_value", 1);
			axis[axis_id]->lower_value = ini.GetLongValue(i->pItem, "lower_value", 0);

			axis_id++;
		}
	}

	COUNT_AXIS = axis.size();
	Gamepad_axis = new AxisData*[COUNT_AXIS];
	for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
		Gamepad_axis[j] = axis[j + 1];
	}
}

int GamepadControlModule::init() {
	if (is_error_init){
		return 1;
	}

#ifdef _WIN32
	LPDIRECTINPUT8 di;
	HRESULT hr;

	// Create a DirectInput device
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL))) {
		return hr;
	}

	//LPDIRECTINPUTDEVICE8W joystick;
	ZeroMemory(&joystick, sizeof(LPDIRECTINPUTDEVICE8W));
	GUID JoyGUID;

	if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, callForEnumDevices, &JoyGUID, DIEDFL_ATTACHEDONLY))){
		colorPrintf(ConsoleColor(ConsoleColor::red),"cant Enum Devices");
		return hr;
	};
	if (FAILED(hr = di->CreateDevice(JoyGUID, &joystick, NULL))){
		colorPrintf(ConsoleColor(ConsoleColor::red),"cant Create Device");
		return hr;
	};

	// ������������� ������ ������
	if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick))){
		colorPrintf(ConsoleColor(ConsoleColor::red),"cant Set DATA");
		return hr;
	}
	
	// 
	DIPROPRANGE dipr;
	// ������� ������� ���������
	ZeroMemory(&JState, sizeof(DIJOYSTATE));
	ZeroMemory(&dipr, sizeof(DIPROPRANGE));

	dipr.diph.dwSize = sizeof(dipr);
	dipr.diph.dwHeaderSize = sizeof(dipr);
	// ��� ��� X
	dipr.diph.dwObj = DIJOFS_X;
	dipr.diph.dwHow = DIPH_BYOFFSET; // �������� � ������� ������
	// �������� ��������
	dipr.lMin = -1024;
	dipr.lMax = 1024;
	// Set to X
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);
	// Set to Y
	dipr.diph.dwObj = DIJOFS_Y;
	joystick->SetProperty(DIPROP_RANGE, &dipr.diph);

	// ��������� ������� ����
	DIPROPDWORD dipdw;
	ZeroMemory(&dipdw, sizeof(DIPROPDWORD));

	dipdw.diph.dwObj = DIJOFS_X; // ��� X
	dipdw.dwData = 1500;     // �������������� 15% ���������
	// Dead zone to X
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	// dead zone to Y
	dipdw.diph.dwObj = DIJOFS_Y; // ��� Y
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
#else
	joystick = new Joystick(InputDevice);
	if (!joystick->isFound())
	{
		colorPrintf(ConsoleColor(ConsoleColor::yellow),"Open gamepad file failed.\n");
		return 1;
	}
#endif
	return  0;
}

const char *GamepadControlModule::getUID() {
	return "Gamepad control module 1";
}

AxisData** GamepadControlModule::getAxis(unsigned int *count_axis) {
	(*count_axis) = COUNT_AXIS;
	return Gamepad_axis;
}

void GamepadControlModule::destroy() {
	for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
		delete Gamepad_axis[j];
	}
	delete[] Gamepad_axis;
#ifndef _WIN32
	//delete joystick;
#endif
	delete this;

}

void *GamepadControlModule::writePC(unsigned int *buffer_length) {
	*buffer_length = 0;
	return NULL;
}

int GamepadControlModule::startProgram(int uniq_index) {
	return 0;
}

void GamepadControlModule::readPC(void *buffer, unsigned int buffer_length) {
}

int GamepadControlModule::endProgram(int uniq_index) {
	return 0;
}

void GamepadControlModule::colorPrintf(ConsoleColor colors, const char *mask, ...) {
	va_list args;
	va_start(args, mask);
	(*colorPrintf_p)(this, colors, mask, args);
	va_end(args);
}

PREFIX_FUNC_DLL  ControlModule* getControlModuleObject() {
	return new GamepadControlModule();
}