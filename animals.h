#ifndef _ANIMALS_H
	#define _ANIMALS_H
	
#define A_RABBIT 0x7F
#define A_WOLF_M '@'
#define A_WOLF_F '#'
#define A_VOID '.'


<<<<<<< Updated upstream
struct Animals {
		char type;
=======
typedef struct {
		int type;
>>>>>>> Stashed changes
		int x;
		int y;
		int health;
		int isMoved;
<<<<<<< Updated upstream
};
=======
} Animals;

typedef Animals* pAnimals;

const char Animal_symbols[] = { '.', 0x7F, '@', '#' };
const int Animal_colors[] = { 0, 12, 14, 10 };
void (*animal_turn[3])(pAnimals, pAnimals, int, int);
>>>>>>> Stashed changes

struct Anim_counter {
	int rab_cnt;
	int wlf_m_cnt;
	int wlf_f_cnt;
};
	
#endif // !_ANIMALS_H
