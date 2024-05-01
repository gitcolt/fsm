#ifndef FSM_H
#define FSM_H

#include <stddef.h>

typedef struct {
    int event;
    int target_state_id;
} Transition;

typedef struct {
    int id;
    Transition transition;
} State;

typedef struct {
    State *states;
    size_t states_sz;
    int curr_state_id;
} FSM;

void fsm_init(FSM *fsm, State *states, size_t states_sz, int initial_state_id);
void fsm_event(FSM *fsm, int ev);

#endif
