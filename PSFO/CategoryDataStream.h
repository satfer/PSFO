#pragma once
#include "CategoryDataSet.h"
#include <algorithm>

// DataSet的封装(或者说bridge), 用于输出单个int
class CategoryDataStream {
public:
	CategoryDataStream() = delete;
	// 虽然是引用, 但是拿来复制完就结束了, 即不会改变之前的, CategoryDataStream里也是复制的, 不会改变之前的, 之后写移动到某一行的时候应该会做出相应修改
	CategoryDataStream(const Kosarak &dataSet, int model) : _dataSet(dataSet) { setModel(model); }
	CategoryDataStream(const Kosarak &dataSet, const std::vector<int> &candidateItems, int model) : _dataSet(dataSet), _candidateItems(candidateItems){ 
		setModel(model);
		// std::sort(_candidateItems.begin(), _candidateItems.end()); // 因为传进来和初始化encoder不再一个地方, 干脆弄好了再传进来, 要求 大的在前面
	}
	~CategoryDataStream() = default;
	CategoryDataStream(const CategoryDataStream &) = default;
	CategoryDataStream& operator=(const CategoryDataStream &) = default;

	void setModel(int model) {
		if (model >= 0 && model <= 2)
			_model = model;
		else {
			_model = 0;
			printf("model只能是0, 1, 2, 已设置为0 .");
		}
	}
	int getModel() {
		return _model;
	}
	int getN() { // 发送多少条数据, 不应该也不需要提前知道, Aggregator能知道自己收集了多少数据. candidateItems且model==0时, 这个不是实际发送的数据
		switch (_model) {
		case 0:
			return _dataSet.getItems();
		case 1:
		case 2:
			return _dataSet.getLines();
		}
	}
	//int getDomainSize()   有多少种输入

	// int next(); 返回类型不同, 没法写成一个next
	int nextNumber() {
		if (_model == 0) {
			int temp = _dataSet.readItem();
			if (!_candidateItems.empty() && !_candidateItemSets.empty())
				throw("_candidateItems和_candidateItemSets不会同时非空.");
			else if (!_candidateItems.empty()) {
				/*while (!_candidateItemsHas(temp)) {
					temp = _dataSet.readItem();
				}*/ // 对是对的, 没啥意义啊_candidateItems+单个输出模式
				throw("?");
			}
			else if (!_candidateItemSets.empty()) {
				throw("?");
			}
			return temp;
		}
		else if (_model == 1) {
			std::vector<int> temp = _dataSet.readItemSet();
			if (!_candidateItems.empty() && !_candidateItemSets.empty())
				throw("_candidateItems和_candidateItemSets不会同时非空.");
			else if (!_candidateItems.empty()) {
				std::vector<int> res;
				for (int x : temp) {
					if (_candidateItemsHas(x)) {
						res.push_back(x);
					}
				}
				temp = res;
			}
			else if (!_candidateItemSets.empty()) {
				throw("?");
			}
			return temp.size();
		}
		else
			throw("Error use of next*()");
	}
	std::vector<int> nextNumbers() {
		if (_model == 2) {
			std::vector<int> temp = _dataSet.readItemSet();
			if (!_candidateItems.empty() && !_candidateItemSets.empty())
				throw("_candidateItems和_candidateItemSets不会同时非空.");
			else if (!_candidateItems.empty()) {
				std::vector<int> res;
				for (int x : temp) {
					if (_candidateItemsHas(x)) {
						res.push_back(x);
					}
				}
				temp = res;
			}
			else if (!_candidateItemSets.empty()) {
				throw("?");
			}
			return temp;
		}
		else
			throw("Error use of next*()");
	}
	void reset() {
		_dataSet.reset();
	}
private:
	Kosarak _dataSet;
	int _model; //0, 1, 2对应输出一项, 输出一行的长度, 输出一行
	std::vector<int> _candidateItems;
	std::vector<std::vector<int>> _candidateItemSets;

	bool _candidateItemsHas(int x) { //二分法, 从大到小排列
		int low = 0, high = _candidateItems.size() - 1, mid;
		while (low <= high) {
			mid = (low + high) / 2;
			if (_candidateItems[mid] == x)
				return true;
			else if (_candidateItems[mid] > x)
				low = mid + 1;
			else
				high = mid - 1;
		}
		return false;
	}
};

