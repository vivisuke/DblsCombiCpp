#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "Schedule.h"

using namespace std;

cchar *_4c16p = "4,7-10,14 | 1,6-9,13  | 2,11-5,15 | 3,16-8,12 "
				"2,8-13,16 | 4,15-6,14 | 3,9-7,12  | 1,11-5,10 "
				"1,5-9,15  | 3,13-8,16 | 6,7-10,11 | 2,4-12,14 "
				"4,11-9,12 | 2,16-6,13 | 1,15-3,7  | 5,14-8,10 "
				"4,12-7,15 | 1,10-5,8  | 2,13-9,14 | 3,11-6,16 "
				"1,14-5,12 | 4,9-8,13  | 3,15-7,10 | 2,6-11,16 "
				"3,5-12,13 | 4,14-6,10 | 7,16-9,11 | 1,8-2,15  ";

int main()
{
	const int n_courts = 4;		//	面数
	const int n_players = 16;	//	プレイヤー数
	Schedule sch(n_courts, n_players);		//	面数、プレイヤー数
#if 0
	auto ptr = _4c16p;
	while( *ptr != '\0' )
		sch.add_round(ptr);
#else
	std::chrono::system_clock::time_point  start, end; // 型は auto で可
	start = std::chrono::system_clock::now(); // 計測開始時間
 	for (int r = 1; r < 8; ++r) {
		//sch.add_random_round();
		//sch.add_balanced_pair_round();
		sch.add_balanced_round();
		//sch.print();
		//sch.print_oppo_counts();
		cout << "rounds.size() = " << sch.m_rounds.size() << endl;
	}
	end = std::chrono::system_clock::now();  // 計測終了時間
	double dur = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); //処理に要した時間をミリ秒に変換
	cout << endl << "duration = " << dur << " msec" << endl;
#endif

	cout << endl;
	sch.print();
	//sch.print_pair_counts();
	sch.print_oppo_counts();

#if 0
	//cout << sch.to_HTML() << endl;
	string filename = "table_" + to_string(n_courts) + "c" + to_string(n_players) + "p.html";
	cout << "FileName = " << filename << endl;
	std::ifstream ifs(filename);
	if( ifs ) {
		string token;
		while( !ifs.eof() ) {
			ifs >> token;
			if (token == "(STD") {
				ifs >> token;		//	skip "="
				ifs >> token;		//	value
				double v = stod(token);
				ifs.close();
				if( sch.calc_oppo_counts_std() < v ) {
					std::ofstream ofs(filename);
					ofs << sch.to_HTML();
					ofs.close();
				}
				break;
			}
		}
	} else {
		std::ofstream ofs(filename);
		ofs << sch.to_HTML();
		ofs.close();
	}
#endif

    std::cout << "\nOK.\n";
}
