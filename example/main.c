#define FSM_IMPL
#include "../fsm.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ARRLEN(arr) sizeof(arr) / sizeof(*(arr))

#define LIGHT_TIMEOUT 1

enum State {
    ST_GRN = -123, // State IDs can be any value. They don't have to start at 0
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
    EV_YEL_TIMEOUT,
    EV_RED_TIMEOUT,
    EV_GRN_TIMEOUT,
    EV_ERR_DETECTED,
    EV_ERR_FIXED,
};

char *event_to_str(char *dst, enum Event event) {
    switch (event) {
        case EV_YEL_TIMEOUT: strcpy(dst, "YELLOW TIMEOUT"); break;
        case EV_RED_TIMEOUT: strcpy(dst, "RED TIMEOUT"); break;
        case EV_GRN_TIMEOUT: strcpy(dst, "GREEN TIMEOUT"); break;
        case EV_ERR_DETECTED: strcpy(dst, "ERROR DETECTED"); break;
        case EV_ERR_FIXED: strcpy(dst, "ERROR FIXED"); break;
    }
    return dst;
}

static FSM traffic_light_fsm = {};

void on_entry_green(void *ctx) {
    printf("GREEN\n");
    sleep(LIGHT_TIMEOUT);
    fsm_event(&traffic_light_fsm, EV_GRN_TIMEOUT);
}

void on_entry_yellow(void *ctx) {
    printf("YELLOW\n");
    sleep(LIGHT_TIMEOUT);
    fsm_event(&traffic_light_fsm, EV_YEL_TIMEOUT);
}

void on_entry_red(void *ctx) {
    printf("RED\n");
    sleep(LIGHT_TIMEOUT);

    if (rand() % 3 == 0)
        fsm_event(&traffic_light_fsm, EV_ERR_DETECTED);
    else
        fsm_event(&traffic_light_fsm, EV_RED_TIMEOUT);
}

void on_entry_error(void *ctx) {
    printf("ERROR DETECTED\n");
    sleep(3);
    fsm_event(&traffic_light_fsm, EV_ERR_FIXED);
}

void on_exit_error(void *ctx) {
    printf("ERROR FIXED\n");
}

static State states[] = {
    {
        .id = ST_GRN,
        .transitions = {
            {
                .event = EV_GRN_TIMEOUT,
                .target_state_id = ST_YEL,
            },
            {
                .event = EV_ERR_DETECTED,
                .target_state_id = ST_YEL,
            },
        },
        .transitions_sz = 2,
        .on_entry = on_entry_green,
    },
    {
        .id = ST_RED,
        .transitions = {
            {
                .event = EV_RED_TIMEOUT,
                .target_state_id = ST_GRN,
            },
            {
                .event = EV_ERR_DETECTED,
                .target_state_id = ST_ERR,
            },
        },
        .transitions_sz = 2,
        .on_entry = on_entry_red,
    },
    {
        .id = ST_YEL,
        .transitions = {
            {
                .event = EV_YEL_TIMEOUT,
                .target_state_id = ST_RED,
            },
            {
                .event = EV_ERR_DETECTED,
                .target_state_id = ST_RED,
            },
        },
        .transitions_sz = 2,
        .on_entry = on_entry_yellow,
    },
    {
        .id = ST_ERR,
        .transitions = {
            {
                .event = EV_ERR_FIXED,
                .target_state_id = ST_RED,
            },
        },
        .transitions_sz = 1,
        .on_entry = on_entry_error,
        .on_exit = on_exit_error,
    },
};

int main() {
    srand(time(NULL));
    fsm_init(&traffic_light_fsm, states, ARRLEN(states), ST_RED);
}
