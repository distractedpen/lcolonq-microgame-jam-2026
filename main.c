#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "raylib.h"
#include "stdlib.h"
#include "emscripten.h"

EM_JS(void, send_ready, (), { window.parent.postMessage({op : "ready"}); });

EM_JS(void, send_started, (char *verb),
        { window.parent.postMessage({op : "started", verb : verb}); });

EM_JS(void, send_done, (bool win),
        { window.parent.postMessage({op : "done", win : win}); });

EM_JS(int, get_difficulty, (), {
    return window.lcolonqJamStart || -1.0;
});

void UpdateDrawFrame(void *state);
Vector2 GetRandomPosition();


int main(int argc, char *argv[]) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(240, 160, "Blargg!");
    SetTargetFPS(60);

    GlobalState *state = malloc(sizeof(GlobalState));
    state->gameState = READY;
    state->difficulty = 0;
    state->win = false;
    InitGame(state);

    send_ready();

    bool started = false;
    while (!WindowShouldClose()) {
        double difficulty = get_difficulty();
        if (!started) {
            if (difficulty > 0.0) {
                started = true;
                state->gameState = ACTIVE;
                send_started("Spin to Defend!");
                printf("Sent Start Message!\n");
            }
        } else {
            if (state->gameState == DONE) {
                printf("Sent Done Message!...win = %s\n",
                        state->win == true ? "true" : "false");
                started = false;
                state->gameState = READY;
                ResetGame(state);
                send_done(state->win);
            }
            UpdateGame(state, GetFrameTime());
            DrawGame(state);
        }
    }
    return 0;
}

