#ifndef _ANIMALS_H
	#define _ANIMALS_H
	
#define A_RABBIT 0x7F
#define A_WOLF_M '@'
#define A_WOLF_F '#'
#define A_VOID '.'


struct Animals {
		char type;
		int x;
		int y;
		int health;
		int isMoved;
};

struct Anim_counter {
	int rab_cnt;
	int wlf_m_cnt;
	int wlf_f_cnt;
};
	
#endif // !_ANIMALS_H
