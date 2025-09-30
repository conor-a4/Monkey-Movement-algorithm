#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "hungry_monkey.h"

#undef srand

const char chars[] = {0, '1', '2'};

struct prng_s{
    unsigned char *numbers;
    int nxt_number;
    int size;
};

/**
 * Gets the next number from the PRNG
 * If the PRNG reaches the end, wrapped is set to 1 and 
 * we start the chain again.
 */
static unsigned char get_nxt_random(struct prng_s *prng, int *wrapped){
    unsigned char num = 0;
    *wrapped = 0;
    if (prng->nxt_number == prng->size){
        prng->nxt_number = 0;
        *wrapped = 1;
    }
    num = prng->numbers[prng->nxt_number];
    prng->nxt_number++;
    return num;
}

/**
 * Move the rows of the field down.
 * Regenerate the top row.
 * On the bottom row see if the monkey gets any treat.
 *
 * Returns:
 * 1 if the monkey gets a treat
 * 0 otherwise.
 */
static int update_field(int field[][FIELD_WIDTH], int monkey_col, struct prng_s *prng){
    int i,j;
    int dice;
    int ret = 0;

    // Start by shifting the rows down
    for (i=FIELD_HEIGHT-1;i>=1;i--)
        for (j=0;j<FIELD_WIDTH;j++)
            field[i][j] = field[i-1][j];

    // Update the first row
    for (j=0;j<FIELD_WIDTH;j++){
        int wrapped = 0;
        // Generate a new treat with probability P_NEW_TREAT
        // Roll the dice
        dice = get_nxt_random(prng, &wrapped);
        if (wrapped){
            fprintf(stderr, "Wrapped!\n");
        }
        if (dice < P_NEW_TREAT) field[0][j] = TREAT_VAL;
        else field[0][j] = 0;
    }

    // Does the monkey get a treat?
    if (field[FIELD_HEIGHT-1][monkey_col] != 0){
        ret = 1;
    }

    // Show the monkey
    field[FIELD_HEIGHT-1][monkey_col] = MONKEY_VAL;
    return ret;
}

void update_screen(int field[][FIELD_WIDTH]){
    int i,j;
    clear();
    wmove(stdscr, 0,0);

    for (i=0;i<FIELD_HEIGHT;i++)
        for (j=0;j<FIELD_WIDTH;j++)
            if (field[i][j])
                mvwaddch(stdscr, i, j, chars[field[i][j]]);
}

void main(){
    int monkey_col = FIELD_WIDTH>>1;
    void *monkey_state = NULL;
    struct monkey_action nxt_action;
    char position[20] = "";
    int step = 0;
    int treats_eaten = 0;

    int field[FIELD_HEIGHT][FIELD_WIDTH];
    int num_steps = 100 + FIELD_HEIGHT;
    // hold random numbers < 100
    struct prng_s prng;
    prng.size = num_steps*FIELD_WIDTH;
    prng.numbers = malloc(sizeof(unsigned char)*prng.size);
    prng.nxt_number = 0;

    // pre-generate random numbers
    srand(1234);
    for (int i=0;i<prng.size;i++){
        prng.numbers[i] = random()%100;
    }

    // Initially the field is empty
    memset(field, 0, sizeof(int)*FIELD_WIDTH*FIELD_HEIGHT);


    initscr();
    cbreak();
    noecho();

    while (step < 100+FIELD_HEIGHT){
        treats_eaten += update_field(field, monkey_col,&prng);
        nxt_action = move_monkey(field, monkey_state);
        update_screen(field);
        // Clear current monkey position
        field[FIELD_HEIGHT-1][monkey_col] = 0;

        monkey_state = nxt_action.state;
        if (nxt_action.move > 0){
            monkey_col ++;
            mvwaddch(stdscr, 0, 0, '+');
        }else if (nxt_action.move < 0){
            monkey_col --;
            mvwaddch(stdscr, 0, 0, '-');
        }else mvwaddch(stdscr, 0, 0, '0');
        if (monkey_col < 0) monkey_col = 0;
        if (monkey_col >= FIELD_WIDTH) monkey_col = FIELD_WIDTH-1;

        sprintf(position, "%d: %d: %d", step, monkey_col, treats_eaten);
        mvwaddstr(stdscr, FIELD_HEIGHT, 0, position);
        getch();
        step++;
    }

    clear();
    char final[50];
    sprintf(final, "Game over! Score: %d\n", treats_eaten);
    mvwaddstr(stdscr, FIELD_HEIGHT>>1, FIELD_WIDTH>>2, final);
    getch();

    endwin();
    // Free prng
    free(prng.numbers);
}
