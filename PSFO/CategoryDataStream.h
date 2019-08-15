#pragma once
#include "CategoryDataSet.h"
#include <algorithm>

// DataSet�ķ�װ(����˵bridge), �����������int
class CategoryDataStream {
public:
	CategoryDataStream() = delete;
	// ��Ȼ������, ��������������ͽ�����, ������ı�֮ǰ��, CategoryDataStream��Ҳ�Ǹ��Ƶ�, ����ı�֮ǰ��, ֮��д�ƶ���ĳһ�е�ʱ��Ӧ�û�������Ӧ�޸�
	CategoryDataStream(const Kosarak &dataSet, int model) : _dataSet(dataSet) { setModel(model); }
	CategoryDataStream(const Kosarak &dataSet, const std::vector<int> &candidateItems, int model) : _dataSet(dataSet), _candidateItems(candidateItems){ 
		setModel(model);
		// std::sort(_candidateItems.begin(), _candidateItems.end()); // ��Ϊ�������ͳ�ʼ��encoder����һ���ط�, �ɴ�Ū�����ٴ�����, Ҫ�� �����ǰ��
	}
	~CategoryDataStream() = default;
	CategoryDataStream(const CategoryDataStream &) = default;
	CategoryDataStream& operator=(const CategoryDataStream &) = default;

	void setModel(int model) {
		if (model >= 0 && model <= 2)
			_model = model;
		else {
			_model = 0;
			printf("modelֻ����0, 1, 2, ������Ϊ0 .");
		}
	}
	int getModel() {
		return _model;
	}
	int getN() { // ���Ͷ���������, ��Ӧ��Ҳ����Ҫ��ǰ֪��, Aggregator��֪���Լ��ռ��˶�������. candidateItems��model==0ʱ, �������ʵ�ʷ��͵�����
		switch (_model) {
		case 0:
			return _dataSet.getItems();
		case 1:
		case 2:
			return _dataSet.getLines();
		}
	}
	//int getDomainSize()   �ж���������

	// int next(); �������Ͳ�ͬ, û��д��һ��next
	int nextNumber() {
		if (_model == 0) {
			int temp = _dataSet.readItem();
			if (!_candidateItems.empty() && !_candidateItemSets.empty())
				throw("_candidateItems��_candidateItemSets����ͬʱ�ǿ�.");
			else if (!_candidateItems.empty()) {
				/*while (!_candidateItemsHas(temp)) {
					temp = _dataSet.readItem();
				}*/ // ���ǶԵ�, ûɶ���尡_candidateItems+�������ģʽ
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
				throw("_candidateItems��_candidateItemSets����ͬʱ�ǿ�.");
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
				throw("_candidateItems��_candidateItemSets����ͬʱ�ǿ�.");
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
	int _model; //0, 1, 2��Ӧ���һ��, ���һ�еĳ���, ���һ��
	std::vector<int> _candidateItems;
	std::vector<std::vector<int>> _candidateItemSets;

	bool _candidateItemsHas(int x) { //���ַ�, �Ӵ�С����
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

