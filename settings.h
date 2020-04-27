#ifndef _SETTINGS_H
	#define _SETTINGS_H

#define DEBUG
#define _PDEBUG

struct Settings {
	int map_length;
	float reprod_chance;
	int wolf_health;
	int rab1, rab2;
	int wlf_m1, wlf_m2;
	int wlf_f1, wlf_f2;
	};

struct Settings get_settings();

#endif // !_SETTINGS_H
