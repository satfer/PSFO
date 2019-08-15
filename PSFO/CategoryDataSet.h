#pragma once
#include <cstdio>
#include <vector>
#include <string>


// CategoryDataSet ����ǻ��ඨ��ӿ�, �û���ָ��ʵ���������ݵĶ�̬; ���ǲ�������, һ��Ҫ������������͵�����;����, ��������ָ����Ʋ��ò���;
// ��̬��CategoryDataStreamʵ����
/*class CategoryDataSet {
public:
	virtual int readItem() = 0;
	virtual std::vector<int> readItemSet() = 0;
	virtual int getItems() = 0;
	virtual int getLines() = 0;
	//virtual int getDomainMax() = 0;
	virtual void reset() = 0;
};*/

//���Դ�Kosarak����֮���Ƶ�datafile: ÿһ�ж���һ���򼸸�������, �ж���
//class Kosarak : public CategoryDataSet { 
class Kosarak {
public:
	Kosarak() = delete;
	Kosarak(const char* dataFile, bool refreshInfo = false) : _dataFileName(dataFile){
		if (fopen_s(&_fp, dataFile, "r") != 0)
			throw("Open data file failed");
		std::string dataInfo(dataFile);
		dataInfo.append(".info");
		FILE *fpInfo;
		if (refreshInfo == false && fopen_s(&fpInfo, dataInfo.c_str(), "r") == 0) {
			fscanf_s(fpInfo, "%d %d %d", &_lines, &_items, &_domainMax);
			printf("Data.info show there are %d lines and %d items, with max value %d .\n", _lines, _items, _domainMax); // ��windows���±�������, BOM��û��Ӱ��
			fclose(fpInfo);
		}
		else {
			// �Լ����ַ���������
			int tempItem;
			fscanf_s(_fp, "%d", &tempItem);
			int itemCounter = 0, lineCounter = 1, domainMax = 0;
			while (!feof(_fp)) {
				++itemCounter;
				if (tempItem > domainMax) {
					domainMax = tempItem;
				}
				if (fgetc(_fp) == '\n') {
					++lineCounter;
				}
				fscanf_s(_fp, "%d", &tempItem);
			}
			if (itemCounter == 0)
				lineCounter = 0;
			_items = itemCounter;
			_lines = lineCounter;
			_domainMax = domainMax;
			if (fopen_s(&fpInfo, dataInfo.c_str(), "w") == 0) {
				fprintf(fpInfo, "%d %d %d", _lines, _items, _domainMax);
				fclose(fpInfo);
			}
			else {
				throw("Open data.info file to write failed");
			}
		}
		_itemControl = _items;
		_lineControl = _lines;
		reset();
		printf("Kosarak(-like) dataset loaded successed.\n");
	}
	~Kosarak() {
		fclose(_fp);
	}
	Kosarak(const Kosarak& b) {
		if (fopen_s(&_fp, b._dataFileName.c_str(), "r") != 0)
			throw("Open data file failed");
		_dataFileName = b._dataFileName;
		_item = 1;
		_line = 1;
		_itemControl = b._itemControl;
		_lineControl = b._lineControl;
		_items = b._items;
		_lines = b._lines;
		_domainMax = b._domainMax;

	}
	Kosarak& operator=(const Kosarak& b) {
		if (this == &b)
			return *this;
		if (fopen_s(&_fp, b._dataFileName.c_str(), "r") != 0)
			throw("Open data file failed");
		_dataFileName = b._dataFileName;
		_item = 1;
		_line = 1;
		_itemControl = b._itemControl;
		_lineControl = b._lineControl;
		_items = b._items;
		_lines = b._lines;
		_domainMax = b._domainMax;
		return *this;
	}

	int readItem() {
		if (_line >= _lineControl + 1 || _item >= _itemControl + 1)
			return -1;
		int res;
		fscanf_s(_fp, "%d", &res);
		++_item;
		if (fgetc(_fp) == '\n') {
			++_line;
		}
		return res;
	}
	std::vector<int> readItemSet() {
		if (_line >= _lineControl + 1 || _item >= _itemControl + 1)
			return { -1 };
		std::vector<int> res;
		int temp;
		fscanf_s(_fp, "%d", &temp);
		res.push_back(temp);
		++_item;
		while (fgetc(_fp) != '\n') {
			fscanf_s(_fp, "%d", &temp);
			res.push_back(temp);
			++_item;
		}
		++_line;
		return res;
	}
	void setIdControl(int idControl) {
		_itemControl = idControl;
	}
	void setLineControl(int lineControl) {
		_lineControl = lineControl;
	}
	int getItems() {
		return _items;
	}
	int getLines() {
		return _lines;
	}
	int getDomainMax() {
		return _domainMax;
	}
	void moveToItem(int item) {

	}
	void moveToLine(int line) {

	}
	void reset() {
		rewind(_fp); //fseek(_fp,0,SEEK_SET);
		_item = 1;
		_line = 1;
		_itemControl = _items;
		_lineControl = _lines;
	}
private:
	FILE *_fp;
	std::string _dataFileName; //�����ֶ�ά����
	int _item = 1; // ��һ������ǵ�_item��(��1��ʼ��)
	int _line = 1; // ��һ������ǵ�_line��(��1��ʼ��)
	int _itemControl; // ������_itemControl�����֮�������
	int _lineControl; // ������_lineControl�����֮�������
	int _items;
	int _lines;
	int _domainMax;
};