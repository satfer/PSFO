#pragma once
#include <random>
#include <chrono>

int trivialHash(int g, int x) {
	return x % g;
}
int prime2Hash(int g, int x) {
	return x * 2 % g;
}
int prime3Hash(int g, int x) {
	return x * 3 % g;
}
int prime5Hash(int g, int x) {
	return x * 5 % g;
}
int prime7Hash(int g, int x) {
	return x * 7 % g;
}
int prime11Hash(int g, int x) {
	return x * 11 % g;
}
int prime13Hash(int g, int x) {
	return x * 13 % g;
}
int prime17Hash(int g, int x) {
	return x * 17 % g;
}
int prime19Hash(int g, int x) {
	return x * 19 % g;
}
class HashFunctions {
public:
	HashFunctions() = default;
	HashFunctions(int g, int n) : _g(g) {//输入域0,1,...,n-1; 输出域0,1,...,_g-1
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		_generator = std::default_random_engine(seed);
		_rd = std::uniform_int_distribution<int>(0, _hashTypes - 1);
		std::vector<std::vector<int>> inverseMappingOfI;
		std::vector<int> inverseMappingOfIWithHashJ;
		for (int i = 0; i < _g; ++i) {
			inverseMappingOfI = {};
			for (int j = 0; j < _hashTypes; ++j) {
				inverseMappingOfIWithHashJ = {};
				for (int k = 0; k < n; ++k) {
					if (_hash[j](_g, k) == i) {
						inverseMappingOfIWithHashJ.push_back(k);
					}
				}
				inverseMappingOfI.push_back(inverseMappingOfIWithHashJ);
			}
			_inverseMapping.push_back(inverseMappingOfI);
		}
	}
	int hash(int x, int &hnumber) {
		hnumber = _rd(_generator);
		return _hash[hnumber](_g, x); // (*_hash)也错
	}
	std::vector<std::vector<std::vector<int>>>& getInverseMapping() {
		return _inverseMapping;
	}
	std::vector<int>& getInverseMappingOf(int x, int hnumber) {
		return _inverseMapping[x][hnumber];
	}
	int getG() {
		return _g;
	}
private:
	int _g; // 输出域0,1,...,_g-1
	int _hashTypes = 9; // hash函数的数量, 记得和下面[]中的数一起改
	int(*_hash[9])(int, int) { trivialHash, prime2Hash, prime3Hash, prime5Hash, prime7Hash, prime11Hash, prime13Hash, prime17Hash, prime19Hash }; // 去掉*说不能用函数数组, 但是这样函数指针数组是可以的
	std::default_random_engine _generator;
	std::uniform_int_distribution<int> _rd;
	std::vector<std::vector<std::vector<int>>> _inverseMapping;
};