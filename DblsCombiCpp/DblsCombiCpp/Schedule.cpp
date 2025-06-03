#include <iostream>
#include "Schedule.h"

using namespace std;

//----------------------------------------------------------------------
void Round::print() const {
	for(int i = 0; i < m_playing.size(); i += 4) {
		//m_pairs[i].print();
		printf("%2d,%2d", m_playing[i]+1, m_playing[i+1]+1);
		cout << " : ";
		//m_pairs[i+1].print();
		printf("%2d,%2d", m_playing[i+2]+1, m_playing[i+3]+1);
		cout << " | ";
	}
	cout << "Rest: ";
	for(auto p: m_resting) {
		//cout << (int)p+1 << " ";
		printf("%3d", p+1);
	}
	cout << endl;
}
void Round::build_first_round(int num_courts, int num_players) {
	m_playing.resize(num_courts*4);
	PlayerId id = 0;
	for(auto& p: m_playing) {
		p = id++;
	}
	int pid = num_courts * 4;
	m_resting.clear();
	while( pid < num_players ) {
		m_resting.push_back(pid++);
	}
}
//----------------------------------------------------------------------
void Schedule::print() const {
	int i = 0;
	for(const auto& r: m_rounds) {
		printf("%2d: ", ++i);
		r.print();
	}
	cout << endl;
}
void Schedule::print_pair_counts() const {
	cout << "pair counts:" << endl;
	int pid = 0;
	for(int p1 = 0; p1 < m_num_players; ++p1) {
		printf("%3d: " , ++pid);
		for(int p2 = 0; p2 < m_num_players; ++p2) {
			if( p1 != p2 )
				cout << m_pair_counts[p1][p2] << " ";
			else
				cout << "- ";
		}
		cout << endl;
	}
	cout << endl;
}
void Schedule::print_oppo_counts() const {
	cout << "opponent counts:" << endl;
	int pid = 0;
	for(int p1 = 0; p1 < m_num_players; ++p1) {
		printf("%3d: " , ++pid);
		for(int p2 = 0; p2 < m_num_players; ++p2) {
			if( p1 != p2 )
				cout << m_oppo_counts[p1][p2] << " ";
			else
				cout << "- ";
		}
		cout << endl;
	}
	cout << endl;
}
void Schedule::build_first_round() {
	if( m_num_resting < 0 ) return;		//	人数・コート数エラーの場合
	m_rounds.resize(1);
	m_rounds[0].build_first_round(m_num_courts, m_num_players);
}
void Schedule::init_pair_counts() {
	m_pair_counts.resize(m_num_players);
	for(auto& v: m_pair_counts) {
		v.resize(m_num_players);
		for(auto& u: v) u = 0;
	}
}
void Schedule::update_pair_counts(const Round& round) {
	for(int i = 0; i < round.m_playing.size(); i += 2) {
		m_pair_counts[round.m_playing[i]][round.m_playing[i+1]] += 1;
		m_pair_counts[round.m_playing[i+1]][round.m_playing[i]] += 1;
	}
}
void Schedule::count_pair_counts() {		//	各プレイヤーが同じ相手と何回ペアを組んだかを計算
	init_pair_counts();
	for(const auto& round: m_rounds) {
		update_pair_counts(round);
	}
}
void Schedule::init_oppo_counts() {
	m_oppo_counts.resize(m_num_players);
	for(auto& v: m_oppo_counts) {
		v.resize(m_num_players);
		for(auto& u: v) u = 0;
	}
}
void Schedule::count_oppo_counts() {		//	各プレイヤーが同じ相手と何回対戦したかを計算
	init_oppo_counts();
}
