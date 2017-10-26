#pragma once

#include <GemSDK.h>
#include <irrlicht.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <iostream>

#define WRIST_MAC_ADDR "00:80:98:DC:9E:46"
#define ARM_MAC_ADDR "00:80:98:DC:9E:09"


using namespace irr::core;
using namespace std;


GemHandle _handleWrist;
GemHandle _handleArm;

class Acceleration {
	float x, y, z;
public:
	Acceleration(float x, float y, float z) : x(x), y(y), z(z) {};
	Acceleration() : x(0), y(0), z(0) {};
	float getGravity() const {
		return sqrt(x*x + y*y + z*z);
	}
	friend std::ostream& operator<<(std::ostream& stream, const Acceleration& a) {
		stream << a.x << "," << a.y << "," << a.z;
		return stream;
	}
	
};

class SensorData {
public:
	Acceleration a;
	quaternion q;
	unsigned short timestamp;
	SensorData(Acceleration a, quaternion q, unsigned short timestamp) : a(a), q(q), timestamp(timestamp) {};
	SensorData() : a(Acceleration()), q(quaternion()), timestamp(0) {};
	//todo: fix with friend method

	friend std::ostream& operator<<(std::ostream& stream, const SensorData& s) {
		stream << s.a << "," << s.a.getGravity() << ", ," << s.q.X << "," << s.q.Y << "," << s.q.Z << "," << s.q.W << ", ,"<< s.timestamp;
		return stream;
	}
};

void printQuat(const quaternion& q);

void connectToSensors();
void OnStateChangedArm(GemState state);
void OnStateChangedWrist(GemState state);

bool processInput(char op);
quaternion calcDeltaRotation(const quaternion& q0, const quaternion& q1);

void analyseMovement();
void processData();
