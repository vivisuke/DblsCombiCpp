#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include "Schedule.h"

using namespace std;

#define		is_empty()	empty()

const double W1 = 100.0;
const double W2 = 1.0;
const double W3 = 0.4;

std::mt19937 rgen(std::random_device{}()); // ランダムシード設定
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
std::string Schedule::to_HTML() const {
	string html = "<!DOCTYPE html>\n";
	html += "<html lang=\"ja\">";
	html += "<head>\n";
	html += "\t<meta charset=\"UTF-8\">\n";
	html += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	html += "\t<title>Tennis Double Combination Table</title>";
	html += "\t<style>\n";
	html += "\ttable {border: 3px solid black; border-collapse: collapse;}\n";
	html += "\ttr,th,td {border: 1px solid black; padding: 2px; text-align: center; font-size: 40px;}\n";
	html += "\t</style>\n";
	html += "</head>\n";
	html += "<body>\n";
	string ttl = to_string(m_num_courts) + u8"面 " + to_string(m_num_players) + u8"人";
	auto oppo_std = calc_oppo_counts_std();
	//ttl += " (STD = " + to_string(oppo_std).substr(0, 5) + ")";
	char buf[8];
	sprintf_s(buf, "%.3f", oppo_std);
	ttl += " (STD = " + string(buf) + u8" 未対戦数 = " + to_string(m_n_oc_zero) + ")";
	html += "<h1 align=center>" + ttl + "</h1>\n";
	html += "<table width=100%>\n";
	html += "<tr>\n";
	html += "\t<td>R</td>\n";
	for(int c = 0; c != m_num_courts; ++c) {
		html += "\t<th>#" + to_string(c+1) + "</th>\n";
	}
	if( m_num_resting != 0 ) {
		html += u8"\t<th>休憩</th>\n";
	}
	html += "</tr>\n";
	int r = 0;
	for(const auto round: m_rounds) {
		html += "<tr>\n";
		html += "\t<td>" + to_string(++r) + "</td>\n";
		for(int c = 0; c < m_num_courts; ++c) {
			int ix = c * 4;
			html += "\t<td>" +
						to_string(round.m_playing[ix]+1) + ", " +
						to_string(round.m_playing[ix+1]+1) + " - " +
						to_string(round.m_playing[ix+2]+1) + ", " +
						to_string(round.m_playing[ix+3]+1) +
					"</td>";
		}
		html += "\n";
		if( m_num_resting != 0 ) {
			html += "\t<td>";
			for(int i = 0; i < m_num_resting; ++i) {
				html += to_string(round.m_resting[i]+1) + " ";
			}
			html += "</td>\n";
		}
		html += "</tr>\n";
	}
	html += "</table>\n";
	html += "</body>\n";
	html += "</html>\n";
	return html;
}
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
	int zc = 0;
	int pid = 0;
	for(int p1 = 0; p1 < m_num_players; ++p1) {
		printf("%3d: " , ++pid);
		for(int p2 = 0; p2 < m_num_players; ++p2) {
			if( p1 != p2 ) {
				if( m_oppo_counts[p1][p2] == 0 ) ++zc;
				cout << m_oppo_counts[p1][p2] << " ";
			} else
				cout << "- ";
		}
		cout << endl;
	}
	double ave, std;
	calc_oppo_counts_ave_std(ave, std);
	printf("ave = %.3f, std = %.3f, zero = %d\n", ave, std, zc);
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
void Schedule::update_oppo_counts(const std::vector<PlayerId>& plist) {
	for(int i = 0; i < plist.size(); i += 4) {
		m_oppo_counts[plist[i]][plist[i+2]] += 1;
		m_oppo_counts[plist[i]][plist[i+3]] += 1;
		m_oppo_counts[plist[i+1]][plist[i+2]] += 1;
		m_oppo_counts[plist[i+1]][plist[i+3]] += 1;
		m_oppo_counts[plist[i+2]][plist[i]] += 1;
		m_oppo_counts[plist[i+3]][plist[i]] += 1;
		m_oppo_counts[plist[i+2]][plist[i+1]] += 1;
		m_oppo_counts[plist[i+3]][plist[i+1]] += 1;
	}
}
void Schedule::undo_oppo_counts(const std::vector<PlayerId>& plist) {
	for(int i = 0; i < plist.size(); i += 4) {
		m_oppo_counts[plist[i]][plist[i+2]] -= 1;
		m_oppo_counts[plist[i]][plist[i+3]] -= 1;
		m_oppo_counts[plist[i+1]][plist[i+2]] -= 1;
		m_oppo_counts[plist[i+1]][plist[i+3]] -= 1;
		m_oppo_counts[plist[i+2]][plist[i]] -= 1;
		m_oppo_counts[plist[i+3]][plist[i]] -= 1;
		m_oppo_counts[plist[i+2]][plist[i+1]] -= 1;
		m_oppo_counts[plist[i+3]][plist[i+1]] -= 1;
	}
}
void Schedule::update_oppo_counts(const Round& round) {
	update_oppo_counts(round.m_playing);
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
	m_n_oc_zero = 0;
	int sum = 0;
	int sum2 = 0;
	for(int i = 0; i != m_num_players; ++i) {
		for(int k = 0; k != m_num_players; ++k) {
			if( k != i ) {
				if( m_oppo_counts[i][k] != 0 ) {
					sum += m_oppo_counts[i][k];
					sum2 += m_oppo_counts[i][k] * m_oppo_counts[i][k];
				} else
					++m_n_oc_zero;
			}
		}
	}
	int n = (m_num_players - 1) * m_num_players;
	ave = (double)sum / n;
	std = sqrt((double)sum2/n - ave*ave);
}
double Schedule::calc_pair_oppo_counts_std() const {
	int sum = 0;
	int sum2 = 0;
	for(int i = 0; i != m_num_players; ++i) {
		for(int k = 0; k != m_num_players; ++k) {
			if( k != i ) {
				auto d = m_pair_counts[i][k] + m_oppo_counts[i][k];
				sum += d;
				sum2 += d * d ;
			}
		}
	}
	int n = (m_num_players - 1) * m_num_players;
	auto ave = (double)sum / n;
	return sqrt((double)sum2/n - ave*ave);
}

void Schedule::make_not_resting_players_list(vector<PlayerId>& lst) {	//	非休憩プレイヤーリストを取得
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
Bitmap Schedule::make_not_resting_players_list() {	//	非休憩プレイヤーリストを取得
	Bitmap bits = 0;
	Bitmap b = 1;
	for(int i = 0; i != m_num_players; ++i, b<<=1) {
		int last = m_resting_pid + m_num_resting;
		if( last < m_num_players ) {
			if( i < m_resting_pid || i >= last ) {
				bits |= b;
			}
		} else {
			last %= m_num_players;
			if( i < m_resting_pid && i >= last ) {
				bits |= b;
			}
		}
	}
	return bits;
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
	make_not_resting_players_list(round.m_playing);	//	非休憩プレイヤーリストを取得
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
	auto& plist = round.m_playing;
	make_not_resting_players_list(plist);
	shuffle(plist.begin(), plist.end(), rgen);
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
	//return W1 * std1 + W2 * std2;
	return W1 * std1 + W2 * std2 + W3 * calc_pair_oppo_counts_std();
}
bool Schedule::is_legal(const std::vector<PlayerId>& plist) {
	assert( !plist.empty() );			//	plist は非空
	assert( plist.size() % 4 == 0 );	//	plist.size() は４の倍数
	for(int i = 0; i < plist.size(); i+=2) {
		if( m_pair_counts[plist[i]][plist[i+1]] > 0 ) return false;		//	同一ペアは不可
		if( plist[i] >= plist[i+1] ) return false;		//	ペアは昇順
	}
	for(int i = 0; i < plist.size(); i+=4) {
		if( plist[i] >= plist[i+2] ) return false;		//	ペア最小IDプレイヤーも昇順
	}
	for(int i = 0; i < plist.size()-4; i+=4) {
		if( plist[i] >= plist[i+4] ) return false;		//	各コート最小IDプレイヤーも昇順
	}
	return true;
}
bool Schedule::is_pair_balanced(const std::vector<PlayerId>& plist) {
	for(int i = 0; i < plist.size(); i+=2) {
		if( m_pair_counts[plist[i]][plist[i+1]] > 0 ) return false;		//	同一ペアは不可
	}
	return true;
}
bool Schedule::is_normalized(const std::vector<PlayerId>& plist) {
	assert( !plist.empty() );			//	plist は非空
	assert( plist.size() % 4 == 0 );	//	plist.size() は４の倍数
	for(int i = 0; i < plist.size(); i+=2) {
		if( plist[i] >= plist[i+1] ) return false;		//	ペアは昇順
	}
	for(int i = 0; i < plist.size(); i+=4) {
		if( plist[i] >= plist[i+2] ) return false;		//	ペア最小IDプレイヤーも昇順
	}
	for(int i = 0; i < plist.size()-4; i+=4) {
		if( plist[i] >= plist[i+4] ) return false;		//	各コート最小IDプレイヤーも昇順
	}
	return true;
}
void Schedule::gen_permutation_BM(Bitmap bits, vector<PlayerId>& plist, int ix) {
	if( ix == plist.size() - 2 ) {		//	残り２要素
		auto t = bits & -bits;		//	最小IDプレイヤー
		plist[ix] = __popcnt(t-1);
		plist[ix+1] = __popcnt((bits^t)-1);	//	残りのプレイヤー
		//assert( plist[ix-4] < plist[ix] );
		++m_count;
		if( is_pair_balanced(plist) ) {
			update_oppo_counts(plist);
			//auto ev = calc_oppo_counts_std();
			auto ev = eval_balance_score();
			undo_oppo_counts(plist);
			if( ev < m_minev ) {
				m_minev = ev;
				m_bestlst = plist;
				m_bestlstlst.clear();
				m_bestlstlst.push_back(plist);
			//} else if( ev == m_minev && m_bestlstlst.size() < 100 ) {
			//	for(int i = 0; i < plist.size(); i+=2) {
			//		assert( plist[i] < plist[i+1] );
			//	}
			//	m_bestlstlst.push_back(plist);
			}
		}
		return;
	}
	if( (ix&3) == 0 ) {		//	ix が４の倍数 → 以降の最小値を plist[ix] に 
		auto t = bits & -bits;		//	最小IDプレイヤー
		plist[ix] = __popcnt(t-1);
		gen_permutation_BM(bits^t, plist, ix+1);
		return;
	}
	auto b = bits;
	while( b != 0 ) {
		auto t = b & -b;		//	最小IDプレイヤー
		plist[ix] = __popcnt(t-1);
		b ^= t;			//	最小IDプレイヤー削除
		if( ((ix&3)!=3 || plist[ix-1] < plist[ix]) &&		//	ペアは昇順か？
			((ix&3)!=3 || m_oppo_counts[plist[ix-3]][plist[ix-1]]<2 &&		//	対戦は２回未満？
							m_oppo_counts[plist[ix-3]][plist[ix]]<2 &&
							m_oppo_counts[plist[ix-2]][plist[ix]]<2 &&
							m_oppo_counts[plist[ix-2]][plist[ix]]<2) &&
			((ix&1) != 1 || m_pair_counts[plist[ix-1]][plist[ix]] == 0) )	//	未ペアチェック
		{
			gen_permutation_BM(bits^t, plist, ix+1);		//	bits^t：使用済みIDビットをOFFに
		}
	}
}
void Schedule::gen_permutation(vector<PlayerId>& plist, int ix) {
	if( ix == plist.size() - 2 ) {		//	残り２要素
		const bool dec = plist[ix] > plist[ix+1];
		if( dec )
			swap(plist[ix], plist[ix+1]);
		//if( plist[ix-4] < plist[ix] ) {
			++m_count;
			if( is_pair_balanced(plist) ) {
				update_oppo_counts(plist);
				//auto ev = calc_oppo_counts_std();
				auto ev = eval_balance_score();
				undo_oppo_counts(plist);
				if( ev < m_minev ) {
					m_minev = ev;
					m_bestlst = plist;
					m_bestlstlst.clear();
					m_bestlstlst.push_back(plist);
				//} else if( ev == m_minev && m_bestlstlst.size() < 100 ) {
				//	for(int i = 0; i < plist.size(); i+=2) {
				//		assert( plist[i] < plist[i+1] );
				//	}
				//	m_bestlstlst.push_back(plist);
				}
			}
		//}
		if( dec )
			swap(plist[ix], plist[ix+1]);
		return;
	}
	if( (ix&3) == 0 ) {		//	ix が４の倍数 → 以降の最小値を plist[ix] に 
		auto minval = plist[ix];
		auto minix = ix;
		for(int i = ix + 1; i < plist.size(); ++i) {
			if( plist[i] < minval ) {
				minval = plist[i];
				minix = i;
			}
		}
		if( minix != ix )
			swap(plist[ix], plist[minix]);
		gen_permutation(plist, ix+1);
		if( minix != ix )
			swap(plist[ix], plist[minix]);
		return;
	}
#if 0
	if( ix == plist.size() - 1 ) {
		if( plist[ix-1] < plist[ix] ) {
			for (int i = 0; i < plist.size(); i += 2) {
				assert(plist[i] < plist[i + 1]);
			}
			++m_count;
			if( is_pair_balanced(plist) ) {
				update_oppo_counts(plist);
				//auto ev = calc_oppo_counts_std();
				auto ev = eval_balance_score();
				undo_oppo_counts(plist);
				if( ev < m_minev ) {
					m_minev = ev;
					m_bestlst = plist;
					m_bestlstlst.clear();
					m_bestlstlst.push_back(plist);
				} else if( ev == m_minev ) {
					for(int i = 0; i < plist.size(); i+=2) {
						assert( plist[i] < plist[i+1] );
					}
					m_bestlstlst.push_back(plist);
				}
			}
		}
		return;
	}
#endif
	for(int dst = ix; dst < plist.size(); ++dst) {
		swap(plist[ix], plist[dst]);
		//	plist[0] ～ plist[ix] が確定
		if( ((ix&1) != 1 || plist[ix-1] < plist[ix]) &&				//	ペアは昇順
			((ix&1) != 1 || m_pair_counts[plist[ix-1]][plist[ix]] == 0) &&		//	未ペア
			((ix&3)!=2 || plist[ix-2] < plist[ix]) /*&&				//	各ペアの最小IDが昇順
			(ix == 0 || (ix&3)!=0 || plist[ix-4] < plist[ix])*/ )		//	各コートの最小IDが昇順
		{
			for(int i = 1; i <= ix; i+=2) {
				assert( plist[i-1] < plist[i] );
			}
			gen_permutation(plist, ix+1);
		}
		swap(plist[ix], plist[dst]);
	}
}
void Schedule::shuffle_corts(vector<PlayerId>& plist) {		//	コート単位でシャフル
	vector<int> ixlst(m_num_courts);
	iota(ixlst.begin(), ixlst.end(), 0);
	shuffle(ixlst.begin(), ixlst.end(), rgen);
	auto lst0 = plist;
	for(int c = 0; c < m_num_courts; ++c) {
		int ix4 = ixlst[c] * 4;
		plist[c*4] = lst0[ix4];
		plist[c*4+1] = lst0[ix4+1];
		plist[c*4+2] = lst0[ix4+2];
		plist[c*4+3] = lst0[ix4+3];
	}

}
//	ペア・対戦相手をバランスさせた組み合わせ追加
void Schedule::add_balanced_round() {
	// 休憩プレイヤーIDの更新
	m_resting_pid -= m_num_resting;
	if( m_resting_pid < 0 ) m_resting_pid += m_num_players;
	// 新しいラウンドを追加
	//m_rounds.resize(m_rounds.size() + 1);
	m_rounds.emplace_back();
	auto& round = m_rounds.back();
	auto& plist = round.m_playing;
	// 非休憩プレイヤーリストの取得とシャッフル
	make_not_resting_players_list(plist);	//	非休憩プレイヤーリストを取得
#if 1
	m_minev = INT_MAX;		// 最小評価値 (ここでは標準偏差)
	m_bestlstlst.clear();
	//vector<PlayerId> bestlst;	// 最良のプレイヤーリスト
	// 全ての順列を試行して最適な組み合わせを見つける (総当たり)
	m_count = 0;
#if 1
	gen_permutation(plist, 1);		//	再帰的に順列生成, 1 for 最初は最小番号固定
#else
	do {
		if( is_normalized(plist) ) {	// 有効な組み合わせかチェック
			++m_count;
			if( is_pair_balanced(plist) ) {
#if 1
				update_oppo_counts(plist);
				auto ev = calc_oppo_counts_std();
				undo_oppo_counts(plist);
				if( ev < m_minev ) {
					m_minev = ev;
					m_bestlst = plist;
				}
#endif
			}
		}
	} while( next_permutation(plist.begin(), plist.end()) );
#endif
	//assert(cnt == 315);
	cout << "m_count = " << m_count << endl;
#else
	shuffle(plist.begin(), plist.end(), rgen);
	//
	auto plist0 = plist;				// シャッフル後のリストを保持 (順列生成のベース)
	vector<int> ixlst(plist.size());
	//for(int i = 0; i != ixlst.size(); ++i) ixlst[i] = i;	// 0, 1, 2, ... のインデックスリスト
	iota(ixlst.begin(), ixlst.end(), 0);
	int minev = INT_MAX;		// 最小評価値 (ここでは標準偏差)
	vector<PlayerId> bestlst;	// 最良のプレイヤーリスト
	// 全ての順列を試行して最適な組み合わせを見つける (総当たり)
	do {
		//	ixlst[] の順序で plist0 から plist を構築
		for(int k = 0; k != ixlst.size(); ++k) plist[k] = plist0[ixlst[k]];
		if( is_legal(plist) ) {	// 有効な組み合わせかチェック
			update_oppo_counts(plist);
			auto ev = calc_oppo_counts_std();
			undo_oppo_counts(plist);
			if( ev < minev ) {
				minev = ev;
				bestlst = plist;
			}
		}
	} while( next_permutation(ixlst.begin(), ixlst.end()) );
#endif
	if( !m_bestlstlst.is_empty() ) {
		plist = m_bestlstlst[rgen() % m_bestlstlst.size()];
	} else
		plist = m_bestlst;
	shuffle_corts(plist);
	round.m_resting.clear();
	for(int i = 0; i < m_num_resting; ++i)
		round.m_resting.push_back((m_resting_pid + i) % m_num_players);
	update_pair_counts(round);
	update_oppo_counts(round);
}
//	ペア・対戦相手をバランスさせた組み合わせ追加
//	ビット演算使用版
void Schedule::add_balanced_round_BM() {
	// 休憩プレイヤーIDの更新
	m_resting_pid -= m_num_resting;
	if( m_resting_pid < 0 ) m_resting_pid += m_num_players;
	// 新しいラウンドを追加
	//m_rounds.resize(m_rounds.size() + 1);
	m_rounds.emplace_back();
	auto& round = m_rounds.back();
	auto& plist = round.m_playing;
	plist.resize(m_num_courts*4);
	// 非休憩プレイヤーリストの取得とシャッフル
	Bitmap bits = make_not_resting_players_list();	//	非休憩プレイヤーリストを取得
	m_minev = INT_MAX;		// 最小評価値 (ここでは標準偏差)
	m_bestlstlst.clear();
	//vector<PlayerId> bestlst;	// 最良のプレイヤーリスト
	// 全ての順列を試行して最適な組み合わせを見つける (総当たり)
	m_count = 0;
	gen_permutation_BM(bits, plist, 0);		//	再帰的に順列生成
	//assert(cnt == 315);
	cout << "m_count = " << m_count << endl;
	if( !m_bestlstlst.is_empty() ) {
		plist = m_bestlstlst[rgen() % m_bestlstlst.size()];
	} else
		plist = m_bestlst;
	shuffle_corts(plist);
	round.m_resting.clear();
	for(int i = 0; i < m_num_resting; ++i)
		round.m_resting.push_back((m_resting_pid + i) % m_num_players);
	update_pair_counts(round);
	update_oppo_counts(round);
}
int get_number(cchar* &ptr) {
	while( !isdigit(*ptr) ) {
		if( *ptr == '\0' ) return -1;
		++ptr;
	}
	int val = *ptr++ - '0';
	while( isdigit(*ptr) ) {
		val = val * 10 + *ptr++ - '0';
	}
	return val;
}
void Schedule::add_round(cchar *&ptr) {
	int cnt = 0;
	m_rounds.emplace_back();
	auto& round = m_rounds.back();
	round.m_resting.clear();
	while( *ptr != '\0' ) {
		auto v = get_number(ptr);
		//cout << v << " ";
		round.m_playing.push_back(v-1);
		if( ++cnt % 16 == 0 ) {
			//cout << endl;
			break;
		}
	}
	while( *ptr != '\0' && !isdigit(*ptr) ) ++ptr;
	update_pair_counts(round);
	update_oppo_counts(round);
}
