#include <iostream>
#include <random>
#include "Schedule.h"

using namespace std;

const double W1 = 100.0;
const double W2 = 1.0;

std::mt19937 rgen(std::random_device{}()); // シードを設定
//std::mt19937 rgen(0); // シードを設定

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
	double ave, std;
	calc_pair_counts_ave_std(ave, std);
	printf("ave = %.3f, std = %.3f\n", ave, std);
	cout << endl;
}
void Schedule::print_oppo_counts() const {
	cout << "oppo counts:" << endl;
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
	double ave, std;
	calc_oppo_counts_ave_std(ave, std);
	printf("ave = %.3f, std = %.3f\n", ave, std);
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
void Schedule::update_oppo_counts(const Round& round) {
	for(int i = 0; i < round.m_playing.size(); i += 4) {
		m_oppo_counts[round.m_playing[i]][round.m_playing[i+2]] += 1;
		m_oppo_counts[round.m_playing[i]][round.m_playing[i+3]] += 1;
		m_oppo_counts[round.m_playing[i+1]][round.m_playing[i+2]] += 1;
		m_oppo_counts[round.m_playing[i+1]][round.m_playing[i+3]] += 1;
		m_oppo_counts[round.m_playing[i+2]][round.m_playing[i]] += 1;
		m_oppo_counts[round.m_playing[i+3]][round.m_playing[i]] += 1;
		m_oppo_counts[round.m_playing[i+2]][round.m_playing[i+1]] += 1;
		m_oppo_counts[round.m_playing[i+3]][round.m_playing[i+1]] += 1;
	}
}
void Schedule::count_oppo_counts() {		//	各プレイヤーが同じ相手と何回対戦したかを計算
	init_oppo_counts();
	for(const auto& round: m_rounds) {
		update_oppo_counts(round);
	}
}
void Schedule::calc_pair_counts_ave_std(double& ave, double& std) const {
	int sum = 0;
	int sum2 = 0;
	for(int i = 0; i != m_num_players; ++i) {
		for(int k = 0; k != m_num_players; ++k) {
			if( k != i ) {
				sum += m_pair_counts[i][k];
				sum2 += m_pair_counts[i][k] * m_pair_counts[i][k];
			}
		}
	}
	int n = (m_num_players - 1) * m_num_players;
	ave = (double)sum / n;
	std = sqrt((double)sum2/n - ave*ave);
}
void Schedule::calc_oppo_counts_ave_std(double& ave, double& std) const {
	int sum = 0;
	int sum2 = 0;
	for(int i = 0; i != m_num_players; ++i) {
		for(int k = 0; k != m_num_players; ++k) {
			if( k != i ) {
				sum += m_oppo_counts[i][k];
				sum2 += m_oppo_counts[i][k] * m_oppo_counts[i][k];
			}
		}
	}
	int n = (m_num_players - 1) * m_num_players;
	ave = (double)sum / n;
	std = sqrt((double)sum2/n - ave*ave);
}

void Schedule::make_not_resting_players_list(vector<PlayerId>& lst) {
	lst.clear();
	for(int i = 0; i != m_num_players; ++i) {
		int last = m_resting_pid + m_num_resting;
		if( last < m_num_players ) {
			if( i < m_resting_pid || i >= last ) {
				lst.push_back(i);
			}
		} else {
			last %= m_num_players;
			if( i < m_resting_pid && i >= last ) {
				lst.push_back(i);
			}
		}
	}
}
bool Schedule::search_balanced_pairs(vector<PlayerId>& plist, int pcnt, int ix) {		//	pcnt: ペア回数上限
	if (ix >= plist.size())  return true;
	auto p1 = plist[ix];
	for (int ix2 = ix + 1; ix2 < plist.size(); ++ix2) {
		if (m_pair_counts[p1][plist[ix2]] < pcnt) {		//	ペア回数が上限以下の場合
			if (ix2 != ix + 1) swap(plist[ix + 1], plist[ix2]);
			if (search_balanced_pairs(plist, pcnt, ix + 2))
				return true;
			if (ix2 != ix + 1) swap(plist[ix + 1], plist[ix2]);
		}
	}
	return false;
}
//	ペア・対戦相手をランダムに決める
void Schedule::add_random_round() {
	m_resting_pid -= m_num_resting;
	if( m_resting_pid < 0 ) m_resting_pid += m_num_players;
	m_rounds.resize(m_rounds.size() + 1);
	auto& round = m_rounds.back();
	make_not_resting_players_list(round.m_playing);
	shuffle(round.m_playing.begin(), round.m_playing.end(), rgen);
	round.m_resting.clear();
	for(int i = 0; i < m_num_resting; ++i)
		round.m_resting.push_back((m_resting_pid + i) % m_num_players);
	update_pair_counts(round);
	update_oppo_counts(round);
}
//	ペアはバランスさせ、対戦相手はランダムに決める
void Schedule::add_balanced_pair_round() {
	m_resting_pid -= m_num_resting;
	if( m_resting_pid < 0 ) m_resting_pid += m_num_players;
	m_rounds.resize(m_rounds.size() + 1);
	auto& round = m_rounds.back();
	make_not_resting_players_list(round.m_playing);
	shuffle(round.m_playing.begin(), round.m_playing.end(), rgen);
	auto& plist = round.m_playing;
	for(int pcnt = 1; ;++pcnt) {
		if( search_balanced_pairs(plist, pcnt, 0) )	//	組んでいないペアを見つける、結果は plist に反映
			break;
	}
	round.m_resting.clear();
	for(int i = 0; i < m_num_resting; ++i)
		round.m_resting.push_back((m_resting_pid + i) % m_num_players);
	update_pair_counts(round);
	update_oppo_counts(round);
}
double Schedule::eval_balance_score() {
	double ave1, std1;
	double ave2, std2;
	calc_pair_counts_ave_std(ave1, std1);
	calc_oppo_counts_ave_std(ave2, std2);
	return W1 * std1 + W2 * std2;
}

