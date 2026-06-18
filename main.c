#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "raylib.h"
#include "stdlib.h"

#if defined(PLATFORM_WEB)
#include "emscripten/emscripten.h"

EM_JS(void, send_ready, (), { window.parent.postMessage({op : "ready"}); });

EM_JS(void, send_started, (char *verb),
      { window.parent.postMessage({op : "started", verb : verb}); });

EM_JS(void, send_done, (bool win),
      { window.parent.postMessage({op : "done", win : win}); });
EM_JS(int, poll_start, (), {
  if (window.lcolonqJamStart) {
    return window.lcolonqJamStart;
  }
  return 0;
});
#else
int gameDifficulty;
#endif

void initGame(GlobalState *state);
void UpdateDrawFrame(void *state);
Vector2 GetRandomPosition();

int main(int argc, char *argv[]) {

  SetConfigFlags(FLAG_MSAA_4X_HINT);

  InitWindow(240, 160, "Blargg!");

  GlobalState *state = malloc(sizeof(GlobalState));
  initGame(state);

  SetTargetFPS(60);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(UpdateDrawFrame, state, 60, 1);
#else

  if (argc >= 2) {
    char *raw_diff = argv[1];
    state->difficulty = atoi(raw_diff);
  } else {
    state->difficulty = 5;
  }

  while (!WindowShouldClose()) {
    if (state->gameState == DONE)
      break;
    UpdateDrawFrame(state);
  }

#endif
  free(state);
  CloseWindow();
  return 0;
}

void initGame(GlobalState *state) {
  state->gameState = READY;
  state->difficulty = 0;
  state->win = false;
  InitCamera(state);
  InitGameActiveState();
}

void startGame(GlobalState *state) {
  state->gameState = ACTIVE;
#if defined(PLATFORM_WEB)
  send_started("Spin to Defend!");
  printf("Sent Start Message!...Spin to Defend!\n");
#endif
}

void UpdateDrawFrame(void *state) {
  GlobalState *gState = (GlobalState *)state;

  if (gState->gameState == READY) {
#if defined(PLATFORM_WEB)
    gState->difficulty = poll_start();
    if (gState->difficulty > 0) {
      InitCubes(state);
      startGame(gState);
    } else {
      return;
    }
#else
    InitCubes(state);
    startGame(state);
#endif
  }

  if (gState->gameState == DONE) {
#if defined(PLATFORM_WEB)
    printf("Sent Done Message!...win = %s\n",
           gState->win == true ? "true" : "false");
    send_done(gState->win);
    DestroyObjects(state);
    initGame(state);
#endif
    return;
  }

  UpdateGame(gState, GetFrameTime());
  DrawGame(gState);
  return;
}
