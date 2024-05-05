#ifndef FSM_H
#define FSM_H

#include <stddef.h>

#define MAX_STATES 10
#define MAX_TRANSITIONS_PER_STATE 3

typedef struct {
    int event;
    int target_state_id;
} Transition;

typedef struct {
    int event;
    size_t target_state_idx;
} _Transition;

typedef struct {
    int id;
    Transition transitions[MAX_TRANSITIONS_PER_STATE];
    size_t transitions_sz;
    void (*on_exit)(void *ctx);
    void (*on_entry)(void *ctx);
} State;

typedef struct {
    int id;
    int idx;
    _Transition transitions[MAX_TRANSITIONS_PER_STATE];
    size_t transitions_sz;
    void (*on_exit)(void *ctx);
    void (*on_entry)(void *ctx);
} _State;

typedef struct {
    _State states[MAX_STATES];
    size_t states_sz;
    _State *curr_state;
    void *ctx;
} FSM;

void fsm_init(FSM *fsm, State *states, size_t states_sz, int initial_state_id);
void fsm_event(FSM *fsm, int ev);

#endif // End FSM_H

#ifdef FSM_IMPL

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
        fsm_state->transitions_sz = states[i].transitions_sz;
        if (fsm_state->id == initial_state_id)
            fsm->curr_state = fsm_state;
    }
    for (size_t i = 0; i < states_sz; ++i) {
        State *state = &states[i];

        for (size_t j = 0; j < state->transitions_sz; ++j) {
            Transition *src_trans = &state->transitions[j];
            _Transition *dst_trans = &fsm->states[i].transitions[j];

            dst_trans->event = src_trans->event;

            int found_target_state = 0;
            for (int k = 0; k < states_sz; ++k) {
                if (states[k].id == src_trans->target_state_id) {
                    dst_trans->target_state_idx = k;
                    found_target_state = 1;
                    break;
                }
            }
            if (!found_target_state) {
                fprintf(stderr,
                        "Could not find target state with id %d\n",
                        src_trans->target_state_id);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (fsm->curr_state->on_entry)
        fsm->curr_state->on_entry(fsm->ctx);
}

void fsm_event(FSM *fsm, int ev) {
    _State *curr_state = fsm->curr_state;

    for (int i = 0; i < curr_state->transitions_sz; ++i) {
        _Transition transition = curr_state->transitions[i];
        if (transition.event == ev) {
            if (curr_state->on_exit)
                curr_state->on_exit(fsm->ctx);
            _State *next_state = &fsm->states[transition.target_state_idx];
            fsm->curr_state = next_state;
            if (next_state->on_entry)
                next_state->on_entry(fsm->ctx);
            return;
        }
    }
    fprintf(stderr,
            "No matching transition found for current state %d with event id %d\n",
            fsm->curr_state->id,
            ev);
}

#endif
