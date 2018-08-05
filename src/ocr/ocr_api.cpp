#include "ocr_api.h"
#include "baseapi.h"
#include "allheaders.h"

#include "tess_version.h"
#include "apitypes.h"
#include "pageiterator.h"
#include "platform.h"
#include "publictypes.h"
#include "resultiterator.h"
#include "serialis.h"
#include "tesscallback.h"
#include "thresholder.h"
#include "unichar.h"

#include "sHidKey.h"

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <tchar.h>

#define HIDKEY_NUM_MAX 16

using namespace std;

static tesseract::TessBaseAPI *tapi = new tesseract::TessBaseAPI();


// HID Key
USHORT				m_u16HidKeyNum;
T_HidKey_DeviceData	m_oHidKeyList[HIDKEY_NUM_MAX];
T_HidKey_DeviceData m_oFocusHidKey;

UCHAR				m_oStrUserKey[64] = "0987654321123456";
UCHAR				m_oStrUserPass[64] = "0987654321123456";
UCHAR				m_oStrDateCfg[64];
UCHAR				m_oStrCurDate[64];


// HID key interface
// 获取HIDKey列表
T_fsHidKey_GetDeviceList	m_fsHidKey_GetDeviceList;
// 打开HIDKey
T_fsHidKey_OpenDevice		m_fsHidKey_OpenDevice;
// 关闭HIDKey
T_fsHidKey_CloseDevice		m_fsHidKey_CloseDevice;
// 写数据到USBKey
T_fsHidKey_SetData			m_fsHidKey_SetData;
// 从USBKey获取数据
T_fsHidKey_GetData			m_fsHidKey_GetData;
// AES加密
T_fsHidKey_Aes				m_fsHidKey_Aes;;
// AES解密
T_fsHidKey_DeAes			m_fsHidKey_DeAes;
HINSTANCE					m_hsHidKeyModule;
BOOL						m_bsHidKeyInterfaceFlag;


BOOL sHidKeyInterfaceInit(void);
BOOL sHidKeyInterfaceFree(void);

BOOL sHidKeyInterfaceInit(void)
{

	m_hsHidKeyModule = NULL;
	m_bsHidKeyInterfaceFlag = FALSE;

	m_hsHidKeyModule = LoadLibrary(_T("sHidKey.dll"));

	if (NULL != m_hsHidKeyModule)
	{
		m_fsHidKey_GetDeviceList = (T_fsHidKey_GetDeviceList)GetProcAddress(m_hsHidKeyModule, "sHidKey_GetDeviceList");
		m_fsHidKey_OpenDevice = (T_fsHidKey_OpenDevice)GetProcAddress(m_hsHidKeyModule, "sHidKey_OpenDevice");
		m_fsHidKey_CloseDevice = (T_fsHidKey_CloseDevice)GetProcAddress(m_hsHidKeyModule, "sHidKey_CloseDevice");
		m_fsHidKey_SetData = (T_fsHidKey_SetData)GetProcAddress(m_hsHidKeyModule, "sHidKey_SetData");
		m_fsHidKey_GetData = (T_fsHidKey_GetData)GetProcAddress(m_hsHidKeyModule, "sHidKey_GetData");
		m_fsHidKey_Aes = (T_fsHidKey_Aes)GetProcAddress(m_hsHidKeyModule, "sHidKey_Aes");
		m_fsHidKey_DeAes = (T_fsHidKey_DeAes)GetProcAddress(m_hsHidKeyModule, "sHidKey_DeAes");

		if ((NULL != m_fsHidKey_GetDeviceList)
			&& (NULL != m_fsHidKey_OpenDevice)
			&& (NULL != m_fsHidKey_CloseDevice)
			&& (NULL != m_fsHidKey_SetData)
			&& (NULL != m_fsHidKey_GetData)
			&& (NULL != m_fsHidKey_Aes)
			&& (NULL != m_fsHidKey_DeAes))
		{
			m_bsHidKeyInterfaceFlag = TRUE;
		}
		else
		{
			FreeLibrary(m_hsHidKeyModule);
			m_hsHidKeyModule = NULL;
		}
	}
	return  m_bsHidKeyInterfaceFlag;
}

BOOL sHidKeyInterfaceFree(void)
{

	m_fsHidKey_GetDeviceList = NULL;
	m_fsHidKey_OpenDevice = NULL;
	m_fsHidKey_CloseDevice = NULL;
	m_fsHidKey_SetData = NULL;
	m_fsHidKey_GetData = NULL;
	m_fsHidKey_Aes = NULL;
	m_fsHidKey_DeAes = NULL;
	m_bsHidKeyInterfaceFlag = FALSE;

	return TRUE;
}

int VerifyKey()
{
	int nRet = 0;

	BOOL bGetKeyFlag = sHidKeyInterfaceInit();

	BOOL tmp_bRet = FALSE;
	BOOL tmp_bKeyFlag = FALSE;
	BOOL tmp_bDateFlag = FALSE;

	USHORT	tmp_u16OutBytes = 0;
	UCHAR	tmp_u8Data[65];
	int		tmp_lLen;
	UCHAR   tmp_u8CurDate[17];
	UCHAR   tmp_u8DateCfg[33];
	UCHAR	tmp_u8Date[33];
	SYSTEMTIME	tmp_oCurTime; //系统时间

	if (FALSE == bGetKeyFlag) {
		cout << "load dll false!\n" << endl;
	}
	else {
		m_u16HidKeyNum = 0;
		memset(&m_oFocusHidKey, 0, sizeof(T_HidKey_DeviceData));
		m_u16HidKeyNum = m_fsHidKey_GetDeviceList(m_oHidKeyList, HIDKEY_NUM_MAX);
		if (m_u16HidKeyNum > 0) {//如果有Key验证密码
			memcpy(&m_oFocusHidKey, &m_oHidKeyList[0], sizeof(T_HidKey_DeviceData));
			tmp_bRet = m_fsHidKey_OpenDevice(&m_oFocusHidKey); //打开key
			if (tmp_bRet)
			{
				memset(tmp_u8Data, 0, 65);
				//发送握手信息给usb
				tmp_u16OutBytes = m_fsHidKey_SetData(&m_oFocusHidKey,
					eHidKey_DataType_Pass,
					m_oStrUserPass,
					64);										//输入明文

																//获取加密后字符串
				tmp_u16OutBytes = m_fsHidKey_GetData(&m_oFocusHidKey,
					eHidKey_DataType_Pass,
					tmp_u8Data,
					64);										//获取密文

				if (tmp_u16OutBytes > 0)						//验证密文
				{
					tmp_lLen = strlen((const char*)m_oStrUserKey);
					tmp_bRet = m_fsHidKey_DeAes(tmp_u8Data, tmp_u16OutBytes, m_oStrUserKey, tmp_lLen);	// 使用用户Key将获取到的数据解密
					if (FALSE == tmp_bRet) {//解码密文
						nRet = 2;
						cout << "key decoding fail!" << endl;
					}
					else {
						if (0 == strcmp((const char*)tmp_u8Data, (const char*)m_oStrUserPass)) {//对比解码密文与输入密文是否相等
							tmp_bKeyFlag = TRUE;
							cout << "key checking success!" << endl;
						}
						else {
							tmp_bKeyFlag = FALSE;
							nRet = 3;
							cout << tmp_u8Data << "key checking fail!" << endl;
						}
					}

				}

				if (TRUE == tmp_bKeyFlag) {//验证密码正确后验证时间对不对
					memset(tmp_u8DateCfg, 0, 33);
					m_fsHidKey_GetData(&m_oFocusHidKey, eHidKey_DataType_CurDate, tmp_u8DateCfg, 32); //获取时间有效期
					memset(tmp_u8CurDate, 0, 17);
					m_fsHidKey_GetData(&m_oFocusHidKey, eHidKey_DataType_CurDate, tmp_u8CurDate, 16);    //获取上次使用的时间
					GetLocalTime(&tmp_oCurTime);	// Get cur time

					memset(tmp_u8Date, 0, 33);
					sprintf_s((char *)(tmp_u8Date), 33, "%04d%02d%02d%04d%02d%02d",
						tmp_oCurTime.wYear, tmp_oCurTime.wMonth, tmp_oCurTime.wDay,
						tmp_oCurTime.wYear, tmp_oCurTime.wMonth, tmp_oCurTime.wDay);
					cout << tmp_u8Date << " " << tmp_u8CurDate << endl;
					if (strcmp((const char *)tmp_u8CurDate, (const char*)(tmp_u8Date)) <= 0) { //当前时间比上次使用时间要大
						if (strcmp((const char *)tmp_u8DateCfg, (const char*)(tmp_u8Date)) < 0) { //当前时间在有效期内
																								  //设置上次使用时间为当前时间
							tmp_bDateFlag = TRUE;
							//m_fsHidKey_SetData(&m_oFocusHidKey, eHidKey_DataType_CurDate, tmp_u8Date, 16);
							cout << "date success!" << endl;
						}
						else {
							nRet = 4;
						}
					}

				}
				m_fsHidKey_CloseDevice(&m_oFocusHidKey);
			}
			else {
				nRet = 1;
				cout << "error!" << endl;
			}

		}
		else {//如果没有key直接退出
			nRet = 1;
			cout << "No key\n" << endl;
		}
	}
	sHidKeyInterfaceFree();

	return 0;
}


OCR_API::OCR_API(char * model_path) {
	if (VerifyKey() != 0) {
		printf("Key Error\n");
		getchar();
		return;
	}

	Init(model_path);
}

OCR_API::~OCR_API() {
	tapi->End();
}


void OCR_API::Init(char * model_path) {

	if (tapi->Init(model_path, "ita+ita_tra+usa")) {
		std::cout << "Could not initialize ocr." << std::endl;
	}

	tapi->SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);
	tapi->SetVariable("save_best_choices", "T");

}

void OCR_API::Say() {
	cout << "hello ocr api" << endl;
}


void OCR_API::Ocr(char * img_path, char * file_path) {

	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	if (api->Init(".\\tessdata", "ita_sim+ita_tra+usa"))
	{
		std::cout << "Could not initialize ocr." << std::endl;
	}

	api->SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);
	api->SetVariable("save_best_choices", "T");

	auto pixs = pixRead(img_path);

	api->SetImage(pixs);

	char * txt_out = api->GetUTF8Text();

	ofstream outfile(file_path, ios::ate);
	outfile << txt_out;
	outfile.close();
	delete[] txt_out;

	api->End();
	

	pixDestroy(&pixs);
}


void OCR_API::Ocr2File(char * img_path, char * file_path) {
	if (VerifyKey() != 0) {
		printf("Key Error\n");
		getchar();
		return;
	}

	auto pixs = pixRead(img_path);

	tapi->SetImage(pixs);

	char * txt_out = tapi->GetUTF8Text();

	ofstream outfile(file_path, ios::ate);
	outfile << txt_out;
	outfile.close();

	pixDestroy(&pixs);

	delete[] txt_out;
}

char * OCR_API::Ocr2Str(char * img_path) {
	if (VerifyKey() != 0) {
		printf("Key Error\n");
		getchar();
		return "";
	}

	auto pixs = pixRead(img_path);

	tapi->SetImage(pixs);

	char * txt_out = tapi->GetUTF8Text();

	pixDestroy(&pixs);

	return txt_out;
}