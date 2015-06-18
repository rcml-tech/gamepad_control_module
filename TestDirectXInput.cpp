

#include <iostream>
#include <dinput.h>
//#include <Xinput.h>

// Функция для EnumDevices
BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);
BOOL CALLBACK callForEnumAxxis(LPDIENUMDEVICEOBJECTSCALLBACKW pdInst, LPVOID pvRef);

/*
LPDIENUMDEVICEOBJECTSCALLBACK testCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi,LPVOID pvRef);
*/
//GUID JoyGUID;


int main(){
	std::cout << "works!" << std::endl;

	LPDIRECTINPUT8 di;
	HRESULT hr;

	// Create a DirectInput device
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL))) {
		return hr;
	}

	LPDIRECTINPUTDEVICE8W joystick;
	GUID JoyGUID;

	if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, callForEnumDevices, &JoyGUID, DIEDFL_ATTACHEDONLY))){
		std::cout << "cant enum" << std::endl;
		return hr;
	};
	if (FAILED(hr = di->CreateDevice(JoyGUID, &joystick, NULL))){
		std::cout << "cant CreateDevice" << std::endl;
		return hr;
	};
	std::cout << JoyGUID.Data1 << std::endl;

	// Устанавливаем формат данных
	if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick))){
		std::cout << "cant Set DATA" << std::endl;
	}
	// 
	HWND hwndC = GetConsoleWindow();
	if (FAILED(hr = joystick->SetCooperativeLevel(hwndC, DISCL_FOREGROUND | DISCL_EXCLUSIVE))){
		std::cout << "cant Set COOP" << std::endl;
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
	dipdw.dwData = 1500;     // Приблизительно 15% диапазона
	// Dead zone to X
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	// dead zone to Y
	dipdw.diph.dwObj = DIJOFS_Y; // Ось Y
	joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);


	//LPDIENUMDEVICEOBJECTSCALLBACKW test;
	//joystick->EnumObjects(test, NULL, DIDFT_AXIS);

	DIDEVICEINSTANCE joyInfo;
	ZeroMemory(&joyInfo, sizeof(LPDIDEVICEINSTANCE));
	joyInfo.dwSize = (DWORD) sizeof(joyInfo);
	joystick->GetDeviceInfo(&joyInfo);
	//joyInfo->tszInstanceName;
	std::cout << "from joy: "  << joyInfo.tszInstanceName << std::endl;
	std::cout << "from joy: " << joyInfo.guidInstance.Data1 << std::endl;

	//HRESULT rez;
	//rez = joystick->Acquire();
	//std::cout << rez << std::endl;
	if (FAILED(hr = joystick->Acquire())) {
		std::cout << hr << std::endl;
		std::cout << S_FALSE << " " << DI_OK << std::endl;
		std::cout << DIERR_INVALIDPARAM <<" " << DIERR_NOTINITIALIZED << " " << DIERR_OTHERAPPHASPRIO << std::endl;
		return hr;
	}

	for (int i = 0; i < 2000; i++){
		joystick->Poll();

		joystick->GetDeviceState(sizeof(JState),&JState);

		std::cout << JState.lX << " " << JState.lY << " " << JState.lRx << " " <<   std::endl;
		for (int i = 0; i < 32; i++){
			std::cout << JState.rgbButtons[i] << "; " ;
		}
		std::cout << "" << std::endl;

		Sleep(50);
	}
	joystick->Unacquire();

	//Xinput
	//_XINPUT_GAMEPAD GamepadStruct;
	//XINPUT_STATE GamepadState;
	//ZeroMemory(&GamepadState, sizeof(XINPUT_STATE));
	//XInputGetState(0, &GamepadState);

	return 0;
}


BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
	std::cout << "callback function works!" << std::endl;
	std::cout << pdInst->tszInstanceName << std::endl;
	//static_cast
	//reinterpret_cast<GUID *> (pvRef);
	*((GUID *)pvRef) = (pdInst->guidInstance);

	//JoyGUID = pdInst->guidInstance;
	std::cout << (*((GUID *)pvRef)).Data1 << std::endl;
	return DIENUM_STOP;
}

BOOL CALLBACK callForEnumAxxis(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
	//LPDIENUMDEVICEOBJECTSCALLBACK
	return false;
}
/*
LPDIENUMDEVICEOBJECTSCALLBACKW testCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef){

}
*/