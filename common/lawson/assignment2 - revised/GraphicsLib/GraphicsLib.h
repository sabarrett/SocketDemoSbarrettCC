#pragma once
#include <allegro5/allegro.h>

ALLEGRO_DISPLAY* initAllegro();
void drawScene(ALLEGRO_DISPLAY* pDisplay);
void cleanupAllegro(ALLEGRO_DISPLAY* pDisplay);

ALLEGRO_EVENT_QUEUE* initEventQueue();
bool processEventQueue(ALLEGRO_EVENT_QUEUE* theQueue);
void cleanupEventQueue(ALLEGRO_EVENT_QUEUE* theQueue);
