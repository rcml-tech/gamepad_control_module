#ifndef Gamepad_CONTROL_MODULE_H
#define	Gamepad_CONTROL_MODULE_H


class GamepadControlModule : public ControlModule {
	AxisData **Gamepad_axis;

	std::map<std::string, system_value> axis_names;
	std::map<int, std::string> axis_bind_map;
	std::map<system_value, AxisData*> axis;

	LPDIRECTINPUTDEVICE8W joystick;
	DIJOYSTATE JState;

	colorPrintfVA_t *colorPrintf_p;

public:
	GamepadControlModule(){};

	//init
	const char *getUID();
	void prepare(colorPrintf_t *colorPrintf_p, colorPrintfVA_t *colorPrintfVA_p);

	//compiler only
	AxisData** getAxis(unsigned int *count_axis);
	void *writePC(unsigned int *buffer_length);

	//intepreter - devices
	int init();
	void execute(sendAxisState_t sendAxisState);
	void final() {};

	//intepreter - program & lib
	void readPC(void *buffer, unsigned int buffer_length);

	//intepreter - program
	int startProgram(int uniq_index);
	int endProgram(int uniq_index);

	//destructor
	void destroy();
	~GamepadControlModule() {}

	void colorPrintf(ConsoleColor colors, const char *mask, ...);
};

#endif	/* Gamepad_CONTROL_MODULE_H */