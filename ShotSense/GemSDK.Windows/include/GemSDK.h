#pragma once

#ifdef _WIN32
	#ifdef gemsdk_EXPORTS
		#define GEMSDK_API __declspec(dllexport)
	#else
		#define GEMSDK_API __declspec(dllimport)
	#endif
#else
	#define GEMSDK_API
#endif

#include "Common.h"

#define GEM_FAILED					0
#define GEM_SUCCESS					1
#define GEM_INVALID_HANDLE			2
#define GEM_CONNECTION_IN_PROGRESS	3
#define GEM_ALREADY_EXISTS			4
#define GEM_NOT_INITIALIZED			5


extern "C"
{
	typedef int GemStatusCode;
	enum GemState;

	/// <summary>
	/// Gem discovered callback
	/// </summary>
	/// <param name="address">Address of the Gem in the following format: XX:XX:XX:XX:XX:XX</param>
	/// <param name="address_type">The new state</param>
	typedef void(*onDeviceDiscovered_t)(const char* address, int address_type);

	/// <summary>
	/// Gem state changed callback.
	/// </summary>
	/// <param name="state">The new state</param>
	typedef void(*onStateChanged_t)(GemState state);

	/// <summary>
	/// IMU data callback
	/// </summary>
	/// <param name="timestamp">An internal counter representing the difference between samples in milliseconds</param>
	/// <param name="quaternion">Quaternion data, float array of 4 elements (x, y, z, w)</param>
	/// <param name="acceleration">Acceleration data, float array of 3 elements (x, y, z)</param>
	typedef void(*onCombinedData_t)(unsigned short timestamp, const float* quaternion, const float* acceleration);

	/// <summary>
	/// IMU raw data callback
	/// </summary>
	/// <param name="acceleration">Acceleration data, float array of 3 elements (x, y, z)</param>
	/// <param name="gyroscope">Gyroscope data, float array of 3 elements (x, y, z)</param>
	/// <param name="magnetometer">Magnetometer data, float array of 3 elements (x, y, z)</param>
	typedef void(*onRawData_t)(const float* acceleration, const float* gyroscope, const float* magnetometer);

	/// <summary>
	/// Tap callback
	/// </summary>
	/// <param name="direction">Direction of tap</param>
	typedef void(*onTapData_t)(unsigned int direction);

	/// <summary>
	/// Pedometer data callback
	/// </summary>
	/// <param name="steps">Step count</param>
	/// <param name="walk_time">Time walking (in seconds)</param>
	typedef void(*onPedometerData_t)(unsigned int steps, float walk_time);

	/// <summary>
	/// Digital read callback
	/// </summary>
	/// <param name="id">Pin id</param>
	/// <param name="value">Pin read value</param>
	typedef void(*onIoDigitalRead_t)(int id, IoValues value);

	/// <summary>
	/// Analog read callback
	/// </summary>
	/// <param name="id">Pin id</param>
	/// <param name="value">Pin read value</param>
	typedef void(*onIoAnalogRead_t)(int id, int value);

	/// <summary>
	/// I2C data callback
	/// </summary>
	/// <param name="address">I2C device address</param>
	/// <param name="reg">Register address (if you didn't use register in the request this value will be 0)</param>
	/// <param name="data">The data read from the I2C device</param>
	/// <param name="data_length">Length of the buffer (0-16 bytes).</param>
	typedef void(*onI2CData_t)(unsigned char address, unsigned char reg, const unsigned char* data, unsigned char data_length);

	/// <summary>
	/// SPI data callback
	/// </summary>
	/// <param name="address">Request id of the SPI call</param>
	/// <param name="data">The data read from the SPI device</param>
	/// <param name="data_length">Length of the buffer (0-16 bytes).</param>
	typedef void(*onSPIData_t)(unsigned char request_id, const unsigned char* data, unsigned char data_length);

	/// <summary>
	/// Initializes the GemSDK, call this before anything else!
	/// </summary>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_Initialize();

	/// <summary>
	/// Cleanup the GemSDK and releases related resources, it is very important to call this when you are finished with the SDK. Otherwise, the Bluetooth Controller might
	/// keep the connection with your Gem active, preventing other devices from connecting to it.
	/// </summary>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_Terminate();

	/// <summary>
	/// Returns an handle for a given Gem address. If a Gem with the given address already exists, it will return the handle to it, and will return a status code
	/// of GEM_ALREADY_EXISTS.
	/// </summary>
	/// <param name="address">The address of the desired Gem, as a string (char array) with the following format: 12:34:56:78:9A:BC</param>
	/// <param name="handle">A pointer to a GemHandle to store the result handle in</param>
	/// <returns>GEM_SUCCESS on success, GEM_ALREADY_EXISTS if a gem with the same address already exists, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_Get(const char* address, GemHandle* handle);

	/// <summary>
	///	Initiates a connection operation to a Gem.
	/// If this function succeeds (returns GEM_SUCCESS) you will be notified when it ends with the callback set with <see cref="Gem_SetOnStateChanged"/>.
	/// Only one connection operation can be active at the same time, if you want to connect more than one Gem, you need to wait for this operation to finish or stop it
	/// manually using <see cref="GemDisconnect"/> with the matching handle.
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, GEM_CONNECTION_IN_PROGRESS if another connection operation is in progress, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_Connect(GemHandle handle);

	/// <summary>
	///	Disconnects a Gem.
	/// If there is a connection in progress to the Gem it will abort it.
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_Disconnect(GemHandle handle);

	/// <summary>
	///	Sets a callback for OnDeviceDiscovered event.	
	/// </summary>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnDeviceDiscovered(onDeviceDiscovered_t func);

	/// <summary>
	///	Sets a callback for OnStateChanged event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnStateChanged(GemHandle handle, onStateChanged_t func);

	/// <summary>
	///	Sets a callback for OnCombinedData event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnCombinedData(GemHandle handle, onCombinedData_t func);

	/// <summary>
	///	Sets a callback for OnRawData event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnRawData(GemHandle handle, onRawData_t func);

	/// <summary>
	///	Sets a callback for OnTapData event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnTapData(GemHandle handle, onTapData_t func);

	/// <summary>
	///	Sets a callback for OnPedometerData event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnPedometerData(GemHandle handle, onPedometerData_t func);

	/// <summary>
	///	Sets a callback for OnIoDigitalRead event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnIoDigitalRead(GemHandle handle, onIoDigitalRead_t func);

	/// <summary>
	///	Sets a callback for OnIoAnalogRead event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnIoAnalogRead(GemHandle handle, onIoAnalogRead_t func);

	/// <summary>
	///	Sets a callback for OnI2CData event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnI2CData(GemHandle handle, onI2CData_t func);

	/// <summary>
	///	Sets a callback for OnSPIData event.	
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="func">A function pointer to be called.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_SetOnSPIData(GemHandle handle, onSPIData_t func);

	/// <summary>
	/// Start scanning, when a Gem found an OnDeviceDiscovered callback will be called with its address and address_type
	/// </summary>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_StartScan();

	/// <summary>
	/// Stops scanning
	/// </summary>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_StopScan();

	/// <summary>
	///	Enable CombinedData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_EnableCombinedData(GemHandle handle);

	/// <summary>
	///	Disables CombinedData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_DisableCombinedData(GemHandle handle);

	/// <summary>
	///	Enable RawData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_EnableRawData(GemHandle handle);

	/// <summary>
	///	Disables RawData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_DisableRawData(GemHandle handle);

	/// <summary>
	///	Enable TapData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_EnableTapData(GemHandle handle);

	/// <summary>
	///	Disables TapData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_DisableTapData(GemHandle handle);

	/// <summary>
	///	Enable PedometerData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_EnablePedometerData(GemHandle handle);

	/// <summary>
	///	Disables PedometerData events
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_DisablePedometerData(GemHandle handle);

	/// <summary>
	///	Configurates digital I/O
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="id">Id of the I/O to write.</param>
	/// <param name="value">Desired type for the I/O</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ConfigureDigital(GemHandle handle, int id, PinIoConfiguration value);

	/// <summary>
	///	Writes to digital I/O
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="id">Id of the I/O to write.</param>
	/// <param name="value">pin state, high or low</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_WriteDigital(GemHandle handle, int id, IoValues value);

	/// <summary>
	///	Requests I/O read operation. Result will be returned via the callback set with <see cref="Gem_SetOnIoDigitalRead"/>
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="id">Id of the I/O to read.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ReadDigital(GemHandle handle, int id);

	/// <summary>
	///	Requests I/O read operation. Result will be returned via the callback set with <see cref="Gem_SetOnIoAnalogRead"/>
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="id">Id of the I/O to read.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ReadAnalog(GemHandle handle, int id);

	/// <summary>
	///	Writes to PWM output pin
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="id">Id of the I/O to read.</param>
	/// <param name="value">Voltage to write in volts 0.0f to 3.6f.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_WritePWM(GemHandle handle, float value);

	/// <summary>
	///	Disables PWM
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_DisablePWM(GemHandle handle);

	/// <summary>
	///	Writes data to I2C bus
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="address">I2C address of the remote device.</param>
	/// <param name="data">Data to write. Supported up to 16 bytes</param>
	/// <param name="data_length">Data length, (0-16)</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_WriteI2CData(GemHandle handle, unsigned char address, unsigned char* data, unsigned char data_length);

	/// <summary>
	/// Writes data to I2C bus to certain register
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="address">I2C address of the remote device.</param>
	/// <param name="reg">Data register to write</param>
	/// <param name="data">Data to write. Supported up to 16 bytes</param>
	/// <param name="data_length">Data length, (0-16)</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_WriteI2CDataReg(GemHandle handle, unsigned char address, unsigned char reg, unsigned char* data, unsigned char data_length);

	/// <summary>
	/// Requests data reading from I2C bus. Result will be returned via the callback set with <see cref="Gem_SetOnI2CData"/>
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="address">I2C address of the remote device.</param>
	/// <param name="count">Bytes count to read. Supported up to 16 bytes per request.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ReadI2CData(GemHandle handle, unsigned char address, unsigned char count);

	/// <summary>
	/// Requests data reading from I2C bus for certain register. Result will be returned via the callback set with <see cref="Gem_SetOnI2CData"/>
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="address">I2C address of the remote device.</param>
	/// <param name="reg">Data register to read</param>
	/// <param name="count">Bytes count to read. Supported up to 16 bytes per request.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ReadI2CDataReg(GemHandle handle, unsigned char address, unsigned char reg, unsigned char count);

	/// <summary>
	/// Writes data to SPI bus
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="data">Data to write. Supported up to 16 bytes</param>
	/// <param name="data_length">Data length (0-16)</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_WriteSPI(GemHandle handle, unsigned char* data, unsigned char data_length);

	/// <summary>
	/// Writes data to SPI bus. Result will be returned via the callback set with <see cref="Gem_SetOnSPIData"/> with the requestId set here
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="requestId">This parameter will be passed to the callback so you'll be able to match the response to the request</param>
	/// <param name="count">Bytes count to read. Supported up to 16 bytes per request.</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ReadSPI(GemHandle handle, unsigned char requestId, unsigned char count);

	/// <summary>
	/// Writes to SPI bus with following reading simultaneously. Result will be returned via the callback set with <see cref="Gem_SetOnSPIData"/> with the requestId set here
	/// </summary>
	/// <param name="handle">The handle of the Gem.</param>
	/// <param name="requestId">This parameter will be passed to the callback so you'll be able to match the response to the request</param>
	/// <param name="data">Data to write. Supported up to 16 bytes</param>
	/// <param name="data_length">Data length (0-16), this is will also be the amount of data read back</param>
	/// <returns>GEM_SUCCESS on success, error code otherwise</returns>
	GEMSDK_API GemStatusCode Gem_ReadWriteSPI(GemHandle handle, unsigned char requestId, unsigned char* data, unsigned char data_length);
};