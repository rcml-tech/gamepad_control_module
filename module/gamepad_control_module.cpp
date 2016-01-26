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

// Global Variables
#define IID "RCT.Gamepad_control_module_v100"

#ifdef _WIN32
long long Button_previous_state[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0};
long zero_sticks_positions[4];
long long zero_arrows_positions;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif
int exit_gamepad_axxis_number = 0;

inline const char *copyStrValue(const char *source) {
  char *dest = new char[strlen(source) + 1];
  strcpy(dest, source);
  return dest;
}

#ifdef _WIN32
BOOL CALLBACK callForEnumDevices(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef) {
  *((GUID *)pvRef) = (pdInst->guidInstance);
  return DIENUM_STOP;
}
#endif

GamepadControlModule::GamepadControlModule() : is_error_init(false){
#ifndef CONTROL_MODULE_H_000
  mi = new ModuleInfo;
  mi->uid = IID;
  mi->mode = ModuleInfo::Modes::PROD;
  mi->version = BUILD_NUMBER;
  mi->digest = NULL;
#endif
};

void GamepadControlModule::execute(sendAxisState_t sendAxisState) {
// srand(time(NULL));
#ifdef _WIN32
  HRESULT hr;
  if (FAILED(hr = joystick->Acquire())) {
    return;
  }

  Button_previous_state[12] = zero_sticks_positions[0];
  Button_previous_state[13] = zero_sticks_positions[1];
  Button_previous_state[14] = zero_sticks_positions[2];
  Button_previous_state[15] = zero_sticks_positions[3];
  Button_previous_state[16] = zero_arrows_positions;
  Button_previous_state[17] = zero_arrows_positions;

  while (true) {
    joystick->Poll();
    joystick->GetDeviceState(sizeof(JState), &JState);
    for (auto i = gamepad_axis_bind_to_module_axis.begin();
         i != gamepad_axis_bind_to_module_axis.end(); i++) {
      switch (i->first) {
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
        case 12:  // binary axis
        {
          if (JState.rgbButtons[i->first - 1] !=
              Button_previous_state[i->first - 1]) {
            if (i->first == exit_gamepad_axxis_number) {
              return;
            }
            if (JState.rgbButtons[i->first - 1]) {
              (*sendAxisState)(this, i->second, axis[i->second]->upper_value);
            } else {
              (*sendAxisState)(this, i->second, axis[i->second]->lower_value);
            }
            Button_previous_state[i->first - 1] =
                JState.rgbButtons[i->first - 1];
          }
          break;
        }
        case 13:  // .lRz
        {
          if (JState.lRz != Button_previous_state[i->first - 1]) {
            (*sendAxisState)(this, i->second, JState.lRz);
            Button_previous_state[i->first - 1] = JState.lRz;
          }
          break;
        }
        case 14:  // .lX
        {
          if (JState.lX != Button_previous_state[i->first - 1]) {
            (*sendAxisState)(this, i->second, JState.lX);
            Button_previous_state[i->first - 1] = JState.lX;
          }
          break;
        }
        case 15:  // .lY
        {
          if (JState.lY != Button_previous_state[i->first - 1]) {
            (*sendAxisState)(this, i->second, JState.lY);
            Button_previous_state[i->first - 1] = JState.lY;
          }
          break;
        }
        case 16:  // .lZ
        {
          if (JState.lZ != Button_previous_state[i->first - 1]) {
            (*sendAxisState)(this, i->second, JState.lZ);
            Button_previous_state[i->first - 1] = JState.lZ;
          }
          break;
        }
        case 17: {  // Arrows UD
          if (JState.rgdwPOV[0] != Button_previous_state[i->first - 1]) {
            switch (JState.rgdwPOV[0]) {
              case 0:
              case 4500:
              case 4500 * 7: {
                (*sendAxisState)(this, i->second, 1);
                Button_previous_state[i->first - 1] = JState.rgdwPOV[0];
                break;
              }
              case 3 * 4500:
              case 4 * 4500:
              case 5 * 4500: {
                (*sendAxisState)(this, i->second, -1);
                Button_previous_state[i->first - 1] = JState.rgdwPOV[0];
                break;
              }
              default: { break; }
            }
            if (JState.rgdwPOV[0] == zero_arrows_positions) {
              (*sendAxisState)(this, i->second, 0);
              Button_previous_state[i->first - 1] = JState.rgdwPOV[0];
            }
          }
          break;
        }
        case 18: {  // Arrows LR
          if (JState.rgdwPOV[0] != Button_previous_state[i->first - 1]) {
            switch (JState.rgdwPOV[0]) {
              case 4500:
              case 2 * 4500:
              case 3 * 4500: {
                (*sendAxisState)(this, i->second, 1);
                Button_previous_state[i->first - 1] = JState.rgdwPOV[0];
                break;
              }
              case 5 * 4500:
              case 6 * 4500:
              case 7 * 4500: {
                (*sendAxisState)(this, i->second, -1);
                Button_previous_state[i->first - 1] = JState.rgdwPOV[0];
                break;
              }
              default: { break; }
            }
            if (JState.rgdwPOV[0] == zero_arrows_positions) {
              (*sendAxisState)(this, i->second, 0);
              Button_previous_state[i->first - 1] = JState.rgdwPOV[0];
            }
          }
          break;
        }
        default:
          break;
      }
    }
  }
  joystick->Unacquire();
#else
  JoystickEvent event;
  while (true) {
    joystick->sample(&event);
    if (!event.isInitialState()) {
      break;
    }
  }

  while (true) {
    // Restrict rate
    usleep(1000);

    // Attempt to sample an event from the joystick
    if (joystick->sample(&event)) {
      if (event.isButton()) {
        switch (event.number) {
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
          case 11: {
            if (gamepad_axis_bind_to_module_axis.count(event.number + 1)) {
              if (event.number + 1 == exit_gamepad_axxis_number &&
                  event.value) {
                return;
              }
              (*sendAxisState)(
                  this, gamepad_axis_bind_to_module_axis[event.number + 1],
                  event.value);
            }
            break;
          }
          default: { break; }
        }
      } else if (event.isAxis()) {
        switch (event.number) {
          case 0:
          case 1: {
            if (gamepad_axis_bind_to_module_axis.count(event.number + 13)) {
              (*sendAxisState)(
                  this, gamepad_axis_bind_to_module_axis[event.number + 13],
                  event.value);
            }
            break;
          }
          case 3:
          case 4:
          case 5:
          case 6: {
            if (gamepad_axis_bind_to_module_axis.count(event.number + 12)) {
              (*sendAxisState)(
                  this, gamepad_axis_bind_to_module_axis[event.number + 12],
                  event.value);
            }
            break;
          }
          default: { break; }
        }
      }
    }
  }
#endif
}

void GamepadControlModule::prepare(colorPrintfModule_t *colorPrintf_p,
                                   colorPrintfModuleVA_t *colorPrintfVA_p) {
  this->colorPrintf_p = colorPrintfVA_p;

#ifdef _WIN32
  WCHAR DllPath[MAX_PATH] = {0};
  GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, (DWORD)MAX_PATH);

  WCHAR *tmp = wcsrchr(DllPath, L'\\');
  WCHAR wConfigPath[MAX_PATH] = {0};

  size_t path_len = tmp - DllPath;

  wcsncpy(wConfigPath, DllPath, path_len);
  wcscat(wConfigPath, L"\\config.ini");

  char ConfigPath[MAX_PATH] = {0};
  wcstombs(ConfigPath, wConfigPath, sizeof(ConfigPath));
#else
  Dl_info PathToSharedObject;
  void *pointer = reinterpret_cast<void *>(getControlModuleObject);
  dladdr(pointer, &PathToSharedObject);
  std::string dltemp(PathToSharedObject.dli_fname);

  int dlfound = dltemp.find_last_of("/");

  dltemp = dltemp.substr(0, dlfound);
  dltemp += "/config.ini";

  const char *ConfigPath = dltemp.c_str();
#endif

  CSimpleIniA ini;
  ini.SetMultiKey(true);

  if (ini.LoadFile(ConfigPath) < 0) {
    colorPrintf(ConsoleColor(ConsoleColor::yellow), "Can't load '%s' file!\n",
                ConfigPath);
    is_error_init = true;
    return;
  }

#ifndef _WIN32
  const char *tempInput;
  tempInput = ini.GetValue("options", "input_path", NULL);
  if (tempInput == NULL) {
    colorPrintf(ConsoleColor(ConsoleColor::yellow),
                "Can't recieve path to input device");
    is_error_init = true;
    return;
  }
  InputDevice.assign(tempInput);
#else
  zero_sticks_positions[0] =
      ini.GetLongValue("stick_zero_positions", "axis_1", 0);
  zero_sticks_positions[1] =
      ini.GetLongValue("stick_zero_positions", "axis_2", 0);
  zero_sticks_positions[2] =
      ini.GetLongValue("stick_zero_positions", "axis_3", 0);
  zero_sticks_positions[3] =
      ini.GetLongValue("stick_zero_positions", "axis_4", 0);
  std::string temp_str =
      ini.GetValue("stick_zero_positions", "arrows", " 4294967295");
  zero_arrows_positions = std::stoll(temp_str);
#endif

  int axis_id = 1;
  CSimpleIniA::TNamesDepend axis_names_ini;
  ini.GetAllKeys("axis", axis_names_ini);
  for (CSimpleIniA::TNamesDepend::const_iterator i = axis_names_ini.begin();
       i != axis_names_ini.end(); ++i) {
    long temp = 0;
    temp = ini.GetLongValue("axis", i->pItem, 0);

    if (temp) {
      std::string col_axis(i->pItem);

      if (col_axis == "Exit") {
        exit_gamepad_axxis_number = temp;
      }
      gamepad_axis_bind_to_module_axis[temp] = axis_id;

      axis[axis_id] = new AxisData();
      axis[axis_id]->axis_index = axis_id;
      axis[axis_id]->name = copyStrValue(col_axis.c_str());
      axis[axis_id]->upper_value = ini.GetLongValue(i->pItem, "upper_value", 1);
      axis[axis_id]->lower_value = ini.GetLongValue(i->pItem, "lower_value", 0);

      axis_id++;
    }
  }

  COUNT_AXIS = axis.size();
  Gamepad_axis = new AxisData *[COUNT_AXIS];
  for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
    Gamepad_axis[j] = axis[j + 1];
  }
}

int GamepadControlModule::init() {
  if (is_error_init) {
    return 1;
  }

#ifdef _WIN32
  LPDIRECTINPUT8 di;
  HRESULT hr;

  // Create a DirectInput device
  if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                                     IID_IDirectInput8, (VOID **)&di, NULL))) {
    return hr;
  }

  // LPDIRECTINPUTDEVICE8W joystick;
  ZeroMemory(&joystick, sizeof(LPDIRECTINPUTDEVICE8W));
  GUID JoyGUID;

  if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, callForEnumDevices,
                                  &JoyGUID, DIEDFL_ATTACHEDONLY))) {
    colorPrintf(ConsoleColor(ConsoleColor::red), "cant Enum Devices");
    return hr;
  };
  if (FAILED(hr = di->CreateDevice(JoyGUID, &joystick, NULL))) {
    colorPrintf(ConsoleColor(ConsoleColor::red), "cant Create Device");
    return hr;
  };

  // Устанавливаем формат данных
  if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick))) {
    colorPrintf(ConsoleColor(ConsoleColor::red), "cant Set DATA");
    return hr;
  }

  //
  DIPROPRANGE dipr;
  // Сначала очищаем структуру
  ZeroMemory(&JState, sizeof(DIJOYSTATE));
  ZeroMemory(&dipr, sizeof(DIPROPRANGE));

  dipr.diph.dwSize = sizeof(dipr);
  dipr.diph.dwHeaderSize = sizeof(dipr);
  // Для оси X
  dipr.diph.dwObj = DIJOFS_X;
  dipr.diph.dwHow = DIPH_BYOFFSET;  // Смещение в формате данных
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

  dipdw.diph.dwObj = DIJOFS_X;  // Ось X
  dipdw.dwData = 1500;          // Приблизительно 15% диапазона
  // Dead zone to X
  joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
  // dead zone to Y
  dipdw.diph.dwObj = DIJOFS_Y;  // Ось Y
  joystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
#else
  joystick = new Joystick(InputDevice);
  if (!joystick->isFound()) {
    colorPrintf(ConsoleColor(ConsoleColor::yellow),
                "Open gamepad file failed.\n");
    return 1;
  }
#endif
  return 0;
}

#ifdef CONTROL_MODULE_H_000
const char *GamepadControlModule::getUID() { return IID; }
#else
const struct ModuleInfo &GamepadControlModule::getModuleInfo() { return *mi; }
#endif

AxisData **GamepadControlModule::getAxis(unsigned int *count_axis) {
  (*count_axis) = COUNT_AXIS;
  return Gamepad_axis;
}

void GamepadControlModule::destroy() {
#ifndef CONTROL_MODULE_H_000
  delete mi;
#endif
  for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
    delete Gamepad_axis[j];
  }
  delete[] Gamepad_axis;
  delete this;
}

void *GamepadControlModule::writePC(unsigned int *buffer_length) {
  *buffer_length = 0;
  return NULL;
}

int GamepadControlModule::startProgram(int uniq_index) { return 0; }

void GamepadControlModule::readPC(void *buffer, unsigned int buffer_length) {}

int GamepadControlModule::endProgram(int uniq_index) { return 0; }

void GamepadControlModule::colorPrintf(ConsoleColor colors, const char *mask,
                                       ...) {
  va_list args;
  va_start(args, mask);
  (*colorPrintf_p)(this, colors, mask, args);
  va_end(args);
}

#ifndef CONTROL_MODULE_H_000
PREFIX_FUNC_DLL unsigned short getControlModuleApiVersion() {
  return CONTROL_MODULE_API_VERSION;
};
#endif

PREFIX_FUNC_DLL ControlModule *getControlModuleObject() {
  return new GamepadControlModule();
}