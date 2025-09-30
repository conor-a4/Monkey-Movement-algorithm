#include <stdlib.h>
#include <string.h>
#include "hungry_monkey.h"
#include <math.h>
#include <stdbool.h>

// im using an extention for different coloured notes thats why there's sysmbols aftet
typedef struct {
    int last_move;
    int column_preference;
} MonkeyState;

struct monkey_action move_monkey(int field[][FIELD_WIDTH], void *monkey_state) {
    struct monkey_action action;

    //save the last move and column pref for later
    MonkeyState* state = (MonkeyState*)monkey_state;
    if (!state) {
        state = malloc(sizeof(MonkeyState));
        state->last_move = MOVE_FWD;
        state->column_preference = -1;
    }

    //* equation variables
    double Vertical_decay = 0.75; //higher number means less decay
    double Left_weight = 0;
    double Right_weight = 0;
    // direction change ratio 
    float direction_choice_ratio = 1.4f;

    // Step 1: Find monkey's current column
    int monkeyColumn = -1;
    for (int column = 0; column < FIELD_WIDTH; column++) {
        if (field[FIELD_HEIGHT - 1][column] == MONKEY_VAL) {
            monkeyColumn = column;
            break;
        }
    }

    // Step 2: Creating the vertical weights for each column 
    //* array to store weight of each column (set to zero cause it breaks otherwise :( )
    float columnWeights[FIELD_WIDTH] = {0};
    //* Itterate through every column
    for (int column = 0; column < FIELD_WIDTH; column++) {
        //* goes bottom up in every row per column to create a weight for each column
        for (int row = FIELD_HEIGHT - 2; row >= 0; row--) {
            //* if there is a treat use it to increase column weight
            if (field[row][column] == TREAT_VAL) {
                //* get the ver distance of the treat for weight calculation
                int vert_dist = FIELD_HEIGHT - 1 - row;
                
                //* make sure the treat is reachable before it affects the weight
                int horiz_dist = abs(column - monkeyColumn);
                int steps = vert_dist;
                
                //* finally if the treat is reachable use its vert to add weight to the column using a mult factor 
                if (horiz_dist <= steps) {

                    float columnweight = pow(Vertical_decay, vert_dist);
                    columnWeights[column] += columnweight;

                    //* Accumulate left/right weights 
                    if (column < monkeyColumn) {
                        Left_weight += columnweight;
                    } 
                    else if (column > monkeyColumn) {
                        Right_weight += columnweight;
                    } 
                    else {
                        Left_weight += columnweight;
                        Right_weight += columnweight;
                    }                
                }
            }
        }
    }

    // Anti ocoscillationcolation 
    if (state->last_move != MOVE_FWD) {
        int momentum_col = monkeyColumn + state->last_move;
        if (momentum_col >= 0 && momentum_col < FIELD_WIDTH) {
            columnWeights[momentum_col] *= 1.1; 
        }
    }

    // Step 3: Choose a target column    
    int target_col = monkeyColumn;
    float max_weight = columnWeights[monkeyColumn];

    //* New and improved left/right decision logic (previous attempts made the monkey better at dancing than fruit catching)
    if (Left_weight > direction_choice_ratio * Right_weight) {
        //* Force left movement as that side as a signifgantly higher density 
        for (int column = 0; column < monkeyColumn; column++) {
            if (columnWeights[column] > max_weight) {
                max_weight = columnWeights[column];
                target_col = column;
            }
        }
    } 

    else if (Right_weight > direction_choice_ratio * Left_weight) {
        //* Force right movement as that side as a signifgantly higher density 
        for (int column = monkeyColumn + 1; column < FIELD_WIDTH; column++) {
            if (columnWeights[column] > max_weight) {
                max_weight = columnWeights[column];
                target_col = column;
            }
        }
    }

    else {
        for (int column = 0; column < FIELD_WIDTH; column++) {
            if (columnWeights[column] > max_weight || (columnWeights[column] == max_weight && state->column_preference == column)) {
                max_weight = columnWeights[column];
                target_col = column;
            }
        }
    }

    // move da munkee 
    if (target_col < monkeyColumn) {
        action.move = MOVE_LEFT;
    } 

    else if (target_col > monkeyColumn) {
        action.move = MOVE_RIGHT;
    } 

    else {
        action.move = MOVE_FWD; 
    }
     
    action.state = state;

    return action;
}