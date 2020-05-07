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
void turn(pAnimals anim_arr, Settings set);
void turn_void(pAnimals cur_anim, pAnimals anim_arr, Settings set);
void turn_rabbit(pAnimals cur_anim, pAnimals anim_arr, Settings set);
void turn_wolf_m(pAnimals cur_anim, pAnimals anim_arr, Settings set);
void turn_wolf_f(pAnimals cur_anim, pAnimals anim_arr, Settings set);
void check_near_Anims(Animals cur_anim, pAnimals anim_arr, int *id_arr, int map_len);
void create_wolf(pAnimals anim_arr, int X, int Y, int index, Settings set);
int prepare_moving(int *id_arr, int turn_type, int turn_num);
void move_animal(pAnimals cur_anim, pAnimals anim_arr, int index, int map_len);

void(*animal_turn[4])(pAnimals, pAnimals, Settings);

int main() {
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	Settings settings = get_settings();
	pAnimals animals;
	struct Anim_counter anim_cnt;
	animal_turn[0] = turn_void;
	animal_turn[1] = turn_rabbit;
	animal_turn[2] = turn_wolf_m;
	animal_turn[3] = turn_wolf_f;

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

	int a = 50000;
	int counter = a;
	system("pause");
 		
	while (1) {
		counter--;

		turn(animals, settings);
		
		
		
		if (!counter) {
			counter = a;
			
			system("cls");
			for (int i = 0; i < settings.map_length; i++) {
				SetConsoleTextAttribute(hConsole, 7);
				printf("%.4d\t", i+1);
				for (int j = 0; j < settings.map_length; j++) {
					SetConsoleTextAttribute(hConsole, Animal_colors[(animals + j + i * settings.map_length)->type]);
					printf("%c", Animal_symbols[(animals + j + i * settings.map_length)->type]);
				}
				printf("\n");
			}
			SetConsoleTextAttribute(hConsole, 7);
			system("pause");
			//Sleep(5000);
			
		}
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

void turn(pAnimals anim_arr, Settings set) {
	int offset;
	for (int i = 0; i < set.map_length; i++) {
		for (int j = 0; j < set.map_length; j++) {
			offset = j + i * set.map_length;
			if ((anim_arr + offset)->isMoved == 0) {
				(anim_arr + offset)->isMoved = 1;
				animal_turn[((anim_arr + offset)->type)]((anim_arr + offset), anim_arr, set);
			}
		}
	}

	for (int i = 0; i < set.map_length; i++) {
		for (int j = 0; j < set.map_length; j++) {
			offset = j + i * set.map_length;
			(anim_arr + offset)->isMoved = 0;
		}
	}
}

void turn_void(pAnimals cur_anim, pAnimals anim_arr, Settings set) {
	/*Функция-заглушка, необходимая для корректной работы общей функции хода
	и используемого массива указателей на функции хода каждого животного*/
}

void turn_rabbit(pAnimals cur_anim, pAnimals anim_arr, Settings set) {
	int nearAnims[9];																	
	int turn_vars = 0;																	
	int num;
	int index;

	check_near_Anims(*cur_anim, anim_arr, nearAnims, set.map_length);
	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == A_VOID) {
			turn_vars++;
		}
	}
	num = randomize(1, turn_vars);

#ifdef PDEBUG
	printf("%d\n", num);
#endif
	
	index = prepare_moving(nearAnims, 0, num);
	move_animal(cur_anim, anim_arr, index, set.map_length);

	if (randomize(1, 100) <= set.reprod_chance) {
		check_near_Anims(*cur_anim, anim_arr, nearAnims, set.map_length);
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
				(anim_arr + (cur_anim->x + (i % 3 - 1)) + (cur_anim->y + (i / 3 - 1)) * set.map_length)->type = A_RABBIT;
				(anim_arr + (cur_anim->x + (i % 3 - 1)) + (cur_anim->y + (i / 3 - 1)) * set.map_length)->isMoved = 1;
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

void turn_wolf_m(pAnimals cur_anim, pAnimals anim_arr, Settings set) {
	int nearAnims[9];
	int rabbits_cnt = 0;
	int voids_cnt = 0;
	int wolf_f_cnt = 0;
	int num;
	int turn_type;
	int index;

	check_near_Anims(*cur_anim, anim_arr, nearAnims, set.map_length);
	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == A_VOID) {
			voids_cnt++;
		}
		else if (nearAnims[i] == A_RABBIT) {
			rabbits_cnt++;
		}
		else if (nearAnims[i] == A_WOLF_F) {
			wolf_f_cnt++;
		}
	}

	if (wolf_f_cnt != 0) {
		/*for (int i = 0; i < 9; i++) {
			if (nearAnims[i] == A_WOLF_F) {													// num равен порядковому номеру
				num--;																		// пустой клетки в массиве nearAnims, на которую сходит кролик
			}
			if (num == 0) {
				index = i;
				break;
			}
		}*/
		num = randomize(1, wolf_f_cnt);
		index = prepare_moving(nearAnims, A_WOLF_F, num);
		create_wolf(anim_arr, cur_anim->x, cur_anim->y, index, set);
	}
	else {
		if (rabbits_cnt != 0) {
			num = randomize(1, rabbits_cnt);
			turn_type = 1;
			cur_anim->health += set.wolf_health;
		}
		else {
			num = randomize(1, voids_cnt);
			turn_type = 0;
		}
		index = prepare_moving(nearAnims, turn_type, num);
		move_animal(cur_anim, anim_arr, index, set.map_length);
	}
#ifdef PDEBUG
	printf("%d\n", num);
#endif
	
	cur_anim->health--;
	if (cur_anim->health <= 0) {
		cur_anim->type = A_VOID;
	}

#ifdef PDEBUG
	printf("%d\n", index);
	for (int i = 0; i < 9; i++) {
		printf("%d ", nearAnims[i]);
	}
	printf("\n");
#endif
}

void turn_wolf_f(pAnimals cur_anim, pAnimals anim_arr, Settings set) {
	int nearAnims[9];
	int rabbits_cnt = 0;
	int voids_cnt = 0;
	int num;
	int turn_type;
	int index = 0;

	check_near_Anims(*cur_anim, anim_arr, nearAnims, set.map_length);
	for (int i = 0; i < 9; i++) {
		if (nearAnims[i] == A_VOID) {
			voids_cnt++;
		}
		else if (nearAnims[i] == A_RABBIT) {
			rabbits_cnt++;
		}
	}
	
	if (rabbits_cnt != 0) {
		num = randomize(1, rabbits_cnt);
		turn_type = 1;
		cur_anim->health += set.wolf_health;
	}
	else {
		num = randomize(1, voids_cnt);
		turn_type = 0;
	}

#ifdef PDEBUG
	printf("%d\n", num);
#endif

	index = prepare_moving(nearAnims, turn_type, num);
	move_animal(cur_anim, anim_arr, index, set.map_length);
	
	cur_anim->health--;
	if (cur_anim->health <= 0) {
		cur_anim->type = A_VOID;
	}

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

void create_wolf(pAnimals anim_arr, int X, int Y, int index, Settings set) {
	int locX = X + index % 3 - 1;
	int locY = Y + index / 3 - 1;
	int wolf_gen;
	int nearAnims[9];
	int num = 0;
	int wolf_places = 0;
	int offset = locX + locY * set.map_length;;
	int ind = -1;
	static int a = 0;


	/*printf("%d \n", X);
	printf("%d \n", Y);
	printf("%d \n", locX);
	printf("%d \n", locY);
	printf("%d \n", index);*/


	//printf("URA\n");
	check_near_Anims(*(anim_arr + offset), anim_arr, nearAnims, set.map_length);
	nearAnims[4] = A_WOLF_F;
	for (int i = 0; i < 9; i++) {
		//printf("%d ", nearAnims[i]);
		if (nearAnims[i] == A_VOID) {
			wolf_places++;
		}
	}
	//printf("\n");

	if ((wolf_places > 0) && (wolf_places < 9)) {
		num = randomize(1, wolf_places);
		if ((ind = prepare_moving(nearAnims, A_VOID, num)) >= 0) {
			//printf("%d \n", ind);
			locX = locX + ind % 3 - 1;
			locY = locY + ind / 3 - 1;
			offset = locX + locY * set.map_length;
			wolf_gen = randomize(A_WOLF_M, A_WOLF_F);
			/*wolf_gen = (int)(++a % 2) + 2;
			if (a % 3 == 0)
				wolf_gen = 3;
			else wolf_gen = 2;*/
//printf("%d\t", a);
		
			//printf("%d\t", wolf_gen);
			
			(anim_arr + offset)->isMoved = 1;
			(anim_arr + offset)->health = set.wolf_health;
			(anim_arr + offset)->type = wolf_gen;
		}
		else {
			printf("ZHOPA");
			exit(0);
		}
	}
}

int prepare_moving(int *id_arr, int turn_type, int turn_num) {
	int index;
	for (int i = 0; i < 9; i++) {
		if (*(id_arr + i) == turn_type) {													// num равен порядковому номеру
			turn_num--;																		// пустой клетки в массиве nearAnims, на которую сходит кролик
		}
		if (turn_num == 0) {
			index = i;
			break;
		}
	}
	return index;
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