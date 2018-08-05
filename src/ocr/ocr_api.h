#pragma once


class OCR_API {
public:
	OCR_API(char * model_path);
	virtual ~OCR_API();
	void Say();
	
	void Ocr2File(char * img_path, char * file_path);
	char * Ocr2Str(char * img_path);

private:
	void Ocr(char * img_path, char * file_path);
	void Init(char * model_path);

};
