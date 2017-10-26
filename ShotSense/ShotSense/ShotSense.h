#pragma once

#include <GemSDK.h>
#include <irrlicht.h>
#include <vector>

#define WRIST_MAC_ADDR "00:80:98:DC:9E:46"
#define ARM_MAC_ADDR "00:80:98:DC:9E:09"

using namespace irr::core;
using namespace std;

class Acceleration {
	float x, y, z;
public:
	Acceleration(float x, float y, float z) : x(x), y(y), z(z) {};
	Acceleration() : x(0), y(0), z(0) {};
};

GemHandle _handleWrist;
GemHandle _handleArm;

void printQuat(const quaternion& q);

void connectToSensors();
void OnStateChangedArm(GemState state);
void OnStateChangedWrist(GemState state);

void analyseMovement();
