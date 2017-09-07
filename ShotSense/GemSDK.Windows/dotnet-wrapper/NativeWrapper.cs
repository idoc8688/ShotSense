using System;
using System.Runtime.InteropServices;

namespace GemSDK
{
    public enum GemState
    {
        Connected,
        Connecting,
        Disconnected,
        Disconnecting
    }

    public enum GemStatusCode : int
    {
        Failed = 0,
        Success = 1,
        InvalidHandle = 2,
        ConnectionInProgress = 3,
        AlreadyExists = 4
    }

    public enum IoValues : int
    {
        Low = 0,
        High = 1
    }

    public enum PinIoConfiguration : int
    {
        Input = 0x00,
        InputNotify = 0x01,
        Output = 0x02,
        Reset = 0x03,
        InputPullup = Input | 0x10,
        InputPulldown = Input | 0x20,
        InputNotifyPullup = InputNotify | 0x10,
        InputtNotifyPulldown = InputNotify | 0x20
    }

    internal static class NativeWrapper
	{
        private const string DLLName = "gemsdk";
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onStateChanged_t(GemState state);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onCombinedData_t(int timestamp, [MarshalAs(UnmanagedType.LPArray, SizeConst = 4)] float[] quaternion, [MarshalAs(UnmanagedType.LPArray, SizeConst = 3)] float[] acceleration);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onTapData_t(int direction);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onPedometerData_t(int steps_count, float walk_time);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onIoDigitalRead_t(int pin, IoValues value);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onIoAnalogRead_t(int pin, int value);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onI2CData_t(byte address, byte reg, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] byte[] data, byte length);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void onSPIData_t(byte requestId, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data, byte length);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_Initialize();

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_Terminate();

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_Get(string address, out int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_Connect(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_Disconnect(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnStateChanged(int handle, onStateChanged_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnCombinedData(int handle, onCombinedData_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnTapData(int handle, onTapData_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnPedometerData(int handle, onPedometerData_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnIoDigitalRead(int handle, onIoDigitalRead_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnIoAnalogRead(int handle, onIoAnalogRead_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnI2CData(int handle, onI2CData_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_SetOnSPIData(int handle, onSPIData_t func);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_EnableCombinedData(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_DisableCombinedData(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_EnableTapData(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_DisableTapData(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_EnablePedometerData(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_DisablePedometerData(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ConfigureDigital(int handle, int id, PinIoConfiguration value);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_WriteDigital(int handle, int id, IoValues value);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ReadDigital(int handle, int id);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ReadAnalog(int handle, int id);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_WritePWM(int handle, float value);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_DisablePWM(int handle);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_WriteI2CData(int handle, byte address, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] byte[] data, byte data_length);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_WriteI2CDataReg(int handle, byte address, byte reg, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)] byte[] data, byte data_length);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ReadI2CData(int handle, byte address, byte count);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ReadI2CDataReg(int handle, byte address, byte reg, byte count);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_WriteSPI(int handle, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] byte[] data, byte data_length);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ReadSPI(int handle, byte requestId, byte count);

        [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
        public static extern GemStatusCode Gem_ReadWriteSPI(int handle, byte requestId, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] byte[] data, byte data_length);
    }
}