#pragma once

#include <GemSDK.h>

#define WRIST_MAC_ADDR "00:80:98:DC:9E:09"
#define ARM_MAC_ADDR "00:80:98:DC:9E:46"

using namespace std;

GemHandle _handleWrist;
GemHandle _handleArm;

void connectToSensors();
void OnStateChangedArm(GemState state);
void OnStateChangedWrist(GemState state);

