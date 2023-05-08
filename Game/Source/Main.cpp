#include "App.h"
#include "Window.h"

#include "Defs.h"
#include "Log.h"
#include <chrono>
#include <string>

// NOTE: SDL redefines main function
#include "SDL/include/SDL.h"

// NOTE: Library linkage is configured in Linker Options
//#pragma comment(lib, "../Game/Source/External/SDL/libx86/SDL2.lib")
//#pragma comment(lib, "../Game/Source/External/SDL/libx86/SDL2main.lib")

#include <stdlib.h>

enum class MainState
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

std::unique_ptr<App> app = nullptr;

int main(int argc, char* args[])
{
	using enum MainState;
	LOG("Engine starting ...");

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while(state != EXIT)
	{
		std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();

		switch(state)
		{
			// Allocate the engine --------------------------------------------
			case CREATE:
				LOG("CREATION PHASE ===============================");

				app = std::make_unique<App>(argc, args);

				state = app ? AWAKE : FAIL;
				break;

			// Awake all modules -----------------------------------------------
			case AWAKE:
				LOG("AWAKE PHASE ===============================");
				state = app->Awake() ? START : FAIL;

				break;

			// Call all modules before first frame  ----------------------------
			case START:
				LOG("START PHASE ===============================");
				state = app->Start() ? LOOP : FAIL;

				LOG("UPDATE PHASE ===============================");
				break;

			// Loop all modules until we are asked to leave ---------------------
			case LOOP:
				if(!app->Update()) state = CLEAN;
				break;

			// Cleanup allocated memory -----------------------------------------
			case CLEAN:
				LOG("CLEANUP PHASE ===============================");
				if(app->CleanUp() == true)
				{
					result = EXIT_SUCCESS;
					state = EXIT;
				}
				else state = FAIL;

				break;

			// Exit with errors and shame ---------------------------------------
			case FAIL:
				LOG("Exiting with errors :(");
				result = EXIT_FAILURE;
				state = EXIT;
				break;
			
			// Just so compiler stops screaming there's a case missing ----------
			case EXIT:
				break;
		}

		//Time per cycles
		std::chrono::high_resolution_clock::time_point endCycle = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds elapsedCycle = std::chrono::duration_cast<std::chrono::microseconds>(endCycle - timeStart);

		//Time per frame in microseconds acording to taget FPS
		int microSecCheck = (int)((1.0f / 60) * 1E6); //target is 60

		// This is to cap FPS, the diplaying of FPS on screen is calculated underneath
		if (elapsedCycle < std::chrono::microseconds(microSecCheck))
		{
			std::this_thread::sleep_for(std::chrono::microseconds(std::chrono::microseconds(microSecCheck) - elapsedCycle));
		}

		//Time per cycle + delay
		std::chrono::high_resolution_clock::time_point endFrame = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds elapsedFrame = duration_cast<std::chrono::microseconds>(endFrame - timeStart);

		//Calculate FPSs and dt
		app->dt = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedFrame).count();
		float FPS = 1 / ((float)elapsedFrame.count() * 10E-7);

		std::string s = std::format("{}", FPS);
		app->win->SetTitle("Return -A JRPG Parody- | FPS: " + s);
	}

	LOG("... Bye! :)\n");

	return result;
}