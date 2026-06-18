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
void InitCamera(GlobalState *state);
void InitObjects();
void InitCubes(GlobalState *state);
void DestroyObjects(GlobalState *state);
void InitGameActiveState();

#endif // GAME_H

