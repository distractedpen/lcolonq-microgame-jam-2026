#include <stdio.h>
#include <stdlib.h>
#include "emscripten/em_asm.h"
#include "game.h"
#include "raylib.h"
#include "stdlib.h"
#include "emscripten.h"

void UpdateDrawFrame(void *state);
Vector2 GetRandomPosition();

int main(int argc, char *argv[]) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(240, 160, "Blargg!");
    InitAudioDevice();
    SetTargetFPS(60);

    GlobalState *state = malloc(sizeof(GlobalState));
    state->gameState = READY;
    state->difficulty = 0;
    state->win = false;
    InitGame(state);

    emscripten_run_script("window.parent.postMessage({op: \"ready\"})");

    bool started = false;
    while (!WindowShouldClose()) {
        double difficulty = EM_ASM_DOUBLE({return window.lcolonqJamStart || -1.0;}); 
        if (!started) {
            if (difficulty > 0.0) {
                started = true;
                state->gameState = ACTIVE;
                emscripten_run_script("window.parent.postMessage({op: \"started\", verb: \"Spin to Defend!\"})");
                printf("Sent Start Message!\n");
            }
        } else {
            if (state->gameState == DONE) {
                printf("Sent Done Message!...win = %s\n",
                        state->win == true ? "true" : "false");
                started = false;
                state->gameState = READY;
                ResetGame(state);
                if (state->win) {
                    emscripten_run_script("window.parent.postMessage({op: \"done\", win: true})");
                } else {
                    emscripten_run_script("window.parent.postMessage({op: \"done\", win: false})");
                }
            }
            UpdateGame(state, GetFrameTime());
            DrawGame(state);
        }
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

