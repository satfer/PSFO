#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include "CategoryDataStream.h"



// Encoder把所有输入编码成0,1,..,N-1,方便处理, 当然也储存了相应的解码信息能还原输入了啥

// encoder需要复制?, 理论上一个dataset要共用一个encoder, 但FO(CategoryDataStream& s, int PFType, int AgType, double ep) 这样调用的话就不需要
// 把vector<int>变成int的编码解码
class VectorIntEncoder {
public:
	VectorIntEncoder() = default;
	//VectorIntEncoder(vector)
	VectorIntEncoder(Kosarak) {
		;
	}

	int encode(std::vector<int>&& b) {
		if (vectorToInt.count(b) == 0) {
			intToVector.push_back(b);
			vectorToInt.insert(std::pair<std::vector<int>, int>(b, vectorToInt.size() - 1)); // 用insert因为vectorToInt[b] = vectorToInt.size() - 1 等效insert_or_assig, 而这里一定是insert;
		}
		return vectorToInt[b];
	}
	std::vector<int> decode(int a) {
		if (a >= 0 && a < intToVector.size())
			return intToVector[a];
		else
			throw("Docode error.");
	}
	void printDecode(int x) {
		bool commaBool = false;
		printf("[");
		for (int xx : decode(x)) {
			if (commaBool == false)
				commaBool = true;
			else
				printf(", ");
			printf("%d", xx);
		}
		printf("]");
	}
	int getSize() { // 指domainSize
		return intToVector.size(); // 编码后为0,1,...,N-1
	}
private:
	std::vector<std::vector<int>> intToVector;
	std::map<std::vector<int>, int> vectorToInt;
};

// 把int变成连续int(0...n-1)的编码解码
class IntIntEncoder {
public:
	IntIntEncoder() = default;
	IntIntEncoder(std::vector<int> decoder) : _decoder(decoder), _continuity(false), _size(decoder.size()) {
		for (int i = 0; i < _decoder.size(); ++i)
			_encoder.insert(std::pair<int, int>(_decoder[i], i)); // 如果decoder有重复的int, 这个insert应该会报错
	}
	IntIntEncoder(int offset, int d) : _offset(offset), _size(d), _continuity(true) {
		printf("IntIntEncoder initialized manually, with continuity, _offset = %d, _size = %d .\n", _offset, _size);
	}
	IntIntEncoder(CategoryDataStream &dataStream, bool continuity = true) { // 理论上LDP的CategoryData都是提前知道域的, 不能说把数据收集到server看看域再加隐私, 这里写自己检测域是在实验的时候不知道域的时候用用
		clock_t sclock = clock();
		if (dataStream.getN() <= 0)
			throw("Empty dataset not allowed.");
		if (continuity == true) {
			int min = INT_MAX, max = 0;
			int temp = dataStream.nextNumber();
			while (temp != -1) {
				min = (temp < min ? temp : min);
				max = (temp > max ? temp : max);
				temp = dataStream.nextNumber();
			}
			_offset = min;
			_size = max - min + 1;
			_continuity = true;
			printf("IntIntEncoder initialized, with continuity, _offset = %d, _size = %d .", _offset, _size);
		}
		else {
			int min = INT_MAX, max = 0;
			int temp = dataStream.nextNumber();
			int i = 0;
			while (temp != -1) {
				min = (temp < min ? temp : min);
				max = (temp > max ? temp : max);
				if (_encoder.count(temp) == 0) {
					_decoder.push_back(temp);
					_encoder.insert(std::pair<int, int>(temp, _decoder.size() - 1));
				}
				temp = dataStream.nextNumber();
			}
			if (max - min + 1 == _decoder.size()) {
				_offset = min;
				_size = max - min + 1;
				_continuity = true;
				_encoder = {};
				_decoder = {};
				printf("IntIntEncoder initialized, with continuity, _offset = %d, _size = %d .", _offset, _size);
			}
			else {
				_continuity = false;
				_size = _decoder.size();
				printf("IntIntEncoder initialized, without continuity, _size = %d .", _size);
			}
		}
		dataStream.reset();
		printf("Use time = %d ms .\n", clock() - sclock);
	}
	/*IntIntEncoder(CategoryDataStream &dataStream, int arraySize) { //知道arraySize的提前分配空间挺好, 如果需要的话再写吧
		throw("to be done");
		while (_decoder.back() == -1) {
			_decoder.pop_back();
		}
	}*/
	~IntIntEncoder() = default;
	IntIntEncoder(const IntIntEncoder&) = default;
	IntIntEncoder& operator=(const IntIntEncoder&) = default;

	int encode(int b) {
		if (b == -1) // -1 代表eof
			return -1;
		else
			return _continuity ? b + _offset : _encoder[b];
	}
	int decode(int a) {
		return _continuity ? a - _offset : _decoder[a];
	}
	void printDecode(int x) {
		printf("%6d", decode(x));
	}
	int getSize() { // 指domainSize
		return _size;
	}
private:
	std::map<int, int> _encoder;
	std::vector<int> _decoder;
	bool _continuity; //连续时存偏移量_offset和数量_N就行, 不连续时存正反映射
	int _offset; // 减_offset后输出
	int _size; //映射后为0,1,...,d-1; size() == d
};