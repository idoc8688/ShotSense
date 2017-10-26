// ShotSense.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "ShotSense.h"
int countWrist = 0;


bool flag = true;
quaternion initQuat;
quaternion lastQuat;
Acceleration lastAcc;


void printQuat(const quaternion& q) {
	cout << q.X << " " << q.Y << " " << q.Z << " " << q.W << endl;
}

void connectToSensors() {
	Gem_Get(ARM_MAC_ADDR, &_handleArm);
	Gem_SetOnStateChanged(_handleArm, OnStateChangedArm);
	Gem_Connect(_handleArm);
}


void OnCombinedDataWrist(unsigned short timestamp, const float* quaternion, const float* acceleration)
{
	if (flag) {
		initQuat.set(quaternion::quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
		flag = false;
	}
	lastQuat.set(quaternion::quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
	if (countWrist++ == 100) {
		//std::cout << "data of Wrist\n";
		//std::cout << "timestamp: " << timestamp << "\n";
		//std::cout << "quaternion: " << quaternion[0] << ", " << quaternion[1] << ", " << quaternion[2] << ", " << quaternion[3] << "\n";
		//std::cout << "acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << "\n";
		countWrist = 0;
	}
}

void OnTapDataWrist(unsigned int direction)
{
	//std::cout << "OnTapData, direction: " << direction << "\n";
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

void analyseMovement() {
	cout << "analysing..." << endl;
	cout << "center quaternion: ";
	printQuat(initQuat);
	cout << endl << "current quaternion: ";
	printQuat(lastQuat);
	cout << endl << endl;
	quaternion inverseCenter(initQuat.makeInverse());
	quaternion deltaRotation(inverseCenter*lastQuat);

	printQuat(deltaRotation);
	/*cout << endl << "delta rotation in euler: ";
	vector3df euler;
	deltaRotation.toEuler(euler);
	cout << euler.X << " " << euler.Y << " " << euler.Z << endl;*/
	
}

//-0.00838301 -0.692344 -0.0724388 0.717874

int main()
{
	GemStatusCode err = Gem_Initialize();

	if (err != GEM_SUCCESS) {
		return 1;
	}
	char op;
	connectToSensors();
	while (true) {
		cin >> op;
		if (op == 'r') {
			analyseMovement();
		}
		else if (op == 'i') {
			initQuat.set(lastQuat);
			cout << "new center: ";
			printQuat(initQuat);
			cout << endl;
		}
		else {
			break;
		}
	}
	getchar(); // prevent the program from terminating       
	Gem_Terminate();
	return 0;
}

