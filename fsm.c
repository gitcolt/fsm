#include "fsm.h"

#include <stdio.h>
#include <stdlib.h>

void fsm_init(FSM *fsm, State *states, size_t states_sz, int initial_state_id) {
    if (states_sz > MAX_STATES) {
        fprintf(stderr, "Too many states. Trying to initialize with %zu states, but"
                        " the maximum number of states is %d\n", states_sz, MAX_STATES);
        exit(EXIT_FAILURE);
    }
    fsm->states_sz = states_sz;

    for (size_t i = 0; i < states_sz; ++i) {
        _State *fsm_state = &fsm->states[i];
        fsm_state->id = states[i].id;
        fsm_state->idx = i;
        fsm_state->on_entry = states[i].on_entry;
        fsm_state->on_exit  = states[i].on_exit;
        if (fsm_state->id == initial_state_id)
            fsm->curr_state = fsm_state;
    }
    for (size_t i = 0; i < states_sz; ++i) {
        State *state = &states[i];
        _Transition *fsm_transition = &fsm->states[i].transition;
        fsm_transition->event = state->transition.event;
        int target_state_id = state->transition.target_state_id;
        int found_target_state = 0;
        for (size_t j = 0; j < states_sz; ++j) {
            if (states[j].id == target_state_id) {
                fsm_transition->target_state_idx = j;
                found_target_state = 1;
                break;
            }
        }
        if (!found_target_state) {
            fprintf(stderr, "Could not find target state with id %d\n", target_state_id);
            exit(EXIT_FAILURE);
        }
    }
}

void fsm_event(FSM *fsm, int ev) {
    _State *curr_state = fsm->curr_state;

    if (ev == curr_state->transition.event) {
        if (curr_state->on_exit)
            curr_state->on_exit(fsm->ctx);
        _State *next_state = &fsm->states[curr_state->transition.target_state_idx];
        fsm->curr_state = next_state;
        if (next_state->on_entry)
            next_state->on_entry(fsm->ctx);
        return;
    }
    fprintf(stderr,
            "No matching transition found for current state %d with event id %d\n",
            fsm->curr_state->id,
            ev);
}
