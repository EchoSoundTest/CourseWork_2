#include <stdio.h>

#include "settings.h"

Settings get_settings() {
	Settings set;
	
#ifndef DEBUG
	printf("¬ведите размер пол€: ");
	scanf("%d", &set.map_length);
	printf("¬ведите шанс размножени€ кролика: ");
	scanf("%d", &set.reprod_chance);
	printf("¬ведите максимальный уровень жизни волков: ");
	scanf("%d", &set.wolf_health);
	printf("¬ведите диапазон количества кроликов: "); 
	scanf("%d %d", &set.rab1, &set.rab2);
	printf("¬ведите диапазон количества волков: ");
	scanf("%d %d", &set.wlf_m1, &set.wlf_m2);
	printf("¬ведите диапазон количества волчиц: ");
	scanf("%d %d", &set.wlf_f1, &set.wlf_f2);
#else
	set.map_length = 50;
	set.reprod_chance = 30;
	set.rab1 = 10;
	set.rab2 = 20;
	set.wlf_f1 = 10;
	set.wlf_f2 = 20;
	set.wlf_m1 = 10;
	set.wlf_m2 = 20;
	set.wolf_health = 5;
#endif
	
	return set;
}