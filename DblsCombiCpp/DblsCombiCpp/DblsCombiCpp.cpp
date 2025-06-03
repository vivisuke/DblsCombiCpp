#include <iostream>
#include "Schedule.h"

using namespace std;

int main()
{
	Schedule sch(2, 10);		//	面数、プレイヤー数
	for (int r = 1; r != 7; ++r) {
		sch.add_random_round();
	}
	sch.print();
	sch.print_pair_counts();
	sch.print_oppo_counts();

    std::cout << "\nOK.\n";
}
