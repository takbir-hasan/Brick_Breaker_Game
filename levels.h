#ifndef LEVELS_H
#define LEVELS_H

struct Level {
    int rows;
    int cols;
    float dx;
    float dy;
    float paddleWidth;
    float brickColor[3];
};

extern Level level1;
extern Level level2;
extern Level level3;
extern Level level4;
extern Level level5;

#endif
