#ifndef _ANIMALS_H
	#define _ANIMALS_H
	
/*#define A_RABBIT 0x7F
#define A_WOLF_M '@'
#define A_WOLF_F '#'
#define A_VOID '.'*/

enum Animals_ID {
	A_VOID,
	A_RABBIT,
	A_WOLF_M,
	A_WOLF_F
};

typedef struct {
		char type;
		int x;
		int y;
		int health;
		int isMoved;
} Animals;

typedef Animals* pAnimals;

const char Animal_symbols[] = { '.', 0x7F, '@', '#' };
const int Animal_colors[] = { 0, 12, 14, 10 };
void (*animal_turn)(pAnimals, pAnimals, int, int);

struct Anim_counter {
	int rab_cnt;
	int wlf_m_cnt;
	int wlf_f_cnt;
};
	
#endif // !_ANIMALS_H
