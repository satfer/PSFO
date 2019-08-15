#pragma once
#include "Encoder.h"
#include "CategoryDataStream.h"
#include "HashFunctions.h"
#include <random>
#include <chrono>
#include <cmath>

// Perturb Functions
class PerturbFunction {
public:
	virtual int perturb(int) = 0;
	virtual void printInfo() = 0;
};

class NoPerturb : public PerturbFunction {
public:
	NoPerturb() = default;
	~NoPerturb() = default;
	NoPerturb(const NoPerturb&) = delete;
	NoPerturb& operator=(const NoPerturb&) = delete;

	int perturb(int x) {
		return x;
	}
	void printInfo() {
		printf("No perturb.");
	}
};

class GRR : public PerturbFunction { //Generalized Randomized Response
public:
	GRR() = delete;
	GRR(double ep, int d) : _ep(ep), _d(d) { // 输入和输出都是0,1,...,N-1
		//printf("Generalized Randomized Response 构造, 记得_N = %d 要 >= 2 .\n", _N);
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		_generator = std::default_random_engine(seed);
		_gapQ = _generator.max()/(_d - 1 + exp(_ep)); // _generator.max() == 4294967295, _generator.max() == 1
	}
	~GRR() = default;
	GRR(const GRR&) = delete;
	GRR& operator=(const GRR&) = delete;

	// 0到q变0, q到2q变1, ..., (_N-1)q之外概率p返回原来的x, (i-1)q到iq变i(i>x)
	virtual int perturb(int x) {
		int res = (int)(_generator() / _gapQ);
		if (res < x)
			return res;
		else if (res >= _d - 1)
			return x;
		else
			return res + 1;
	}
	virtual void printInfo() {
		printf("GRR with ep = %.2f, d = %d .", _ep, _d);
	}
protected:
	double _ep;
	int _d;
	double _gapQ;
	std::default_random_engine _generator;
};


class OLH : public GRR { // HashFunctions 写到外部
public:
	OLH() = delete;
	OLH(double ep, int g) : GRR(ep, g) {}

	int perturb(int x) {
		return GRR::perturb(x);
	}
	void printInfo() {
		printf("OLH with ep = %.2f, g = %d .", _ep, _d);
	}
};

// aggregators //Aggregator快收尾的时候写, 毕竟就一个解码
class Aggregator {
public:
	Aggregator() = delete;
	Aggregator(int d) : _d(d) { 
		_counter = std::vector<std::pair<int, int>>(d);
		for (int i = 0; i < d; ++i)
			_counter[i] = { i, 0 };
	}
	virtual void collect(int x) = 0;
	virtual void unbiase() = 0;
	void unbiase(int l) {
		for (auto& p : _counter) {
			p.second = l * p.second;
		}
	}
	void unbiase(double u) {
		for (auto& p : _counter) {
			p.second = u * p.second;
		}
	}
	void sort() {
		std::sort(_counter.begin(), _counter.end(), [](std::pair<int, int> a, std::pair<int, int> b) -> bool { return a.second > b.second; });
	}
	std::vector<std::pair<int, int>>& getCounter() {
		return _counter;
	}
	std::vector<int> getFront(int num) {
		std::vector<int> res(num);
		for (int i = 0; i < num && i < _counter.size(); ++i) {
			res[i] = _counter[i].first;
		}
		return res;
	}
protected:
	std::vector<std::pair<int, int>> _counter;
	int _d;
	int _N; // 收集多少条数据, 不应该也不需要提前知道; 记得collect中++_N
};

class NoPerturbAg : public Aggregator {
public:
	NoPerturbAg() = delete;
	NoPerturbAg(int d) : Aggregator(d) {}
	void collect(int x) {
		++_counter[x].second;
		++_N;
	}
	void unbiase() {}
};

class GRRAg : public Aggregator {
public:
	GRRAg() = delete;
	GRRAg(double ep, int d) : Aggregator(d), _ep(ep) {}
	void collect(int x) {
		++_counter[x].second;
		++_N;
	}
	void unbiase() {
		if (_N <= 0)
			throw("N should be positive. 记得collect的时候++_N");
		int a = 1 + _d / (exp(_ep) - 1);
		int b = -_N / (exp(_ep) - 1);
		for (auto& p : _counter) {
			p.second = a * p.second + b;
		}
	}
protected:
	double _ep;
	double _N;
};

class OLHAg : public Aggregator {
public:
	OLHAg() = delete;
	OLHAg(double ep, int d, int g) : Aggregator(d), _ep(ep), _g(g) {	}
	void collect(int x) {
		++_counter[x].second;
		++_N;
	}
	void unbiase() {
		if (_N <= 0)
			throw("N should be positive. 记得collect的时候++_N");
		int a = 1 / (exp(_ep) / (exp(_ep) + _g - 1) - 1.0 / _g);
		int b = -_N / _g / (exp(_ep) / (exp(_ep) + _g - 1) - 1.0 / _g);
		for (auto& p : _counter) {
			p.second = a * p.second + b;
		}
	}
protected:
	double _ep;
	double _g;
};

/*class CLDP {
	virtual void execute() = 0;
	virtual void show(int n) = 0;
};*/

// Frequency Oracles 
class FO {
public:
	FO() = delete;
	FO(CategoryDataStream categoryDataStream, IntIntEncoder iiencoder, int model, double ep) : _categoryDataStream(categoryDataStream), _iiencoder(iiencoder), _model(model), _ep(ep) {
		_initFOAg();
	}
	~FO() {
		delete _PF;
		delete _Ag;
	}
	FO(const FO&) = delete;
	FO& operator=(const FO&) = delete;

	void execute() {
		_categoryDataStream.reset();
		int hnumber, i;
		int x = _getInput();
		while (x != -1) {
			if (_model == 2) {
				x = _hashfunctions.hash(x, hnumber);
			}
			x = _PF->perturb(x);
			if (_model == 0 || _model==1)
				_Ag->collect(x);
			else if (_model == 2) {
				for (int xx : _hashfunctions.getInverseMappingOf(x, hnumber)) {
					_Ag->collect(xx);
				}
			}
			x = _getInput();
		}
		_Ag->unbiase();
	}
	void sort() {
		_Ag->sort();
	}
	void show(int n) {
		printf("Perturb Function: ");
		_PF->printInfo();
		printf(" Aggregator: ");
		//_Ag->?
		printf("\n");
		std::vector<std::pair<int, int>>& counter = _Ag->getCounter();
		for (int i = 0; i < n && i < counter.size(); ++i) {
			if (_categoryDataStream.getModel() == 0 || _categoryDataStream.getModel() == 1)
				_iiencoder.printDecode(counter[i].first);
			else if (_categoryDataStream.getModel() == 2)
				_viencoder.printDecode(counter[i].first);
			else
				throw("Unknown model of _categoryDataStream.");
			printf("出现了%d次.\n", counter[i].second);
		}
	}
	std::vector<int> getFront(int num) {
		return _Ag->getFront(num);
	}
private:
	void _initFOAg() {
		int g;
		int domainSize;
		if (_categoryDataStream.getModel() == 0 || _categoryDataStream.getModel() == 1)
			domainSize = _iiencoder.getSize();
		else if (_categoryDataStream.getModel() == 2)
			domainSize = _viencoder.getSize();
		switch (_model) {
		case 0:
			_PF = new NoPerturb();
			_Ag = new NoPerturbAg(domainSize);
			break;
		case 1:
			_PF = new GRR(_ep, domainSize);
			_Ag = new GRRAg(_ep, domainSize);
			break;
		case 2:
			g = int(exp(_ep) + 1);
			_PF = new OLH(_ep, g);
			_Ag = new OLHAg(_ep, domainSize, g);
			_hashfunctions = HashFunctions(g, domainSize);
			break;
		default:
			throw("Unknown Type.");
		}
	}
	int _getInput() { // 和_categoryDataStream输出 接口的地方
		int x;
		if (_categoryDataStream.getModel() == 0 || _categoryDataStream.getModel() == 1)
			x = _iiencoder.encode(_categoryDataStream.nextNumber());
		else if (_categoryDataStream.getModel() == 2)
			x = _viencoder.encode(_categoryDataStream.nextNumbers());
		else
			throw("Unknown model of _categoryDataStream.");
		return x;
	}
	int _model;
	CategoryDataStream _categoryDataStream;
	PerturbFunction *_PF;
	Aggregator *_Ag;
	double _ep;
	HashFunctions _hashfunctions;
	IntIntEncoder _iiencoder;
	VectorIntEncoder _viencoder;
	// int _parallel; // 并行数, 构造时输入0的话会变成最大核数
};

//  Padding-and-Sampling-based Frequency Oracle
// PSFO和FO不是is也不是has, 他们是同类, (可以有同一个虚基类)
class PSFO {
public:
	PSFO() = delete;
	PSFO(CategoryDataStream categoryDataStream, IntIntEncoder iiencoder, int model, double ep, int l, double u = 1.0) : 
		_categoryDataStream(categoryDataStream), _iiencoder(iiencoder), _model(model), _ep(ep), _l(l), _u(u) {
		if (_categoryDataStream.getModel() != 2)
			throw("PSFO only support inputting vector, that is categoryDataStream.getModel() == 2");
		_initFOAg();
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		_generator = std::default_random_engine(seed);
		_rdDomain = std::uniform_int_distribution<int>(0, _iiencoder.getSize() - 1);
		_rdL = std::uniform_int_distribution<int>(0, _l - 1);
	}
	~PSFO() {
		delete _PF;
		delete _Ag;
	}
	PSFO(const FO&) = delete;
	PSFO& operator=(const FO&) = delete;

	void execute() {
		_categoryDataStream.reset();
		int hnumber, i;
		int x = _getInput();
		while (x != -1) {
			if (_model == 2) {
				x = _hashfunctions.hash(x, hnumber);
			}
			x = _PF->perturb(x);
			if (_model == 0 || _model == 1)
				_Ag->collect(x);
			else if (_model == 2) {
				for (int xx : _hashfunctions.getInverseMappingOf(x, hnumber)) {
					_Ag->collect(xx);
				}
			}
			x = _getInput();
		}
		_Ag->unbiase();
		_Ag->unbiase(_l);
		_Ag->unbiase(_u);
	}
	void sort() {
		_Ag->sort();
	}
	void show(int n) {
		printf("Perturb Function: ");
		_PF->printInfo();
		printf(" Aggregator: ");
		//_Ag->?
		printf("\n");
		std::vector<std::pair<int, int>>& counter = _Ag->getCounter();
		for (int i = 0; i < n && i < counter.size(); ++i) {
			_iiencoder.printDecode(counter[i].first);
			printf("出现了%d次.\n", counter[i].second);
		}
	}
	std::vector<int> getFront(int num) {
		return _Ag->getFront(num);
	}
protected:
	void _initFOAg() {
		int g;
		int domainSize = _iiencoder.getSize();
		if (_model == 3) { //Adap
			if (domainSize < exp(_ep) * _l * (4 * _l - 1) + 1) {
				_model = 1;
				_ep = log(_l * (exp(_ep) - 1) + 1);
			}
			else {
				_model = 2;
			}
		}
		switch (_model) {
		case 0:
			_PF = new NoPerturb();
			_Ag = new NoPerturbAg(domainSize);
			break;
		case 1:
			_PF = new GRR(_ep, domainSize);
			_Ag = new GRRAg(_ep, domainSize);
			break;
		case 2:
			g = int(exp(_ep) + 1);
			_PF = new OLH(_ep, g);
			_Ag = new OLHAg(_ep, domainSize, g);
			_hashfunctions = HashFunctions(g, domainSize);
			break;
		default:
			throw("Unknown Type.");
		}
	}
	int _getInput() {
		if (_categoryDataStream.getModel() != 2)
			throw("PSFO only support inputting vector, that is categoryDataStream.getModel() == 2");
		// 随机padding and sampling 和 抽到vector外再随机的条件概率是一样的, 正确性由概率的独立性(伪随机的随机独立性能)保证
		int x, temp;
		std::vector<int> tempVector = _categoryDataStream.nextNumbers();
		if (tempVector.size() >= _l) {
			x = tempVector[std::uniform_int_distribution<int>(0, tempVector.size() - 1)(_generator)];
			x = _iiencoder.encode(x);
		}
		else {
			temp = _rdL(_generator);
			if (temp < tempVector.size()) {
				x = tempVector[temp];
				x = _iiencoder.encode(x);
			}
			else
				x = _rdDomain(_generator); // 这是在0,...,domainSize-1里随机, 相当于已经encode
		}
		return x;
	}
	int _model;
	CategoryDataStream _categoryDataStream;
	PerturbFunction *_PF;
	Aggregator *_Ag;
	double _ep;
	int _l; //padding的长度
	double _u; //u是Ag的修正值, 指论文里的u(L)
	HashFunctions _hashfunctions;
	IntIntEncoder _iiencoder;
	VectorIntEncoder _viencoder;
	std::default_random_engine _generator; // 随机这东西, 无所谓吧, 理论上(标准库够好的话)每次随机都是相互独立的
	std::uniform_int_distribution<int> _rdDomain;
	std::uniform_int_distribution<int> _rdL;
};