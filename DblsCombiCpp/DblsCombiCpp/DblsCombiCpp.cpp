#include <iostream>
#include "Schedule.h"

using namespace std;

int main()
{
	Schedule sch(2, 8);		//	2面、8プレイヤー
	sch.print();

    std::cout << "\nOK.\n";
}
