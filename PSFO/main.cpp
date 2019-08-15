#include "PSFO.h"
#include <iostream>
#include <thread>

void SVIM(double ep, int k) {
	// Ŀǰû�����ݷ�Ƭ ����ep����
	Kosarak dset("../dataset/kosarak.dat", false);
	//CategoryDataStream ds0(dset, 0);
	//CategoryDataStream ds1(dset, 1);

	CategoryDataStream ds2(dset, 2);
	IntIntEncoder iie(-1, 41270);
	PSFO step1(ds2, iie, 3, ep, 1);
	step1.execute();
	step1.sort();
	step1.show(2 * k);
	std::vector<int> candidates = step1.getFront(2 * k);

	CategoryDataStream ds1c(dset, candidates, 1);
	IntIntEncoder iieL(-1, 2 * k);
	FO step2(ds1c, iieL, 2, ep);
	step1.execute();
	step2.show(2 * k);
	std::vector<int> lengths = step2.getFront(2 * k);
	// lengthsȱһ����8ҳ�����ȥ��
	int L = 0, sumL = 0 ,tempSumL = 0;
	for (L = 0; L < lengths.size(); ++L) {
		// ��һƬ�����кܶ���д��, ��Ҫ��step2, ��������v���ȱ�����֮��һ��<=2*k; ��ôд����lengths.size()һ��=2*k
		// ����IntIntEncoder iieL(1, 2 * k);����L����������lengths[L-1]��
		sumL += lengths[L];
	}
	for (L = 0; L < lengths.size(); ++L) {
		tempSumL += lengths[L];
		if (tempSumL > 0.9 * sumL)
			break;
	}
	L = L + 1;

	CategoryDataStream ds2c(dset, candidates, 2);
	IntIntEncoder iieC(candidates);
	PSFO step3(ds2c, iieC, 3, ep, L);
	step3.execute();
	step3.sort();
	step3.show(2 * k);
}

void SVIMGRR(double ep, int k) {
	// Ŀǰû�����ݷ�Ƭ ����ep����
	Kosarak dset("../dataset/kosarak.dat", false);
	//CategoryDataStream ds0(dset, 0);
	//CategoryDataStream ds1(dset, 1);

	std::vector<int> candidates{ 5,2,10,0,54,29972,489,6071,20304,4947 };
	if (0) {
		CategoryDataStream ds2(dset, 2);
		IntIntEncoder iie(-1, 41270);
		PSFO step1(ds2, iie, 1, ep, 1);
		step1.execute();
		step1.sort();
		step1.show(2 * k);
		candidates = step1.getFront(2 * k);
	}
		

	CategoryDataStream ds1c(dset, candidates, 1);
	IntIntEncoder iieL(0, 2 * k + 1); //���г���0��
	FO step2(ds1c, iieL, 1, ep);
	step2.execute();
	step2.show(2 * k + 1);
	std::vector<int> lengths = step2.getFront(2 * k);
	// lengthsȱһ����8ҳ�����ȥ��
	int L = 0, sumL = 0, tempSumL = 0;
	for (L = 0; L < lengths.size(); ++L) {
		// ��һƬ�����кܶ���д��, ��Ҫ��step2, ��������v���ȱ�����֮��һ��<=2*k; ��ôд����lengths.size()һ��=2*k
		// ����IntIntEncoder iieL(1, 2 * k);����L����������lengths[L-1]��
		sumL += lengths[L];
	}
	for (L = 0; L < lengths.size(); ++L) {
		tempSumL += lengths[L];
		if (tempSumL > 0.9 * sumL)
			break;
	}
	if (L < lengths.size()) // Ŀǰ����lengths ȫ��0�����, Ȼ������L == lengths.size(), ��+1֮�������vectorԽ��
		L = L + 1;

	CategoryDataStream ds2c(dset, candidates, 2);
	IntIntEncoder iieC(candidates);
	PSFO step3(ds2c, iieC, 1, ep, L);
	step3.execute();
	step3.sort();
	step3.show(2 * k);
}

int main() {

	// ����
	//�ֶ�ѡ����dataset CategoryDataStream, ���CategoryDataStream�ֶ�����encoder ,Ȼ����FO
	Kosarak k("../dataset/kosarak.dat", false);
	//IntIntEncoder *e = new IntIntEncoder(k);
	IntIntEncoder iie(-1, 41270);
	CategoryDataStream ds0(k, 0);
	CategoryDataStream ds2(k, 2);
	double ep = 5;
	std::vector<FO*> FOs{   }; //new FO(ds0, iie, 0, ep), new FO(ds0, iie, 1, ep),new FO(ds0,iie, 2, ep)
	std::vector<PSFO*> PSFOs{  }; //new PSFO(ds2, iie, 0, ep, 10), new PSFO(ds2, iie, 1, ep, 10),
	// FO(ds0, iie, 0, ep).execute();
	std::vector<std::thread> threads;
	for (int i = 0; i < FOs.size(); ++i) {
		threads.push_back(std::thread(&FO::execute, FOs[i]));
	}
	for (int i = 0; i < PSFOs.size(); ++i) {
		threads.push_back(std::thread(&PSFO::execute, PSFOs[i]));
	}
	for (int i = 0; i < threads.size(); ++i) {
		threads[i].join();
	}
	for (int i = 0; i < FOs.size(); ++i) {
		FOs[i]->sort();
		FOs[i]->show(10);
	}
	for (int i = 0; i < PSFOs.size(); ++i) {
		PSFOs[i]->sort();
		PSFOs[i]->show(10);
	}


	//SVIM(1, 5);
	SVIMGRR(4, 5);


	system("pause");
	return 0;
}