#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <Windows.h>

#include "animals.h"
#include "settings.h"

int randomize(int val1, int val2); /*Функция возвращает случайное целое число в диапазоне от val1 до val2 включительно*/
void generate_model(pAnimals anim_arr, struct Anim_counter anim_cnt, int map_len, int an_health);
void place_animals(pAnimals anim_arr, int cur_anim_cnt, int an_type, int map_len, int an_health);
void turn(pAnimals anim_arr, int map_len, int reprod_chance, int an_health);
void turn_rabbit(pAnimals cur_anim, pAnimals anim_arr, int map_len, int reprod_chance);
void turn_wolf_m(pAnimals cur_anim, pAnimals anim_arr, int map_len, int health);
void turn_wolf_f(pAnimals cur_anim, pAnimals anim_arr, int map_len, int health);
void check_near_Anims(Animals cur_anim, pAnimals anim_arr, int *id_arr, int map_len);
void move_animal(pAnimals cur_anim, pAnimals anim_arr, int index, int map_len);

int main() {
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	Settings settings = get_settings();
	pAnimals animals;
	struct Anim_counter anim_cnt;
	animal_turn[0] = turn_rabbit;
	animal_turn[1] = turn_wolf_m;
	animal_turn[2] = turn_wolf_f;

	animals = (pAnimals)calloc(settings.map_length * settings.map_length, sizeof(Animals));
	anim_cnt.rab_cnt = randomize(settings.rab1, settings.rab2);
	anim_cnt.wlf_m_cnt = randomize(settings.wlf_m1, settings.wlf_m2);
	anim_cnt.wlf_f_cnt = randomize(settings.wlf_f1, settings.wlf_f2);

	system("cls");

	generate_model(animals, anim_cnt, settings.map_length, settings.wolf_health);

	for (int i = 0; i < settings.map_length; i++) {
		for (int j = 0; j < settings.map_length; j++) {
			printf("%c", Animal_symbols[(animals + j + i * settings.map_length)->type]);
		}
		printf("\n");
	}

	system("pause");
	while (1) {
		system("cls"); 
		turn(animals, settings.map_length, settings.reprod_chance, settings.wolf_health);
		for (int i = 0; i < settings.map_length; i++) {
			for (int j = 0; j < settings.map_length; j++) {
				SetConsoleTextAttribute(hConsole, Animal_colors[(animals + j + i * settings.map_length)->type]);
				printf("%c", Animal_symbols[(animals + j + i * settings.map_length)->type]);
			}
			printf("\n");
		}

		SetConsoleTextAttribute(hConsole, 7);
		system("pause");
	}

	free(animals);
	return 0;
}

int randomize(int val1, int val2) {
	int res = val1 + rand() % (val2 - val1 + 1);
	return res;
}

void generate_model(pAnimals anim_arr, struct Anim_counter anim_cnt, int map_len, int an_health) {
	int offset;
	for (int i = 0; i < map_len; i++) {
		for (int j = 0; j < map_len; j++) {
			offset = j + i * map_len;
			(anim_arr + offset)->type = A_VOID;
			(anim_arr + offset)->x = j;
			(anim_arr + offset)->y = i;
		}
	}

	place_animals(anim_arr, anim_cnt.rab_cnt, A_RABBIT, map_len, an_health);
	place_animals(anim_arr, anim_cnt.wlf_m_cnt, A_WOLF_M, map_len, an_health);
	place_animals(anim_arr, anim_cnt.wlf_f_cnt, A_WOLF_F, map_len, an_health);
}

void place_animals(pAnimals anim_arr, int cur_anim_cnt, int an_type, int map_len, int an_health) {
	int offset;
	while (cur_anim_cnt > 0) {
		for (int i = 0; i < map_len; i++) {
			for (int j = 0; j < map_len; j++) {
				offset = j + i * map_len;
				if ((cur_anim_cnt > 0) && ((anim_arr + offset)->type == A_VOID) && (randomize(0, 100) > 99)) {
					(anim_arr + offset)->type = an_type;
					(anim_arr + offset)->x = j;
					(anim_arr + offset)->y = i;
					(anim_arr + offset)->health = an_health;
					(anim_arr + offset)->isMoved = 0;
					cur_anim_cnt--;
				}
			}
		}
	}
}

void turn(pAnimals anim_arr, int map_len, int reprod_chance, int an_health) {
	int offset;
	for (int i = 0; i < map_len; i++) {
		for (int j = 0; j < map_len; j++) {
			offset = j + i * map_len;
			if ((anim_arr + offset)->isMoved == 0) {
				(anim_arr + offset)->isMoved = 1;
				if ((anim_arr + offset)->type == A_RABBIT) {
					animal_turn[A_RABBIT - 1]((anim_arr + offset), anim_arr, map_len, reprod_chance);
				}
				else if ((anim_arr + offset)->type == A_VOID) {
					continue;
				}
				else {
					animal_turn[((anim_arr + offset)->type) - 1]((anim_arr + offset), anim_arr, map_len, reprod_chance);
				}
			}
		}
	}

	for (int i = 0; i < map_len; i++) {
		for (int j = 0; j < map_len; j++) {
			offset = j + i * map_len;
			(anim_arr + offset)->isMoved = 0;
		}
	}
}

void turn_rabbit(pAnimals cur_anim, pAnimals anim_arr, int map_len, int reprod_chance) {
	int nearAnims[9];																	
	int turn_vars = 0;																	
	int num;
	int index = 0;

	check_near_Anims(*cur_anim, anim_arr, nearAnims, map_len);
	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == 0) {
			turn_vars++;
		}
	}
	num = randomize(1, turn_vars);

#ifdef PDEBUG
	printf("%d\n", num);
#endif
	
	for (int i = 0; i < 9; i++) {													
		if (nearAnims[i] == 0) {													// num равен порядковому номеру
																					// пустой клетки в массиве nearAnims, на которую сходит кролик
			num--;
		}
		if (num == 0) {
			index = i ;
			break;
		}
	}
	move_animal(cur_anim, anim_arr, index, map_len);

	if (randomize(1, 100) <= reprod_chance) {
		check_near_Anims(*cur_anim, anim_arr, nearAnims, map_len);
		nearAnims[4] = 1;
		for (int i = 0; i < 9; i++) {
			if (nearAnims[i] == 0) {
				turn_vars++;
			}
		}
		num = randomize(1, turn_vars);
		for (int i = 0; i < 9; i++) {
			if (nearAnims[i] == 0) {												// num равен порядковому номеру пустой клетки
				num--;																// в массиве nearAnims, в которой появится новый кролик
			}
			if (num == 0) {
				(anim_arr + (cur_anim->x + (i % 3 - 1)) + (cur_anim->y + (i / 3 - 1)) * map_len)->type = A_RABBIT;
				(anim_arr + (cur_anim->x + (i % 3 - 1)) + (cur_anim->y + (i / 3 - 1)) * map_len)->isMoved = 1;
				break;
			}
		}
	}

#ifdef PDEBUG
	printf("%d\n", index);
	for (int i = 0; i < 9; i++) {
		printf("%d ", nearAnims[i]);
	}
	printf("\n");
#endif
}

void turn_wolf_m(pAnimals cur_anim, pAnimals anim_arr, int map_len, int health) {
	int nearAnims[9];
	int turn_vars = 0;
	int num;
	int index = 0;

	check_near_Anims(*cur_anim, anim_arr, nearAnims, map_len);
	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == 0) {
			turn_vars++;
		}
	}
	num = randomize(1, turn_vars);

#ifdef PDEBUG
	printf("%d\n", num);
#endif

	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == 0) {													// num равен порядковому номеру
																					// пустой клетки в массиве nearAnims, на которую сходит кролик
			num--;
		}
		if (num == 0) {
			index = i;
			break;
		}
	}
	move_animal(cur_anim, anim_arr, index, map_len);

#ifdef PDEBUG
	printf("%d\n", index);
	for (int i = 0; i < 9; i++) {
		printf("%d ", nearAnims[i]);
	}
	printf("\n");
#endif
}

void turn_wolf_f(pAnimals cur_anim, pAnimals anim_arr, int map_len, int health) {
	int nearAnims[9];
	int turn_vars = 0;
	int num;
	int index = 0;

	check_near_Anims(*cur_anim, anim_arr, nearAnims, map_len);
	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == 0) {
			turn_vars++;
		}
	}
	num = randomize(1, turn_vars);

#ifdef PDEBUG
	printf("%d\n", num);
#endif

	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == 0) {													// num равен порядковому номеру
																					// пустой клетки в массиве nearAnims, на которую сходит кролик
			num--;
		}
		if (num == 0) {
			index = i;
			break;
		}
	}
	move_animal(cur_anim, anim_arr, index, map_len);

#ifdef PDEBUG
	printf("%d\n", index);
	for (int i = 0; i < 9; i++) {
		printf("%d ", nearAnims[i]);
	}
	printf("\n");
#endif
}

void check_near_Anims(Animals cur_anim, pAnimals anim_arr, int *id_arr, int map_len) {
	int index = 0;
	int offset;
	int locX;
	int locY;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if ((i == 0) && (j == 0)) {
				*(id_arr + index) = A_VOID;
			}
			else {
				locX = cur_anim.x + j;
				locY = cur_anim.y + i;
				if ((locY > -1) && (locY < map_len) && (locX > -1) && (locX < map_len)) {
					offset = locX + locY * map_len;
					*(id_arr + index) = (anim_arr + offset)->type;
				}
				else {
					*(id_arr + index) = -1;
				}
		}
			index++;
	}
}
}

void move_animal(pAnimals cur_anim, pAnimals anim_arr, int index, int map_len) {
	int locX = index % 3 - 1;
	int locY = index / 3 - 1;

	if (!((locX == 0) && (locY == 0))) {
		locX += cur_anim->x;
		locY += cur_anim->y;
#ifdef PDEBUG 
		printf("%d %d\n", cur_anim->x, cur_anim->y);
		printf("%d %d\n", locX, locY);
#endif
		(anim_arr + locX + locY * map_len)->type = cur_anim->type;
		(anim_arr + locX + locY * map_len)->health = cur_anim->health;
		(anim_arr + locX + locY * map_len)->isMoved = cur_anim->isMoved;
		cur_anim->type = A_VOID;
	}
}