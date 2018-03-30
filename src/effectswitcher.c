#include <genesis.h>
#include <timer.h>
#include "effectswitcher.h"

u32 effect_started;

void
effects_init(void)
{
	current_effect = effects;
	if (current_effect->init)
		current_effect->init();
	effect_started = getTick();
}

void
play_effect(void)
{
	current_effect->effect();
	if (!current_effect->duration)
		return;
	u32 elapsed = getTick() - effect_started;
	if (elapsed >= 300 * current_effect->duration)
		next_effect();
}

void
clear_screen(void)
{
	VDP_clearPlan(PLAN_A, 1);
	VDP_clearPlan(PLAN_B, 1);
}

void
prev_effect(void)
{
	if (current_effect == effects)
		return;
	current_effect--;
	clear_screen();
	if (current_effect->init)
		current_effect->init();
	effect_started = getTick();
}

void
next_effect(void)
{
	struct effect *next = current_effect + 1;
	if (!next->effect)
		return;
	current_effect = next;
	clear_screen();
	if (current_effect->init)
		current_effect->init();
	effect_started = getTick();
}
