/**
* CopyRight (C) 2012, David
* All rights reserved
*
* @file sHidKey.h
* @date 2018-07-22
* @author David.li
* @version V1.0
* @modify None
*/

#ifndef __SHIDKEY_H__
#define __SHIDKEY_H__

//=============================================================================
// Include File
//=============================================================================
#include <Wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Constant
//=============================================================================
#define HIDKEY_KEY_LEN		16

typedef enum
{
	eHidKey_DataType_UserKey = 0,	// 用户的Key
	eHidKey_DataType_DateCfg,	// 使用期限
	eHidKey_DataType_CurDate,	// 当前日期
	eHidKey_DataType_Pass,		// 密码

	eHidKey_DataType_Num
}E_HidKey_DataType;

//=============================================================================
// Structure
//=============================================================================
// HidKey参数
typedef struct _HidKey_DeviceData
{
	USHORT    m_u16VID;  // Key的UID
	USHORT    m_u16PID;  // Key的PID
	USHORT    m_u16BCDVer;  // Key的版本号
	USHORT    m_u16DeviceType;  // Key的芯片类型
	USHORT    m_u16Rev1;
	USHORT    m_u16Rev2;
	USHORT    m_u16Rev3;
	USHORT    m_u16UnitID; // 扩展属性的ID号

	UCHAR    m_u8DevicePort[32]; // Key端口号-字符型
	UCHAR    m_u8DeviceName[512]; // Key名称-字符型
	UCHAR    m_u8DevicePath[512]; // Key路径-字符型
	UCHAR    m_u8SerialNumber[512];  // Key序列号

	PVOID     m_pDeviceHand; // 设备打开的句柄
	PVOID     m_pRev1;
}T_HidKey_DeviceData, *T_pHidKey_DeviceData;

//=============================================================================
// Variable
//=============================================================================

//=============================================================================
// Function Declear
//=============================================================================
// 获取HIDKey列表
typedef USHORT (*T_fsHidKey_GetDeviceList) (T_pHidKey_DeviceData psHidKeyDeviceData, USHORT u16BufNum);
// 打开HIDKey
typedef BOOL (*T_fsHidKey_OpenDevice) (T_pHidKey_DeviceData psHidKeyDeviceData);
// 关闭HIDKey
typedef BOOL (*T_fsHidKey_CloseDevice) (T_pHidKey_DeviceData psHidKeyDeviceData);
// 写数据到USBKey
typedef BOOL (*T_fsHidKey_SetData) (T_pHidKey_DeviceData psHidKeyDeviceData, USHORT u16DataType, PUCHAR pu8Data, USHORT u16DataLen);
// 从USBKey获取数据
typedef USHORT (*T_fsHidKey_GetData) (T_pHidKey_DeviceData psHidKeyDeviceData, USHORT u16DataType, PUCHAR pu8DataBuf, USHORT u16BufLen);
// AES加密
typedef BOOL (*T_fsHidKey_Aes) (PUCHAR pu8Data, USHORT u16DataLen, PUCHAR pu8Key, USHORT u16KeyLen);
// AES解密
typedef BOOL (*T_fsHidKey_DeAes) (PUCHAR pu8Data, USHORT u16DataLen, PUCHAR pu8Key, USHORT u16KeyLen);

//=============================================================================
// Function
//=============================================================================

#if defined(DLL_EXPORTS)
#define _DLLExport __declspec(dllexport)
#else 

#if defined(_LIB)
#define _DLLExport
#else
#define _DLLExport __declspec(dllimport) 
#endif

#endif

// 获取HIDKey列表
//*========================================
// @Function sHidKey_GetDeviceList
// @Brief Get the HID key device list
// @Param T_pHidKey_DeviceData psHidKeyDeviceData [IN] The HID key device list buffer
// @Param USHORT u16BufNum [IN] The HID key device list buffer number
// @Return USHORT The exist HID device number
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
USHORT _DLLExport sHidKey_GetDeviceList (T_pHidKey_DeviceData psHidKeyDeviceData,
							  USHORT u16BufNum);
// 打开HIDKey
//*========================================
// @Function sHidKey_OpenDevice
// @Brief Open the HID key device
// @Param T_pHidKey_DeviceData psHidKeyDeviceData [IN] The HID key device list buffer
// @Return BOOL True mean open HID key device success
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
BOOL _DLLExport sHidKey_OpenDevice (T_pHidKey_DeviceData psHidKeyDeviceData);

// 关闭HIDKey
//*========================================
// @Function sHidKey_CloseDevice
// @Brief Close the HID key device
// @Param T_pHidKey_DeviceData psHidKeyDeviceData [IN] The HID key device list buffer
// @Return BOOL True mean close HID key device success
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
BOOL _DLLExport sHidKey_CloseDevice (T_pHidKey_DeviceData psHidKeyDeviceData);

// 写数据到USBKey
//*========================================
// @Function sHidKey_SetData
// @Brief Write data to hid key
// @Param T_pHidKey_DeviceData psHidKeyDeviceData [IN] The HID key device list buffer
// @Param USHORT u16DataType [IN] The data type
// @Param PUCHAR pu8Data [IN] The data
// @Param USHORT u16DataLen [IN] The data length
// @Return BOOL True mean operation success
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
BOOL _DLLExport sHidKey_SetData (T_pHidKey_DeviceData psHidKeyDeviceData,
					  USHORT u16DataType,
					  PUCHAR pu8Data,
					  USHORT u16DataLen);

// 从USBKey获取数据
//*========================================
// @Function sHidKey_SetData
// @Brief Write data to hid key
// @Param T_pHidKey_DeviceData psHidKeyDeviceData [IN] The HID key device list buffer
// @Param USHORT u16DataType [IN] The data type
// @Param PUCHAR pu8DataBuf [IN] The data buffer
// @Param USHORT u16BufLen [IN] The data buffer length
// @Return BOOL True mean operation success
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
USHORT _DLLExport sHidKey_GetData (T_pHidKey_DeviceData psHidKeyDeviceData,
					  USHORT u16DataType,
					  PUCHAR pu8DataBuf,
					  USHORT u16BufLen);

//*========================================
// @Function sHidKey_Aes
// @Brief AES encode string
// @Param PUCHAR pu8Data [INOUT] The data to encode
// @Param USHORT u16DataLen [IN] The data length, must divide by 16, and max is 64
// @Param PUCHAR pu8Key [IN] The key
// @Param USHORT u16KeyLen [IN] The key length, must 16
// @Return BOOL True mean operation success
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
BOOL _DLLExport sHidKey_Aes (PUCHAR pu8Data,
				  USHORT u16DataLen,
				  PUCHAR pu8Key,
				  USHORT u16KeyLen);

//*========================================
// @Function sHidKey_DeAes
// @Brief AES decode string
// @Param PUCHAR pu8Data [INOUT] The data to decode
// @Param USHORT u16DataLen [IN] The data length, must divide by 16, and max is 64
// @Param PUCHAR pu8Key [IN] The key
// @Param USHORT u16KeyLen [IN] The key length, must 16
// @Return BOOL True mean operation success
//
// @Author David.li
// @Date 2018-7-22
// @Version V0.1
// @Modify None
//*========================================
BOOL _DLLExport sHidKey_DeAes (PUCHAR pu8Data,
					USHORT u16DataLen,
					PUCHAR pu8Key,
					USHORT u16KeyLen);

#ifdef __cplusplus
};
#endif

#endif
