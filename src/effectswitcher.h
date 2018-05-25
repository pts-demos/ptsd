#ifndef EFFECTSWITCHER_H
#define EFFECTSWITCHER_H

struct effect {
	void (*effect)(void);
	void (*init)(void);
	void (*transition)(void);
	u32 duration; /* seconds */
	void (*uninit)(void);
};

struct effect *current_effect;
/* in main.c */
extern struct effect effects[];

void effects_init(void);
void play_effect(void);
void prev_effect(void);
void next_effect(void);
void clear_screen(void);
#endif
