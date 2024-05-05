#define FSM_IMPL
#include "../fsm.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ARRLEN(arr) sizeof(arr) / sizeof(*(arr))

enum State {
    ST_GRN,
    ST_YEL,
    ST_RED,
    ST_ERR,
    ST_ANY,
};

char *state_to_str(char *dst, enum State state) {
    switch (state) {
        case ST_GRN: strcpy(dst, "GREEN"); break;
        case ST_YEL: strcpy(dst, "YELLOW"); break;
        case ST_RED: strcpy(dst, "RED"); break;
        case ST_ERR: strcpy(dst, "ERROR"); break;
        case ST_ANY: strcpy(dst, "ANY"); break;
    }
    return dst;
}

enum Event {
    EV_YEL_TO_RED,
    EV_RED_TO_GRN,
    EV_GRN_TO_YEL,
    EV_ERR,
};

char *event_to_str(char *dst, enum Event event) {
    switch (event) {
        case EV_YEL_TO_RED: strcpy(dst, "YELLOW to RED"); break;
        case EV_RED_TO_GRN: strcpy(dst, "RED to GREEN"); break;
        case EV_GRN_TO_YEL: strcpy(dst, "GREEN to YELLOW"); break;
        case EV_ERR: strcpy(dst, "ERROR"); break;
    }
    return dst;
}

void exiting_red_state_func() {
    printf("exiting RED state\n");
}

void entering_yel_state_func() {
    printf("entering YELLOW state\n");
}

int main() {
    FSM traffic_light_fsm;
    State states[] = {
        { .id = ST_GRN, .transitions = { { .event = EV_GRN_TO_YEL, .target_state_id = ST_YEL, } }, .transitions_sz = 1,                                      },
        { .id = ST_RED, .transitions = { { .event = EV_RED_TO_GRN, .target_state_id = ST_GRN, } }, .transitions_sz = 1, .on_exit  = exiting_red_state_func,  },
        { .id = ST_YEL, .transitions = { { .event = EV_YEL_TO_RED, .target_state_id = ST_RED, } }, .transitions_sz = 1, .on_entry = entering_yel_state_func, },
    };

    fsm_init(&traffic_light_fsm, states, ARRLEN(states), ST_RED);

    char buf[99];
    printf("Initial state: %s\n", state_to_str(buf, traffic_light_fsm.curr_state->id));
    sleep(1);

    enum Event ev = EV_RED_TO_GRN;
    fsm_event(&traffic_light_fsm, ev);
    printf("Event triggered: %s\n", event_to_str(buf, ev));
    printf("state: %s\n", state_to_str(buf, traffic_light_fsm.curr_state->id));
    sleep(1);

    ev = EV_GRN_TO_YEL;
    fsm_event(&traffic_light_fsm, ev);
    printf("Event triggered: %s\n", event_to_str(buf, ev));
    printf("state: %s\n", state_to_str(buf, traffic_light_fsm.curr_state->id));
    sleep(1);

    ev = EV_YEL_TO_RED;
    fsm_event(&traffic_light_fsm, ev);
    printf("Event triggered: %s\n", event_to_str(buf, ev));
    printf("state: %s\n", state_to_str(buf, traffic_light_fsm.curr_state->id));
    sleep(1);
}
