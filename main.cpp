#include <GL/glut.h>
#include <vector>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <random>
#include "levels.h"
using namespace std;


//-----------Structures-------------
struct Brick {
    float x, y;
    bool destroyed = false;
    float destroyTime = 0.0f; // For animation
    float originalY; // For bounce effect
};

struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    float size;
    float color[3];
};

//-----------Modern Color Palette-------------
namespace Colors {
    // Background gradients
    const float DARK_BLUE[3] = {0.05f, 0.05f, 0.2f};
    const float LIGHT_BLUE[3] = {0.1f, 0.1f, 0.4f};
    
    // Paddle colors
    const float PADDLE_GRADIENT_TOP[3] = {0.2f, 0.6f, 1.0f};
    const float PADDLE_GRADIENT_BOTTOM[3] = {0.1f, 0.3f, 0.8f};
    
    // Ball colors
    const float BALL_GRADIENT_CENTER[3] = {1.0f, 1.0f, 1.0f};
    const float BALL_GRADIENT_EDGE[3] = {0.8f, 0.9f, 1.0f};
    
    // Brick colors (modern palette)
    const float BRICK_LEVEL1[3] = {0.2f, 0.8f, 0.4f}; // Modern green
    const float BRICK_LEVEL2[3] = {0.9f, 0.3f, 0.3f}; // Modern red
    const float BRICK_LEVEL3[3] = {1.0f, 0.7f, 0.2f}; // Modern orange
    const float BRICK_LEVEL4[3] = {0.7f, 0.3f, 0.9f}; // Modern purple
    const float BRICK_LEVEL5[3] = {0.2f, 0.5f, 0.9f}; // Modern blue
    
    // UI colors
    const float TEXT_COLOR[3] = {1.0f, 1.0f, 1.0f};
    const float SCORE_COLOR[3] = {0.9f, 0.9f, 0.1f};
    const float GAME_OVER_COLOR[3] = {1.0f, 0.2f, 0.2f};
    
    // Particle colors
    const float PARTICLE_BRICK[3] = {1.0f, 0.6f, 0.2f};
    const float PARTICLE_PADDLE[3] = {0.2f, 0.6f, 1.0f};
}


// ----------------- Constants -----------------
const int windowWidth = 600, windowHeight = 600;
const float FPS = 60.0f;
const float FRAME_TIME = 1.0f / FPS;
const float PARTICLE_GRAVITY = 0.1f;
const int PARTICLE_COUNT_BRICK = 8;
const int PARTICLE_COUNT_PADDLE = 5;
const float LEVEL_COMPLETE_DELAY = 2.0f;

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

// Animation and effects
vector<Particle> particles;
float animationTime = 0.0f;
bool showLevelComplete = false;
float levelCompleteTime = 0.0f;
float screenShake = 0.0f;
int scoreChange = 0;
float scoreChangeTime = 0.0f;


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

void drawGradientRect(float x, float y, float w, float h, const float* topColor, const float* bottomColor) {
    glBegin(GL_QUADS);
    glColor3f(topColor[0], topColor[1], topColor[2]);
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glColor3f(bottomColor[0], bottomColor[1], bottomColor[2]);
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

void drawGradientCircle(float cx, float cy, float r, const float* centerColor, const float* edgeColor) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(centerColor[0], centerColor[1], centerColor[2]);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 100; ++i) {
        float angle = 2 * 3.1416 * i / 100;
        glColor3f(edgeColor[0], edgeColor[1], edgeColor[2]);
        glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
    }
    glEnd();
}

void drawBrickWithShadow(float x, float y, float w, float h, const float* color) {
    // Shadow
    glColor3f(0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawRect(x + 2, y + 2, w, h);
    glDisable(GL_BLEND);
    
    // Main brick with gradient
    drawGradientRect(x, y, w, h, color, color);
    
    // Highlight
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x, y);
    glVertex2f(x, y + h);
    glEnd();
}

void drawText(float x, float y, const char* str) {
    glRasterPos2f(x, y);
    for (const char* c = str; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}

void createParticles(float x, float y, const float* color, int count = 8) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<float> velDist(-3.0f, 3.0f);
    uniform_real_distribution<float> lifeDist(0.5f, 1.5f);
    uniform_real_distribution<float> sizeDist(2.0f, 5.0f);
    
    for (int i = 0; i < count; i++) {
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = velDist(gen);
        p.vy = velDist(gen);
        p.life = lifeDist(gen);
        p.maxLife = p.life;
        p.size = sizeDist(gen);
        p.color[0] = color[0];
        p.color[1] = color[1];
        p.color[2] = color[2];
        particles.push_back(p);
    }
}

void updateParticles() {
    for (auto it = particles.begin(); it != particles.end();) {
        it->x += it->vx;
        it->y += it->vy;
        it->vy += PARTICLE_GRAVITY;
        it->life -= FRAME_TIME;
        
        if (it->life <= 0) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void drawParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (const auto& p : particles) {
        float alpha = p.life / p.maxLife;
        glColor4f(p.color[0], p.color[1], p.color[2], alpha);
        drawCircle(p.x, p.y, p.size * alpha);
    }
    
    glDisable(GL_BLEND);
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
                Brick brick = {x, y, false, 0.0f, y};
                bricks.push_back(brick); 
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
                Brick brick = {x, y, false, 0.0f, y};
                bricks.push_back(brick);
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
                Brick brick = {x, y, false, 0.0f, y};
                bricks.push_back(brick);
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
                Brick brick = {x, y, false, 0.0f, y};
                bricks.push_back(brick);
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
                    Brick brick = {x, y, false, 0.0f, y};
                bricks.push_back(brick);
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
    } else {
        gameOver = true;
    }
}

// ----------------- Display -----------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Apply screen shake
    float shakeX = 0, shakeY = 0;
    if(screenShake > 0) {
        static random_device rd;
        static mt19937 gen(rd());
        uniform_real_distribution<float> shakeDist(-screenShake * 10, screenShake * 10);
        shakeX = shakeDist(gen);
        shakeY = shakeDist(gen);
    }
    
    glPushMatrix();
    glTranslatef(shakeX, shakeY, 0);
    
    // Background gradient
    glBegin(GL_QUADS);
    glColor3f(Colors::DARK_BLUE[0], Colors::DARK_BLUE[1], Colors::DARK_BLUE[2]);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glColor3f(Colors::LIGHT_BLUE[0], Colors::LIGHT_BLUE[1], Colors::LIGHT_BLUE[2]);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Paddle with gradient
    drawGradientRect(paddleX, windowHeight - 30, paddleWidth, paddleHeight, 
                    Colors::PADDLE_GRADIENT_TOP, Colors::PADDLE_GRADIENT_BOTTOM);

    // Ball with gradient
    drawGradientCircle(ballX, ballY, ballRadius, 
                      Colors::BALL_GRADIENT_CENTER, Colors::BALL_GRADIENT_EDGE);

    // Bricks with enhanced visuals
    for(const auto& b: bricks) {
        if(!b.destroyed) {
            const float* brickColor;
            switch(levelNumber) {
                case 1: brickColor = Colors::BRICK_LEVEL1; break;
                case 2: brickColor = Colors::BRICK_LEVEL2; break;
                case 3: brickColor = Colors::BRICK_LEVEL3; break;
                case 4: brickColor = Colors::BRICK_LEVEL4; break;
                case 5: brickColor = Colors::BRICK_LEVEL5; break;
                default: brickColor = Colors::BRICK_LEVEL1; break;
            }
            drawBrickWithShadow(b.x, b.y, brickWidth, brickHeight, brickColor);
        }
    }

    // Draw particles
    drawParticles();

    // Score & Level with enhanced colors
    glColor3f(Colors::SCORE_COLOR[0], Colors::SCORE_COLOR[1], Colors::SCORE_COLOR[2]);
    char scoreText[50]; 
    sprintf(scoreText, "Score: %d", score); 
    drawText(20, 30, scoreText);
    
    // Score change display
    if(scoreChange != 0) {
        float alpha = 1.0f - (scoreChangeTime / 1.0f);
        glColor4f(Colors::SCORE_COLOR[0], Colors::SCORE_COLOR[1], Colors::SCORE_COLOR[2], alpha);
        char changeText[20];
        sprintf(changeText, "+%d", scoreChange);
        drawText(20, 60, changeText);
    }

    glColor3f(Colors::TEXT_COLOR[0], Colors::TEXT_COLOR[1], Colors::TEXT_COLOR[2]);
    char levelText[20]; 
    sprintf(levelText, "Level: %d", levelNumber); 
    drawText(windowWidth - 120, 30, levelText);

    // Level complete message
    if(showLevelComplete) {
        glColor3f(Colors::SCORE_COLOR[0], Colors::SCORE_COLOR[1], Colors::SCORE_COLOR[2]);
        const char* msg = "Level Complete!";
        int textWidth = 0;
        for (const char* c = msg; *c; ++c) {
            textWidth += glutBitmapWidth(GLUT_BITMAP_9_BY_15, *c);
        }
        float centerX = windowWidth / 2.0f;
        float posX = centerX - textWidth / 2.0f;
        drawText(posX, windowHeight / 2, msg);
    }

    if(gameOver){
        glColor3f(Colors::GAME_OVER_COLOR[0], Colors::GAME_OVER_COLOR[1], Colors::GAME_OVER_COLOR[2]);
        const char* msg = "Game Over! Press R to Restart";
        int textWidth = 0;
        for (const char* c = msg; *c; ++c) {
            textWidth += glutBitmapWidth(GLUT_BITMAP_9_BY_15, *c);
        }
        float centerX = windowWidth / 2.0f;
        float posX = centerX - textWidth / 2.0f;
        drawText(posX, windowHeight / 2, msg);
    }
    
    glPopMatrix(); // End screen shake transformation

    glutSwapBuffers();
}

// ----------------- Update -----------------
void update(int val) {
    if (gameOver) {
        glutTimerFunc(16, update, 0);
        glutPostRedisplay();
        return;
    }

    // Update animation time
    animationTime += FRAME_TIME;
    
    // Update particles
    updateParticles();
    
    // Update level complete timer
    if(showLevelComplete) {
        levelCompleteTime += FRAME_TIME;
        if(levelCompleteTime > LEVEL_COMPLETE_DELAY) {
            showLevelComplete = false;
            levelCompleteTime = 0.0f;
        }
    }
    
    // Update screen shake
    if(screenShake > 0) {
        screenShake -= FRAME_TIME * 5.0f;
        if(screenShake < 0) screenShake = 0;
    }
    
    // Update score change display
    if(scoreChange != 0) {
        scoreChangeTime += FRAME_TIME;
        if(scoreChangeTime > 1.0f) {
            scoreChange = 0;
            scoreChangeTime = 0.0f;
        }
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
        // Create paddle particles
        createParticles(ballX, ballY, Colors::PARTICLE_PADDLE, PARTICLE_COUNT_PADDLE);
        // Add screen shake
        screenShake = 0.3f;
    }

    // Brick collision
    for (auto& b : bricks) {
        if (!b.destroyed &&
            ballX >= b.x && ballX <= b.x + brickWidth &&
            ballY >= b.y && ballY <= b.y + brickHeight) {
            dy *= -1;
            b.destroyed = true;
            score += 10;
            scoreChange = 10;
            scoreChangeTime = 0.0f;
            
            // Create brick destruction particles
            createParticles(b.x + brickWidth/2, b.y + brickHeight/2, Colors::PARTICLE_BRICK, PARTICLE_COUNT_BRICK);
            // Add screen shake
            screenShake = 0.2f;
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
        showLevelComplete = true;
        levelCompleteTime = 0.0f;
        // Go to next level after a short delay
        static int levelCompleteTimer = 0;
        levelCompleteTimer++;
        if(levelCompleteTimer > (int)(LEVEL_COMPLETE_DELAY * FPS)) {
            nextLevel();
            levelCompleteTimer = 0;
        }
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
    glClearColor(Colors::DARK_BLUE[0], Colors::DARK_BLUE[1], Colors::DARK_BLUE[2], 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
