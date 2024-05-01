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

#endif // End FSM_H

#ifdef FSM_IMPL

#include <stdio.h>
#include <stdlib.h>

void fsm_init(FSM *fsm, State *states, size_t states_sz, int initial_state_id) {
    fsm->states = states;
    fsm->states_sz = states_sz;
    fsm->curr_state_id = initial_state_id;
}

void fsm_event(FSM *fsm, int ev) {
    for (size_t i = 0; i < fsm->states_sz; ++i) {
        State s = fsm->states[i];
        if (s.id != fsm->curr_state_id)
            continue;
        if (ev == s.transition.event) {
            fsm->curr_state_id = s.transition.target_state_id;
            return;
        }
    }
    fprintf(stderr,
            "No matching transition found for current state %d with event id %d\n",
            fsm->curr_state_id,
            ev);
}

#endif
