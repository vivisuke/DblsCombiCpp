﻿#pragma once

#include <vector>

//typedef unsigned char uchar;
//typedef unsigned short ushort;
//using PlayerId = unsigned char;
using cchar = const char;
using uchar = unsigned char;
using ushort = unsigned short;
using PlayerId = short;						//	0オリジン
using Bitmap = int;

//	各ラウンドでの試合組み合わせ
struct Round {
	std::vector<PlayerId>	m_playing;		//	非休憩中プレイヤーリスト
	std::vector<PlayerId>	m_resting;		//	休憩中プレイヤーリスト
public:
	void	print() const;
	void	build_first_round(int num_courts, int num_players);
};
//	  全ラウンド(Round)のリスト・組合せ情報
struct Schedule
{
public:
	int		m_count;
	int		m_num_players;
	int		m_num_courts;
	int		m_num_resting;			//	１ラウンドで休憩するプレイヤー人数
	double	m_std;
	double	m_minev;
	std::vector<PlayerId> m_plist;		//	最良組み合わせ
	std::vector<PlayerId> m_bestlst;	//	最良組み合わせ
	std::vector<std::vector<PlayerId>> m_bestlstlst;
	double	m_ave_oppo;				//	 平均対戦回数 （2*NRnd*(1-rest/n_players)/(n_players-1)）
	PlayerId	m_resting_pid;		//	休憩中プレイヤー（最小id）
	bool	m_rest_desc;			//	休憩：降順
	std::vector<Round>		m_rounds;	// 各ラウンド配列
	//std::vector<PlayerId>	m_plist;	//	ペア（ix, ix+1）配列
	std::vector<std::vector<ushort>>	m_pair_counts;		//	同じ相手と何回ペアを組んだか
	mutable int		m_n_oc_zero;				//	非対戦回数
	std::vector<std::vector<ushort>>	m_oppo_counts;		//	同じ相手と何回対戦したか
public:
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
	int		get_n_oc_zero() const { return m_n_oc_zero; }
	void	print() const;
	std::string	to_HTML() const;
	void	print_pair_counts() const;
	void	print_oppo_counts() const;
	void	make_not_resting_players_list(std::vector<PlayerId>&);		//	非休憩プレイヤーリスト取得
	Bitmap	make_not_resting_players_list();							//	非休憩プレイヤーリスト取得
	bool	search_balanced_pairs(std::vector<PlayerId>&, int, int);		//	なるべく重複しないペアを求める
	void	build_first_round();
	void	init_pair_counts();				//	
	void	update_pair_counts(const Round&);			//	
	void	update_oppo_counts(const Round&);			//	
	void	count_pair_counts();			//	各プレイヤーが同じ相手と何回ペアを組んだかを計算
	void	init_oppo_counts();
	//double	eval_pairs(const std::vector<Pair>&);
	void	update_oppo_counts(const std::vector<PlayerId>&);			//	
	void	undo_oppo_counts(const std::vector<PlayerId>&);			//	
	void	count_oppo_counts();				//	各プレイヤーが同じ相手と何回対戦したかを計算
	void	calc_pair_counts_ave_std(double&, double&) const;
	void	calc_oppo_counts_ave_std(double&, double&) const;
	//void	calc_pair_oppo_ave_std(double&, double&) const;			//	pair_counts + oppo_counts の平均・標準偏差
	double	calc_pair_oppo_counts_std() const;		//	pair_counts + oppo_counts の標準偏差計算
	double	calc_oppo_counts_std() const {
		double ave, std;
		calc_oppo_counts_ave_std(ave, std);
		return std;
	}

	void	shuffle_corts(std::vector<PlayerId>&);		//	コート単位でシャフル
	void	gen_permutation(std::vector<PlayerId>&, int);		//	再帰的に順列生成
	void	gen_permutation_BM(Bitmap, std::vector<PlayerId>&, int);		//	再帰的に順列生成、ビット演算版
	double	eval_balance_score();			//	目的関数、0 ならば偏り無し
	bool	is_legal(const std::vector<PlayerId>&);
	bool	is_pair_balanced(const std::vector<PlayerId>&);
	bool	is_normalized(const std::vector<PlayerId>&);
	void	add_round(cchar*&);
	void	add_random_round();
	void	add_balanced_pair_round();
	void	add_balanced_round();
	void	add_balanced_round_BM();		//	bitmap 使用版
	void	add_greedy_balanced_round();
};

