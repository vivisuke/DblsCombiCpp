#include <iostream>
#include <chrono>
#include "Schedule.h"

using namespace std;

int main()
{
	Schedule sch(3, 14);		//	面数、プレイヤー数
	std::chrono::system_clock::time_point  start, end; // 型は auto で可
	start = std::chrono::system_clock::now(); // 計測開始時間
 	for (int r = 1; r < 8; ++r) {
		//sch.add_random_round();
		//sch.add_balanced_pair_round();
		sch.add_balanced_round();
		//sch.print();
		//sch.print_oppo_counts();
	}
	end = std::chrono::system_clock::now();  // 計測終了時間
	double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); //処理に要した時間をミリ秒に変換
	cout << "duration = " << elapsed << " msec" << endl;

	sch.print();
	sch.print_pair_counts();
	sch.print_oppo_counts();

	cout << sch.to_HTML() << endl;

    std::cout << "\nOK.\n";
}
