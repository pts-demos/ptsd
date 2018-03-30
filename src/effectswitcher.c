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
prev_effect(void)
{
	if (current_effect == effects)
		return;
	current_effect->transition();
	current_effect--;
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
	current_effect->transition();
	current_effect = next;
	if (current_effect->init)
		current_effect->init();
	effect_started = getTick();
}
