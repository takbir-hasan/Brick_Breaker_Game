#include <GL/glut.h>
#include <vector>
#include <cstdio>
#include <cmath>
#include <fstream>
#include "levels.h"
using namespace std;


//-----------Structures-------------
struct Brick {
    float x, y;
    bool destroyed = false;

};


// ----------------- Window -----------------
const int windowWidth = 900, windowHeight = 900;

// ----------------- Game Variables -----------------
float paddleX = 250;
float paddleWidth = 100, paddleHeight = 10;

float ballX = 300, ballY = 250;
float ballRadius = 8;
float dx, dy;

vector<Brick> bricks;
float brickWidth = 60, brickHeight = 20;

int score = 0;
bool gameOver = false;


// ----------------- Levels -----------------
int levelNumber = 1;
Level* currentLevel = nullptr;


// ----------------- Progress -----------------


void saveProgress(int level) {
    ofstream file("progress.txt");
    if(file.is_open()) {
        file << level;
        file.close();
    }
}

int loadProgress() {
    ifstream file("progress.txt");
    int lvl = 1;
    if(file.is_open()) {
        file >> lvl;
        file.close();
    }
    return lvl;
}

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y); 
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h); 
    glVertex2f(x, y+h);
    glEnd();
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 100; ++i) {
        float angle = 2 * 3.1416 * i / 100;
        glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
    }
    glEnd();
}

void drawText(float x, float y, const char* str) {
    glRasterPos2f(x, y);
    for (const char* c = str; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}


// Stroke text for larger messages
void drawStrokeText(float x, float y, const char* str, float scale = 0.2f) 
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, -scale, 1);
    for (const char* c = str; *c; ++c)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
    glPopMatrix();
}



// ----------------- Game Functions -----------------
void resetGame() {
    bricks.clear();

    dx = currentLevel->dx;       
    dy = currentLevel->dy;
    paddleWidth = currentLevel->paddleWidth;


    if(currentLevel == &level1)
    {
        // Pyramid | Triangle Pattern for level1
        int baseCols = currentLevel->rows;
        int rows = currentLevel->rows;

        for(int r = 0; r<rows; r++)
        {
            int cols = baseCols - r;  // For each rows columns number
            float startX = (windowWidth - (cols * (brickWidth + 10) - 10)) / 2; // center-align
            for(int c = 0; c < cols; c++) 
            {
                float x = startX + c * (brickWidth + 10);
                float y = r * (brickHeight + 10) + 80; // 80 px top margin and 10 px gap
                bricks.push_back({x, y}); 
            }
        }
    } else if(currentLevel == &level2) {
        // Regular grid for level2
        int cols = currentLevel->cols;
        int rows = currentLevel->rows;

        float totalWidth = cols * (brickWidth + 10) - 10;
        float startX = (windowWidth - totalWidth) / 2;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                float x = startX + c * (brickWidth + 10);
                float y = r * (brickHeight + 10) + 80;
                bricks.push_back({x, y});
            }
        }
    } else if(currentLevel == &level3) {
        // Regular grid for level3
        int cols = currentLevel->cols;
        int rows = currentLevel->rows;

        float totalWidth = cols * (brickWidth + 10) - 10;
        float startX = (windowWidth - totalWidth) / 2;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                float x = startX + c * (brickWidth + 10);
                float y = r * (brickHeight + 10) + 80;
                bricks.push_back({x, y});
            }
        }
    } else if(currentLevel == &level4) {
        // Diamond pattern for level4
        int maxCols = currentLevel->cols;
        int rows = currentLevel->rows;
        
        for(int r = 0; r < rows; r++) {
            int cols;
            if(r < rows/2) {
                cols = r + 1; // Growing diamond
            } else {
                cols = rows - r; // Shrinking diamond
            }
            
            float startX = (windowWidth - (cols * (brickWidth + 10) - 10)) / 2;
            for(int c = 0; c < cols; c++) {
                float x = startX + c * (brickWidth + 10);
                float y = r * (brickHeight + 10) + 80;
                bricks.push_back({x, y});
            }
        }
    } else if(currentLevel == &level5) {
        // Full grid with gaps for level5 (hardest)
        int cols = currentLevel->cols;
        int rows = currentLevel->rows;

        float totalWidth = cols * (brickWidth + 10) - 10;
        float startX = (windowWidth - totalWidth) / 2;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                // Create gaps in the pattern for extra difficulty
                if(!(r == 1 && c == 2) && !(r == 1 && c == 7) && 
                   !(r == 3 && c == 1) && !(r == 3 && c == 8) &&
                   !(r == 5 && c == 4) && !(r == 5 && c == 5)) {
                    float x = startX + c * (brickWidth + 10);
                    float y = r * (brickHeight + 10) + 80;
                    bricks.push_back({x, y});
                }
            }
        }
    } else if(currentLevel == &level6) {
        // Full grid with star pattern for level6
        int cols = currentLevel->cols;
        int rows = currentLevel->rows;

        float totalWidth = cols * (brickWidth + 10) - 10;
        float startX = (windowWidth - totalWidth) / 2;

        // Star pattern
        int starPattern[rows][cols] = {
            {0,0,0,0,0,1,0,0,0,0,0},
            {0,0,0,0,1,1,1,0,0,0,0},
            {0,0,0,1,0,1,0,1,0,0,0},
            {0,0,1,0,0,1,0,0,1,0,0},
            {0,1,0,0,0,1,0,0,0,1,0},
            {1,1,1,1,1,1,1,1,1,1,1},
            {0,1,0,0,0,1,0,0,0,1,0},
            {0,0,1,0,0,1,0,0,1,0,0},
            {0,0,0,1,0,1,0,1,0,0,0},
            {0,0,0,0,1,1,1,0,0,0,0},
            {0,0,0,0,0,1,0,0,0,0,0}
        };

        for(int r = 0; r < rows; r++) {
            for(int c = 0; c < cols; c++) {
                if(starPattern[r][c] == 1) {
                    float x = startX + c * (brickWidth + 10);
                    float y = r * (brickHeight + 10) + 80;
                    bricks.push_back({x, y});
                }
            }
        }
    }

    
    
    ballX = windowWidth/2;
    ballY = windowHeight/2;
    paddleX = windowWidth/2 - paddleWidth/2;
    

    score = 0;
    gameOver = false;

}


void nextLevel() {
    if(levelNumber == 1)
    { 
        levelNumber=2; 
        currentLevel=&level2; 
        saveProgress(levelNumber); 
        resetGame(); 
    } else if(levelNumber == 2) {
        levelNumber=3; 
        currentLevel=&level3; 
        saveProgress(levelNumber); 
        resetGame(); 
    } else if(levelNumber == 3) {
        levelNumber=4; 
        currentLevel=&level4; 
        saveProgress(levelNumber); 
        resetGame(); 
    } else if(levelNumber == 4) {
        levelNumber=5; 
        currentLevel=&level5; 
        saveProgress(levelNumber); 
        resetGame(); 
    } else if(levelNumber == 5) {
        levelNumber=6;
        currentLevel=&level6; 
        saveProgress(levelNumber);
        resetGame();
    }
    else {
        gameOver = true;
    }
}

// ----------------- Display -----------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //Paddle
    glColor3f(0, 0, 1);
    drawRect(paddleX, windowHeight - 30, paddleWidth, paddleHeight);


    //Ball
    glColor3f(1, 1, 1);
    drawCircle(ballX, ballY, ballRadius);

    // Bricks
    for(const auto& b: bricks)
    {
        if(!b.destroyed){
            glColor3f(currentLevel->brickColor[0],
                    currentLevel->brickColor[1],
                    currentLevel->brickColor[2]);
            drawRect(b.x,b.y,brickWidth,brickHeight);
        }
    }

    // Score & Level
    char scoreText[50]; 
    sprintf(scoreText, "Score: %d", score); drawText(50,50,scoreText);

    char levelText[20]; 
    sprintf(levelText, "Level: %d", levelNumber); drawText(windowWidth-100,50,levelText);

    if(gameOver){
        glColor3f(1,1,1); // Red color for game over text
        const char* msg = "Game Over! Press R to Restart";
        int textWidth = 0;
        for (const char* c = msg; *c; ++c)
        {
            // Calculate total width of the text
            textWidth += glutBitmapWidth(GLUT_BITMAP_9_BY_15, *c);
        }
        // Calculate centered position
        float centerX = windowWidth / 2.0f;
        float posX = centerX - textWidth / 2.0f;
        drawText(posX, 50, msg); // Here 50 is y position and posX is x position left aligned
    }

    glutSwapBuffers();
}

// ----------------- Update -----------------
void update(int val) {
    if (gameOver) {
        glutTimerFunc(16, update, 0);
        glutPostRedisplay();
        return;
    }

    ballX += dx;
    ballY += dy;

    // Wall collision
    if (ballX < 0 || ballX > windowWidth) 
    {
        dx = dx * (-1);
    }
    if (ballY < 0) 
    {
        dy = dy * (-1);
    }
    if (ballY > windowHeight) 
    {
        gameOver = true;
    }

    // Paddle collision
    if (ballY + ballRadius >= windowHeight - 30 &&
        ballX >= paddleX && ballX <= paddleX + paddleWidth) {
        dy *= -1;
    }

    // Brick collision
    for (auto& b : bricks) {
        if (!b.destroyed &&
            ballX >= b.x && ballX <= b.x + brickWidth &&
            ballY >= b.y && ballY <= b.y + brickHeight) {
            dy *= -1;
            b.destroyed = true;
            score += 10;
            break;
        }
    }

    // Check level complete
    bool allDestroyed = true;
    for(auto &b: bricks) 
    {
        if(!b.destroyed)
        {
            allDestroyed=false;
        }
    }

    if(allDestroyed)
    {
        nextLevel();
    }


    glutPostRedisplay();
    glutTimerFunc(32, update, 0);
}

void keyboard(unsigned char key, int, int) { // We can ignore x and y because we don’t use the mouse position.
    if(key=='r' || key=='R'){
        saveProgress(levelNumber);
        resetGame();
    }
}

void special(int key, int, int) {
    if (key == GLUT_KEY_LEFT && paddleX > 0)
    {
        paddleX -= 25;
    }
    if (key == GLUT_KEY_RIGHT && paddleX + paddleWidth < windowWidth)
    {
        paddleX += 25;
    }
}

// ----------------- Init -----------------
void init() {
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    glClearColor(0, 0, 0, 1);
    resetGame();
}

// ----------------- Main -----------------
int main(int argc, char** argv) {

    levelNumber = loadProgress();
    if(levelNumber==1){
        currentLevel = &level1;
    }
    else if(levelNumber==2) 
    {
        currentLevel = &level2;
    }
    else if(levelNumber==3) 
    {
        currentLevel = &level3;
    }
    else if(levelNumber==4) 
    {
        currentLevel = &level4;
    }
    else if(levelNumber==5) 
    {
        currentLevel = &level5;
    } else if(levelNumber==6) 
    {
        currentLevel = &level6;
    }
 

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL Brick Breaker");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
