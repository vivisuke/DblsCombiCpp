#pragma once

#include <vector>

//typedef unsigned char uchar;
//typedef unsigned short ushort;
//using PlayerId = unsigned char;
using uchar = unsigned char;
using ushort = unsigned short;
using PlayerId = short;						//	0オリジン

//	各ラウンドでの試合組み合わせ
struct Round {
	std::vector<PlayerId>	m_playing;		//	休憩中プレイヤーリスト
	std::vector<PlayerId>	m_resting;		//	休憩中プレイヤーリスト
public:
	void	print() const;
	void	build_first_round(int num_courts, int num_players);
};
//	  全ラウンド(Round)のリスト・組合せ情報
struct Schedule
{
	int		m_num_players;
	int		m_num_courts;
	int		m_num_resting;			//	１ラウンドで休憩するプレイヤー人数
	double	m_ave_oppo;				//	 平均対戦回数 （2*NRnd*(1-rest/n_players)/(n_players-1)）
	PlayerId	m_resting_pid;		//	休憩中プレイヤー（最小id）
	bool	m_rest_desc;			//	休憩：降順
	std::vector<Round>		m_rounds;	// 各ラウンド配列
	//std::vector<PlayerId>	m_plist;	//	ペア（ix, ix+1）配列
	std::vector<std::vector<ushort>>	m_pair_counts;		//	同じ相手と何回ペアを組んだか
	std::vector<std::vector<ushort>>	m_oppo_counts;		//	同じ相手と何回対戦したか
	Schedule(int num_courts, int num_players, bool rest_desc=true) {
		m_num_players = num_players;
		m_num_courts = num_courts;
		m_num_resting = num_players - num_courts * 4;	//	休憩人数
		m_resting_pid = num_courts * 4;			//	休憩中プレイヤー（最小id）
		m_rest_desc = rest_desc;
		build_first_round();
		count_pair_counts();
		count_oppo_counts();
	}
	void	print() const;
	void	build_first_round();
	void	init_pair_counts();				//	
	void	update_pair_counts(const Round&);			//	
	void	count_pair_counts();			//	各プレイヤーが同じ相手と何回ペアを組んだかを計算
	void	init_oppo_counts();
	//double	eval_pairs(const std::vector<Pair>&);
	//void	update_oppo_counts(const std::vector<Pair>&);			//	
	//void	undo_oppo_counts(const std::vector<Pair>&);			//	
	void	count_oppo_counts();				//	各プレイヤーが同じ相手と何回対戦したかを計算
};

