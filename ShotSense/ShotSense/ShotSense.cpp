// ShotSense.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "ShotSense.h"

int countWrist = 0;

void connectToSensors() {
	Gem_Get(ARM_MAC_ADDR, &_handleArm);
	Gem_SetOnStateChanged(_handleArm, OnStateChangedArm);
	Gem_Connect(_handleArm);
}


void OnCombinedDataWrist(unsigned short timestamp, const float* quaternion, const float* acceleration)
{
	if (countWrist++ == 100) {
		std::cout << "data of Wrist\n";
		std::cout << "timestamp: " << timestamp << "\n";
		std::cout << "quaternion: " << quaternion[0] << ", " << quaternion[1] << ", " << quaternion[2] << ", " << quaternion[3] << "\n";
		std::cout << "acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << "\n";
		countWrist = 0;
	}
}

void OnTapDataWrist(unsigned int direction)
{
	std::cout << "OnTapData, direction: " << direction << "\n";
}



void OnStateChangedArm(GemState state) {
	std::cout << "gem state Arm: ";

	switch (state)
	{
	case GemState::Connected:
		std::cout << "Connected ";
		Gem_Get(WRIST_MAC_ADDR, &_handleWrist);
		Gem_SetOnStateChanged(_handleWrist, OnStateChangedWrist);
		Gem_Connect(_handleWrist);
		break;
	case GemState::Connecting:
		std::cout << "Connecting ";
		break;
	case GemState::Disconnected:
		std::cout << "Disconnected ";
		break;
	case GemState::Disconnecting:
		std::cout << "Disconnecting ";
		break;
	}

	std::cout << endl;
}


void OnStateChangedWrist(GemState state) {
	std::cout << "gem state Wrist: ";
	switch (state) {
	case GemState::Connected:
		std::cout << "Connected ";
		Gem_SetOnCombinedData(_handleWrist, OnCombinedDataWrist);
		Gem_EnableCombinedData(_handleWrist);
		Gem_SetOnTapData(_handleWrist, OnTapDataWrist);
		Gem_EnableTapData(_handleWrist);
		break;
	case GemState::Connecting:
		std::cout << "Connecting ";
		break;
	case GemState::Disconnected:
		std::cout << "Disconnected ";
		break;
	case GemState::Disconnecting:
		std::cout << "Disconnecting ";
		break;
	}

	std::cout << endl;
}

int main()
{
	GemStatusCode err = Gem_Initialize();

	if (err != GEM_SUCCESS) {
		return 1;
	}
	connectToSensors();

	getchar(); // prevent the program from terminating       
	Gem_Terminate();
	return 0;
}

