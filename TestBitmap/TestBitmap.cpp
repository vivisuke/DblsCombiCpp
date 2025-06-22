#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <intrin.h>
#include <bit> // C++20以降で必要

using namespace std;

using uchar = unsigned char;
using ushort = unsigned short;
using PlayerID = unsigned short;
using Bitmap = int;

const int N_PLAYERS = 16;		//	プレイヤー数

int	g_cnt = 0;
vector<PlayerID> g_ar(N_PLAYERS);

bool is_valid(const vector<PlayerID> &ar) {
	for(int i = 0; i < ar.size(); i += 2) {
		if( ar[i] >= ar[i+1] ) return false;
	}
	for(int i = 0; i < ar.size() - 2; i += 4) {
		if( ar[i] >= ar[i+2] ) return false;
	}
	for(int i = 0; i < ar.size() - 4; i += 4) {
		if( ar[i] >= ar[i+4] ) return false;
	}
	return true;
}

void gen_permutation(int ix) {
	if( ix == N_PLAYERS - 2 ) {	//	最後の２要素の場合
		bool dec = g_ar[ix] > g_ar[ix+1];
		if( dec )
			swap(g_ar[ix], g_ar[ix+1]);		//	ペアを昇順に
		//for(int i = 0; i < N_PLAYERS; ++i) cout << g_ar[i]+1 << " "; cout << endl;
		++g_cnt;
		if( dec )
			swap(g_ar[ix], g_ar[ix+1]);		//	下に戻す
		return;
	}
	if( (ix&3) == 0 ) {			//	各コート最初のプレイヤー
		int minix = ix;
		int minpid = g_ar[ix];
		for(int i = ix+1; i < N_PLAYERS; ++i) {
			if( g_ar[i] < minpid ) {
				minpid = g_ar[i];
				minix = i;
			}
		}
		if( minix != ix )
			swap(g_ar[ix], g_ar[minix]);
		gen_permutation(ix+1);
		if( minix != ix )
			swap(g_ar[ix], g_ar[minix]);
		return;
	}
	for(int i = ix; i < N_PLAYERS; ++i) {
		swap(g_ar[ix], g_ar[i]);
		if( ((ix&1)==0 || g_ar[ix-1] < g_ar[ix]) /*&&		//	ペアは昇順か？
			((ix&3)!=2 || g_ar[ix-2] < g_ar[ix])*/ )		//	対戦ペアは昇順か？
		{	
			gen_permutation(ix+1);
		}
		swap(g_ar[ix], g_ar[i]);
	}
}
void gen_permutation(Bitmap bits, int ix) {
	if( ix == N_PLAYERS - 2 ) {			//	最後の２要素の場合
		auto t = bits & -bits;		//	最小IDプレイヤー
		g_ar[ix] = __popcnt(t-1);
		g_ar[ix+1] = __popcnt((bits^t)-1);	//	残りのプレイヤー
		//for(int i = 0; i < N_PLAYERS; ++i) cout << (g_ar[i]+1) << " "; cout << endl;
		++g_cnt;
		return;
	}
	if( (ix&3) == 0 ) {			//	各コート最初のプレイヤー
		auto t = bits & -bits;		//	最小IDプレイヤー
		g_ar[ix] = __popcnt(t-1);
		gen_permutation(bits^t, ix+1);
		return;
	}
	auto b = bits;
	while( b != 0 ) {
		auto t = b & -b;		//	最小IDプレイヤー
		g_ar[ix] = __popcnt(t-1);
		b ^= t;			//	最小IDプレイヤー削除
		if( ((ix&1)==0 || g_ar[ix-1] < g_ar[ix]) /*&&		//	ペアは昇順か？
			((ix&3)!=2 || g_ar[ix-2] < g_ar[ix])*/ )		//	対戦ペアは昇順か？
		{	
			gen_permutation(bits^t, ix+1);		//	bits^t：使用済みIDビットをOFFに
		}
	}
}

int main()
{
    //vector<PlayerID> ar(N_PLAYERS);
    std::chrono::system_clock::time_point  start, end;
    cout << "N_PLAYERS = " << N_PLAYERS << endl << endl;

//----------------------------------------------------------------------

    if( N_PLAYERS <= 12 ) {
	    cout << endl << "next_permutation():" << endl;
	    iota(g_ar.begin(), g_ar.end(), 0);
	    start = std::chrono::system_clock::now(); // 計測開始時間
	    g_cnt = 0;
	    do {
	        if( is_valid(g_ar) )
		        ++g_cnt;
	    } while (next_permutation(g_ar.begin(), g_ar.end()));
	    end = std::chrono::system_clock::now();  // 計測終了時間
	    double dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
	    cout << "duration = " << dur << " msec" << endl;
	    cout << "cnt = " << g_cnt << endl;
    }

//----------------------------------------------------------------------

    if( true ) {
	    cout << endl << "gen_permutation():" << endl;
	    iota(g_ar.begin(), g_ar.end(), 0);
	    start = std::chrono::system_clock::now(); // 計測開始時間
	    g_cnt = 0;
	    gen_permutation(1);
	    end = std::chrono::system_clock::now();  // 計測終了時間
	    double dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
	    cout << "duration = " << dur << " msec" << endl;
	    cout << "cnt = " << g_cnt << endl;
    }

//----------------------------------------------------------------------

    if( true ) {
	    cout << endl << "bitmap:" << endl;
	    Bitmap bits = (1<<N_PLAYERS) - 1;					//	利用可能 bits
	    start = std::chrono::system_clock::now(); // 計測開始時間
	    g_cnt = 0;
	    gen_permutation(bits, 0);
	    end = std::chrono::system_clock::now();  // 計測終了時間
	    double dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
	    cout << "duration = " << dur << " msec" << endl;
	    cout << "cnt = " << g_cnt << endl;
    }

//----------------------------------------------------------------------

    std::cout << "\nOK.\n";
}
