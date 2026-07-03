#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include  <time.h>

typedef struct GameActiveState {
    int numCubes;
    int maxCubes;
    int cubeSpawnTimer;
    float baseCubeSpeed;
    int playerLives;
    int cubesRemaining;
    char *HUD;
} GameActiveState;

typedef struct Cube {
    Vector3 position;
    float angle;
    float speed;
    Color color;
    bool active;
} Cube;

typedef struct Sphere {
    Vector3 center;
    float radius;
    Color color;
} Sphere;

typedef struct Triangle {
    Vector2 top;
    Vector2 bLeft;
    Vector2 bRight;
    Vector2 position;
    Color color;
} Triangle;

typedef struct Shield {
    Vector2 startingPos;
    Vector2 endingPos;
    float startAngle;
    float size;
} Shield;

typedef struct Mouse {
    Vector2 position;
} Mouse;

Cube *createCube();
void activateCube(Cube *c);
bool checkCollision(Cube *c, Sphere *s);
bool checkCollisionShield(Cube *c, Shield *s);

void ResetCamera(Camera *camera);
void ResetGameState();
void ResetObjects();
void ResetCubes(GlobalState *state);
void InitSounds();

Sound blockSound;
Sound hitSound;
Sound winSound;
Sound loseSound;

GameActiveState *gameActiveState;
Mouse *mouse;
Cube **cubes;
Shader cubeShader;
int cubeColorLoc;

Triangle *triangle;
Model pyramidModel;
Shader triangleShader;
int triangleColorLoc;

Shield *shield;
Shader shieldShader;

Sphere *sphere;


void InitGame(GlobalState *state) {
    Camera *camera = malloc(sizeof(Camera));
    ResetCamera(camera);
    state->camera = camera;

    triangleShader = LoadShader(0, "shaders/triangle.fs");
    triangleColorLoc = GetShaderLocation(triangleShader, "ourColor");
    cubeShader = LoadShader(0, "shaders/cube.fs");
    cubeColorLoc = GetShaderLocation(cubeShader, "ourColor");

    gameActiveState = malloc(sizeof(GameActiveState));
    ResetGameState();
    
    mouse = malloc(sizeof(Mouse));
    sphere = malloc(sizeof(Sphere));
    triangle = malloc(sizeof(Triangle));
    shield = malloc(sizeof(Shield));
    ResetObjects();

    cubes = malloc(sizeof(Cube*) * gameActiveState->maxCubes);
    ResetCubes(state);
    for (int i = 0; i < gameActiveState->maxCubes; i++) {
        Cube *c = createCube();
        cubes[i] = c;
    }

    InitSounds();
}

void InitSounds() {
    blockSound = LoadSound("sounds/block.ogg");
    hitSound = LoadSound("sounds/hit.ogg");
    winSound = LoadSound("sounds/win.ogg");
    loseSound = LoadSound("sounds/lose.ogg");
}

void ResetGame(GlobalState *state) {
    ResetCamera(state->camera);
    ResetGameState();
    ResetObjects();
    ResetCubes(state);
}

void ResetCamera(Camera *camera) {
    camera->position = (Vector3){0.0f, 0.0f, 15.0f};
    camera->target = (Vector3){0.0f, 0.0f, 0.0f};
    camera->up = (Vector3){0.0f, 1.0f, 0.0f};
    camera->fovy = 60.0f;
    camera->projection = CAMERA_PERSPECTIVE;
}

void ResetGameState() {
    gameActiveState->playerLives = 5;
}

void ResetObjects() {
    mouse->position = GetMousePosition();
    mouse->position = (Vector2){mouse->position.x - (GetScreenWidth() / 2.0f), mouse->position.y - (GetScreenHeight() / 2.0f)};

    sphere->center = (Vector3){0.0f, 0.0f, 0.0f};
    sphere->radius = 5.0f;

    triangle->position = (Vector2){0.0f, 0.0f};
    triangle->top = (Vector2){2, 0};
    triangle->bRight = (Vector2){-1, 1};
    triangle->bLeft = (Vector2){-1, -1};

    shield->size = PI / 3; 
}

void ResetCubes(GlobalState *state) {
    gameActiveState->cubesRemaining = 5 + (int)(state->difficulty / 2);
    gameActiveState->maxCubes = 10;
    gameActiveState->cubeSpawnTimer = 60 - (int)(state->difficulty / 2);
    gameActiveState->baseCubeSpeed = 0.2f; 
    gameActiveState->numCubes = 0;
}

void activateCube(Cube *cube) {
    int randX = GetRandomValue(0, 1) ? GetRandomValue(-20, -10) : GetRandomValue(10, 20);
    int randY = GetRandomValue(0, 1) == 1 ? 10.0f : -10.0f;
    cube->position = (Vector3){
        randX,
        randY,
        0.0f,
    };
    cube->active = true;
    cube->speed = gameActiveState->baseCubeSpeed;
}

Cube *createCube() {
    Cube *cube = malloc(sizeof(Cube));
    cube->position = (Vector3){
        0.0f,
        0.0f,
        0.0f,
    };
    cube->speed = gameActiveState->baseCubeSpeed;
    cube->active = false;
    return cube;
}


void rotateTriangle(float theta) {
    triangle->top = Vector2Rotate(triangle->top, theta);
    triangle->bRight = Vector2Rotate(triangle->bRight, theta);
    triangle->bLeft = Vector2Rotate(triangle->bLeft, theta);
}

Vector2 swapComponents(Vector2 v) {
    return (Vector2){-v.y, -v.x};
}

void updateTriangleShader() {
    double timeValue = GetTime();
    float greenValue = sin(timeValue) + 0.5f;
    float blueValue = -1 * (sin(timeValue) + 0.5f);
    Vector4 ourColor = {0, greenValue, blueValue, 1.0f};
    SetShaderValue(triangleShader, triangleColorLoc, &ourColor, SHADER_UNIFORM_VEC4);
}

void updateCubeShader() {
    double timeValue = GetTime();
    float redValue = sin(timeValue) + 0.5f;
    float blueValue = -1 * (sin(timeValue) + 0.5f);
    Vector4 ourColor = {redValue, 0, blueValue, 1.0f};
    SetShaderValue(cubeShader, cubeColorLoc, &ourColor, SHADER_UNIFORM_VEC4);
}


void UpdateGame(GlobalState *gState, float dt) {
    if (gameActiveState->cubesRemaining == 0) {
        if (!IsSoundPlaying(winSound)) PlaySound(winSound);
        gState->win = true;
        gState->gameState = DONE;
        return;
    } else if (gameActiveState->playerLives == 0) {
        if (!IsSoundPlaying(loseSound)) PlaySound(loseSound);
        gState->win = false;
        gState->gameState = DONE;
        return;
    }

    Vector2 newMousePos = GetMousePosition();
    Vector2 translatedMousePos = (Vector2){newMousePos.x - (GetScreenWidth() / 2.0f), newMousePos.y - (GetScreenHeight() / 2.0f)};
    float theta = Vector2DotProduct(triangle->top, swapComponents(translatedMousePos)) / (Vector2Length(translatedMousePos) * Vector2Length(triangle->top));
    rotateTriangle(theta);
    mouse->position = translatedMousePos;

    if (gameActiveState->cubeSpawnTimer <= 0) {
        for (int i = 0; i < gameActiveState->maxCubes; i++) {
            if (!cubes[i]->active) {
                activateCube(cubes[i]);
                gameActiveState->numCubes++;
                break;
            }
        }

        gameActiveState->cubeSpawnTimer = 100 - gState->difficulty;
    } else {
        if (dt > 1) {
            gameActiveState->cubeSpawnTimer -= (int)dt;
        } else {
            gameActiveState->cubeSpawnTimer -= 1;
        }
    }

    for (int i = 0; i < gameActiveState->numCubes; i++) {
        Cube *c = cubes[i];
        if (c->active) {
            if (checkCollisionShield(c, shield)) {
                if (!IsSoundPlaying(blockSound)) PlaySound(blockSound);
                c->active = false;
                gameActiveState->numCubes--;
                gameActiveState->cubesRemaining--;
            }
            if (checkCollision(c, sphere)) {
                if (!IsSoundPlaying(hitSound)) PlaySound(hitSound);
                c->active = false;
                gameActiveState->numCubes--;
                gameActiveState->playerLives--;
            } else {
                float e = c->speed / sqrt(c->position.x * c->position.x + c->position.y * c->position.y);
                int xSgn = c->position.x > 0 ? -1 : 1;
                int ySgn = c->position.y > 0 ? -1 : 1;
                float xSpeed = xSgn * e * (triangle->position.x - c->position.x);
                float ySpeed = ySgn * e * (triangle->position.y - c->position.y);
                if (c->position.x >= 0)
                    c->position.x -= xSpeed; 
                else
                    c->position.x += xSpeed; 

                if (c->position.y >= 0)
                    c->position.y -= ySpeed;
                else
                    c->position.y += ySpeed;
            }
        }
    }

    shield->startingPos = Vector2Scale(Vector2Rotate(Vector2Normalize(triangle->top), shield->size / 2), sphere->radius + 2);
    shield->endingPos = Vector2Scale(Vector2Rotate(Vector2Normalize(triangle->top), -1 * shield->size / 2), sphere->radius + 2);

    sprintf(gameActiveState->HUD, "Lives: %d | Remaining: %d", gameActiveState->playerLives, gameActiveState->cubesRemaining);

    // Update Shaders
    updateTriangleShader();
    updateCubeShader();

    return;
}

void drawCubes() {
    for (int i = 0; i < gameActiveState->numCubes; i++) {
        Cube *c = cubes[i];
        if (c->active) {
            DrawCube(c->position, 0.5f, 0.5f, 0.5f, RED);
        }
    }
}

void DrawGame(GlobalState *gState) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(*gState->camera);
        BeginShaderMode(triangleShader);
            DrawTriangle3D(
                    (Vector3){triangle->top.x, triangle->top.y, 0},
                    (Vector3){triangle->bRight.x, triangle->bRight.y, 0},
                    (Vector3){triangle->bLeft.x, triangle->bLeft.y, 0}, 
                    WHITE);
        EndShaderMode();
            DrawSphereWires(sphere->center, sphere->radius, 5, 5, GRAY);
            DrawLine3D((Vector3){ shield->startingPos.x, shield->startingPos.y, 0 },
                    (Vector3){ shield->endingPos.x, shield->endingPos.y, 0 }, 
                    BLUE);
        BeginShaderMode(cubeShader);
            drawCubes();
        EndShaderMode();
        EndMode3D();
        DrawText(gameActiveState->HUD, 10, 10, 14, BLACK);
    EndDrawing();
    return;
}

bool checkCollision(Cube *c, Sphere *s) {
    return CheckCollisionBoxSphere(
                (BoundingBox){
                    (Vector3){ c->position.x - 0.25f,
                               c->position.y - 0.25f,
                               c->position.z - 0.25f },
                    (Vector3){ c->position.x + 0.25f,
                               c->position.y + 0.25f,
                               c->position.z + 0.25f },
                }, sphere->center, 5.0f);
};

bool checkCollisionShield(Cube *c, Shield *s) {
    return CheckCollisionLines(
            (Vector2) {
                c->position.x - 0.5f,
                c->position.y - 0.5f
            }, // Cube Diagonal start
            (Vector2) {
                c->position.x + 0.5f,
                c->position.y + 0.5f
            }, // Cube Diagonal end
            s->startingPos,
            s->endingPos,
            NULL);
}
