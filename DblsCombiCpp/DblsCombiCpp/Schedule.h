#pragma once

#include <vector>

//typedef unsigned char uchar;
//typedef unsigned short ushort;
//using PlayerId = unsigned char;
using uchar = unsigned char;
using ushort = unsigned short;
using PlayerId = short;						//	0�I���W��

//	�e���E���h�ł̎����g�ݍ��킹
struct Round {
	std::vector<PlayerId>	m_playing;		//	�x�e���v���C���[���X�g
	std::vector<PlayerId>	m_resting;		//	�x�e���v���C���[���X�g
public:
	void	print() const;
	void	build_first_round(int num_courts, int num_players);
};
//	  �S���E���h(Round)�̃��X�g�E�g�������
struct Schedule
{
	int		m_num_players;
	int		m_num_courts;
	int		m_num_resting;			//	�P���E���h�ŋx�e����v���C���[�l��
	double	m_ave_oppo;				//	 ���ϑΐ�� �i2*NRnd*(1-rest/n_players)/(n_players-1)�j
	PlayerId	m_resting_pid;		//	�x�e���v���C���[�i�ŏ�id�j
	bool	m_rest_desc;			//	�x�e�F�~��
	std::vector<Round>		m_rounds;	// �e���E���h�z��
	//std::vector<PlayerId>	m_plist;	//	�y�A�iix, ix+1�j�z��
	std::vector<std::vector<ushort>>	m_pair_counts;		//	��������Ɖ���y�A��g�񂾂�
	std::vector<std::vector<ushort>>	m_oppo_counts;		//	��������Ɖ���ΐ킵����
	Schedule(int num_courts, int num_players, bool rest_desc=true) {
		m_num_players = num_players;
		m_num_courts = num_courts;
		m_num_resting = num_players - num_courts * 4;	//	�x�e�l��
		m_resting_pid = num_courts * 4;			//	�x�e���v���C���[�i�ŏ�id�j
		m_rest_desc = rest_desc;
		build_first_round();
		count_pair_counts();
		count_oppo_counts();
	}
	void	print() const;
	void	build_first_round();
	void	init_pair_counts();				//	
	void	update_pair_counts(const Round&);			//	
	void	count_pair_counts();			//	�e�v���C���[����������Ɖ���y�A��g�񂾂����v�Z
	void	init_oppo_counts();
	//double	eval_pairs(const std::vector<Pair>&);
	//void	update_oppo_counts(const std::vector<Pair>&);			//	
	//void	undo_oppo_counts(const std::vector<Pair>&);			//	
	void	count_oppo_counts();				//	�e�v���C���[����������Ɖ���ΐ킵�������v�Z
};

