#pragma once


class OCR_API {
public:
	OCR_API();
	virtual ~OCR_API();
	void CheckLib();
	
	void Ocr2File(char * img_path, char * file_path);
	char * Ocr2Str(char * img_path);

	//错误代码
	// 0 : 正常
	// 1 : usb不存在	
	// 2 : usbkey解码失败
	// 3 : usbkey密码验证失败
	// 4 : usbkey超期 
	// 5 : 动态链接库不存在
	// 6 : 输入或输出不存在
	int status;

private:
	void Ocr(char * img_path, char * file_path);
	void Init();

};
