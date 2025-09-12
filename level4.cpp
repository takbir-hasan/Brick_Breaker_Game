#include "levels.h"

// Level 4 data - Diamond pattern with faster ball and smaller paddle
Level level4 = {
    7,          // rows
    9,          // cols
    6.0f,       // dx (faster ball)
    7.0f,       // dy (faster ball)
    60.0f,      // paddleWidth (smaller paddle)
    {1.0f, 0.0f, 1.0f} // brickColor (magenta)
};
