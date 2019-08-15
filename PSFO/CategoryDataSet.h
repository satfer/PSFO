#pragma once
#include <cstdio>
#include <vector>
#include <string>


// CategoryDataSet 想的是基类定义接口, 用基类指针实现输入数据的多态; 但是并不好用, 一是要定死输出的类型导致用途不广, 二是子类指针深复制不好操作;
// 多态靠CategoryDataStream实现了
/*class CategoryDataSet {
public:
	virtual int readItem() = 0;
	virtual std::vector<int> readItemSet() = 0;
	virtual int getItems() = 0;
	virtual int getLines() = 0;
	//virtual int getDomainMax() = 0;
	virtual void reset() = 0;
};*/

//可以打开Kosarak和与之类似的datafile: 每一行都是一个或几个正整数, 有多行
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
			printf("Data.info show there are %d lines and %d items, with max value %d .\n", _lines, _items, _domainMax); // 用windows记事本创建的, BOM有没有影响
			fclose(fpInfo);
		}
		else {
			// 自己数字符数和行数
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
	std::string _dataFileName; //懒得手动维护了
	int _item = 1; // 下一个输出是第_item个(从1开始数)
	int _line = 1; // 下一个输出是第_line行(从1开始数)
	int _itemControl; // 读到第_itemControl个输出之后不再输出
	int _lineControl; // 读到第_lineControl行输出之后不再输出
	int _items;
	int _lines;
	int _domainMax;
};