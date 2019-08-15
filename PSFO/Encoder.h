#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include "CategoryDataStream.h"



// Encoder��������������0,1,..,N-1,���㴦��, ��ȻҲ��������Ӧ�Ľ�����Ϣ�ܻ�ԭ������ɶ

// encoder��Ҫ����?, ������һ��datasetҪ����һ��encoder, ��FO(CategoryDataStream& s, int PFType, int AgType, double ep) �������õĻ��Ͳ���Ҫ
// ��vector<int>���int�ı������
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
			vectorToInt.insert(std::pair<std::vector<int>, int>(b, vectorToInt.size() - 1)); // ��insert��ΪvectorToInt[b] = vectorToInt.size() - 1 ��Чinsert_or_assig, ������һ����insert;
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
	int getSize() { // ָdomainSize
		return intToVector.size(); // �����Ϊ0,1,...,N-1
	}
private:
	std::vector<std::vector<int>> intToVector;
	std::map<std::vector<int>, int> vectorToInt;
};

// ��int�������int(0...n-1)�ı������
class IntIntEncoder {
public:
	IntIntEncoder() = default;
	IntIntEncoder(std::vector<int> decoder) : _decoder(decoder), _continuity(false), _size(decoder.size()) {
		for (int i = 0; i < _decoder.size(); ++i)
			_encoder.insert(std::pair<int, int>(_decoder[i], i)); // ���decoder���ظ���int, ���insertӦ�ûᱨ��
	}
	IntIntEncoder(int offset, int d) : _offset(offset), _size(d), _continuity(true) {
		printf("IntIntEncoder initialized manually, with continuity, _offset = %d, _size = %d .\n", _offset, _size);
	}
	IntIntEncoder(CategoryDataStream &dataStream, bool continuity = true) { // ������LDP��CategoryData������ǰ֪�����, ����˵�������ռ���server�������ټ���˽, ����д�Լ����������ʵ���ʱ��֪�����ʱ������
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
	/*IntIntEncoder(CategoryDataStream &dataStream, int arraySize) { //֪��arraySize����ǰ����ռ�ͦ��, �����Ҫ�Ļ���д��
		throw("to be done");
		while (_decoder.back() == -1) {
			_decoder.pop_back();
		}
	}*/
	~IntIntEncoder() = default;
	IntIntEncoder(const IntIntEncoder&) = default;
	IntIntEncoder& operator=(const IntIntEncoder&) = default;

	int encode(int b) {
		if (b == -1) // -1 ����eof
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
	int getSize() { // ָdomainSize
		return _size;
	}
private:
	std::map<int, int> _encoder;
	std::vector<int> _decoder;
	bool _continuity; //����ʱ��ƫ����_offset������_N����, ������ʱ������ӳ��
	int _offset; // ��_offset�����
	int _size; //ӳ���Ϊ0,1,...,d-1; size() == d
};