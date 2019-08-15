#include <random>
#include <chrono>
#include <cmath>

class GeneralizedRandomizedResponse {
public:
	GeneralizedRandomizedResponse() = delete;
	GeneralizedRandomizedResponse(int N) : _N(N) { //输入输出都是0,1,...,N
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		_generator = std::default_random_engine(seed);
		_gap = new int[_N];
	} 
	GeneralizedRandomizedResponse(const GeneralizedRandomizedResponse&) = delete;
	GeneralizedRandomizedResponse& operator=(const GeneralizedRandomizedResponse&) = delete;
	~GeneralizedRandomizedResponse() {
		delete[] _gap;
	}
	
	int operator()(int x){
		return 0;
	}
private:
	int _N;
	int *_gap;
	std::default_random_engine _generator;
};