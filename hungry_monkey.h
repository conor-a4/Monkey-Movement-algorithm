#ifndef __HUNGRY_MONKEY__H_
#define __HUNGRY_MONKEY__H_
#include <stdio.h>
#include <stdlib.h>

#define FIELD_HEIGHT 30
#define FIELD_WIDTH  20
#define P_NEW_TREAT    5
#define TREAT_VAL    1
#define MONKEY_VAL        2

#define MOVE_LEFT -1
#define MOVE_RIGHT 1
#define MOVE_FWD 0

struct monkey_action{
    int move;
    void *state;
};

#define srand while(0)

struct monkey_action move_monkey(int field[][FIELD_WIDTH], void *monkey_state);

#endif
