#pragma once

typedef int GemHandle;

enum GemState
{
	Connected,
	Connecting,
	Disconnected,
	Disconnecting
};

enum PinIoConfiguration
{
	Input = 0x00,
	InputNotify = 0x01,
	Output = 0x02,
	Reset = 0x03,
	InputPullup = Input | 0x10,
	InputPulldown = Input | 0x20,
	InputNotifyPullup = InputNotify | 0x10,
	InputtNotifyPulldown = InputNotify | 0x20
};

enum IoValues
{
	Low = 0x00,
	High = 0x01
};