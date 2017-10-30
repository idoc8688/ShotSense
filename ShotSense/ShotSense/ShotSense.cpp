// ShotSense.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ShotSense.h"

/*      *******************************     Static functions   *******************************      /*/

bool isExists(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

void printQuat(const quaternion& q) {
	cout << q.X << " " << q.Y << " " << q.Z << " " << q.W << endl;
}

const std::string getCurrentDate() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y_%m_%d", &tstruct);
	return buf;
}

/*      *************************     GemSense functions implementations   *************************      /*/


void connectToSensors() {
	Gem_Get(ARM_MAC_ADDR, &_handleArm);
	Gem_SetOnStateChanged(_handleArm, OnStateChangedArm);
	Gem_Connect(_handleArm);
}

void OnCombinedDataWrist(unsigned short timestamp, const float* quaternion, const float* acceleration)
{
	if (flag.first) {
		initWristQuat.set(quaternion::quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
		flag.first = false;
		return;
	}
	lastWristQuat.set(quaternion::quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
	lastWristDR.set(calcDeltaRotation(initWristQuat, lastWristQuat));
	Acceleration lastAcc(acceleration[0], acceleration[1], acceleration[2]);
	if (inMovement) {
		wristData.push_back(SensorData(lastAcc, lastWristDR, timestamp));
	}

}

void OnCombinedDataArm(unsigned short timestamp, const float* quaternion, const float* acceleration)
{
	if (flag.second) {
		initArmQuat.set(quaternion::quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
		flag.second = false;
	}

	lastArmQuat.set(quaternion::quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
	lastArmDR.set(calcDeltaRotation(initArmQuat,lastArmQuat));
	Acceleration lastAcc(acceleration[0], acceleration[1], acceleration[2]);
	if (inMovement) {
		armData.push_back(SensorData(lastAcc, lastArmDR, timestamp));
	}
}

void OnStateChangedArm(GemState state) {
	std::cout << "gem state Arm: ";
	switch (state)
	{
	case GemState::Connected:
		std::cout << "Connected ";
		Gem_SetOnCombinedData(_handleArm, OnCombinedDataArm);
		Gem_EnableCombinedData(_handleArm);

		//connect to wrist sensor
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

/*      *******************************     Functions implementations   *******************************      /*/


quaternion calcDeltaRotation(const quaternion& q0, const quaternion& q1) {
	quaternion inverseCenter(q0);
	inverseCenter.makeInverse();
	quaternion deltaRotation(inverseCenter*q1);
	return deltaRotation;

}

void analyseMovement() {
	cout << "analysing..." << endl;
	cout << "center quaternion: ";
	printQuat(initArmQuat);
	printQuat(initWristQuat);
	cout << endl << "current quaternion: ";
	printQuat(lastArmQuat);
	printQuat(lastWristQuat);
	cout << endl << endl;
	quaternion inverseArmCenter(initArmQuat.makeInverse());
	quaternion deltaArmRotation(inverseArmCenter*lastArmQuat);

	printQuat(deltaArmRotation);
	
}

bool processInput(char op) {
	if (op == 'i') {
		initArmQuat.set(lastArmQuat);
		initWristQuat.set(lastWristQuat);
		cout << "new center arm: ";
		printQuat(initArmQuat);
		cout << "new center wrist: ";
		printQuat(initWristQuat);
	} else if (op == 'r') {
		 analyseMovement();
	} else if (op == 's') {
		inMovement = true;
		armData.clear();
		wristData.clear();
	} else if (op == 'e') {
		inMovement = false;
		if (armData.size() > wristData.size()) {
			armData.resize(wristData.size());
		}
		else if (armData.size() < wristData.size()) {
			wristData.resize(armData.size());
		}
		cout << "Hit or Miss? (enter 'h' or 'm')" << endl;
		char missOrHit;
		cin >> missOrHit;
		processData((missOrHit == 'm'));
	}
	else {
		return true;
	}
	return false;
}

//first set: m h h m m m h h h m m m
//second set: h h h m h h m m m m m 
void processData(bool isMiss) {
	ofstream f;
	std::string dirName = getCurrentDate();
	std::string name = getAvailableFilename(isMiss,dirName);
	f.open(dirName + "\\" + name);
	f << "arm, , , , , , , , , , , , wrist" << endl;
	f << "a.x,a.y,a.z,G, , q.x,q.y,q.z,q.w, ,time, ," << "a.x,a.y,a.z,G, , q.x,q.y,q.z,q.w, ,time," << endl;

	unsigned int startOffset = findStartOfMoveIndex();
	unsigned int endOffset = findEndOfMoveIndex();
	for (unsigned int i = startOffset; i <= endOffset; i++) {
		f << armData[i] << ", ," << wristData[i] << endl;
	}
	f.close();
	cout << "Saved last shot as " << name << endl;
}

int findEndOfMoveIndex() {
	vector<float> avgGravity;
	float tmp;
	int index = 0;
	for (unsigned int i = 0; i < armData.size(); i++) {
		tmp = (armData[i].a.getGravity() + wristData[i].a.getGravity());
		avgGravity.push_back(tmp / 2);
	}
	// find last peak to find end of movement from there
	unsigned int lastPeakIndex = 0;
	for (unsigned int i = avgGravity.size() - 1; i > 0; i--) {
		if (avgGravity[i] > AVG_GRAVITY_LAST_PEAK) {
			lastPeakIndex = i;
			break;
		}
	}

	for (unsigned int i = lastPeakIndex; i < armData.size() - END_MOVEMENT_SAMPLES_FORWARD; i++) {
		bool isCloseToFollowingSamples = true;
		float tmpAvg = avgGravity[i];
		if (tmpAvg < AVG_GRAVITY_UPPER_THRESHOLD_END_MOVE && tmpAvg > AVG_GRAVITY_LOWER_THRESHOLD_END_MOVE) {
			for (unsigned int offset = 1; offset < END_MOVEMENT_SAMPLES_FORWARD; offset++) {
				if (abs(tmpAvg-avgGravity[i+offset]) > AVG_GRAVITY_DIFF_END_MOVE) 
					isCloseToFollowingSamples = false;
			}
			if (isCloseToFollowingSamples) return i;
		}
	}
	return armData.size() - 1;
}

std::string getAvailableFilename(bool isMiss, std::string dirName) {
	std::string filename(SHOT_DATA_FILENAME);
	filename.append( ((isMiss) ? "miss_" : "hit_"));
	std::string tmp(filename);
	filename += std::to_string(findLowestIndexForNextFile(filename, dirName));
	filename.append(SHOT_DATA_EXTENSION);
	return filename;
}

int findLowestIndexForNextFile(std::string filename, std::string relativePath) {
	unsigned int i = 0;
	for (i = 0; ; i++) {
		if (!isExists(relativePath + "\\" + filename + std::to_string((int)i) + SHOT_DATA_EXTENSION)) break;
	}
	return i;
}

int findStartOfMoveIndex() {
	vector<float> avgGravity;
	float tmp;
	int index = 0;
	for (unsigned int i = 0; i < armData.size(); i++) {
		tmp = (armData[i].a.getGravity() + wristData[i].a.getGravity());
		avgGravity.push_back(tmp/2);
	}
	for (unsigned int i = 0; i < avgGravity.size(); i++) {
		if (avgGravity[i] > AVG_GRAVITY_THRESHOLD_START_MOVE)
			return std::max(0, (int)(i - START_MOVEMENT_SAMPLES_BACK));

	}
	return index;
}
#include <experimental/filesystem>

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
		if (processInput(op)) break;
	}
	getchar(); // prevent the program from terminating       
	Gem_Terminate();
	return 0;
}