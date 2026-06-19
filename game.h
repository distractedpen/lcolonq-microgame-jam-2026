#include "raylib.h"
#ifndef GAME_H

typedef enum Gamestate {
  READY,
  ACTIVE,
  DONE,
} Gamestate;


typedef struct GlobalState {
  Gamestate gameState;
  int difficulty;
  int win;
  Camera *camera;
} GlobalState;

void UpdateGame(GlobalState *state, float dt);
void DrawGame(GlobalState *state);

void InitGame(GlobalState *state);

void DestroyObjects(GlobalState *state);

void ResetGame(GlobalState *state);

#endif // GAME_H

