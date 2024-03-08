#include <vector>
#include <list>
#include <fstream>

#include <stdio.h>

#include "qd_minigame_interface.h"
#include "AntiT_minigame.h"
#include "Figure.h"

#ifndef __pre_h 
#define __pre_h

typedef std::vector<qdMinigameObjectInterface*> obj_container_t;

#define figure_max_width 4
#define figure_max_height 4

#define VK_UP 38
#define VK_DOWN 40
#define VK_LEFT 37
#define VK_RIGHT 39
#define VK_END 35
#define VK_HOME 36


#define KEY_FIRST_BOUNCE ((float)0.5)
#define KEY_NEXT_BOUNCE ((float)0.2)

#define SHIFT_DOWN_TIME ((float)0.02)

#define FIRST_LEVEL_SPEED ((float)0.05)

#endif // #ifndef __pre_h 