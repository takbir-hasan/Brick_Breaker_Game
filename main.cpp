#include <GL/glut.h>
#include <vector>
#include <cstdio>
#include <cmath>

struct Brick {
    float x, y;
    bool destroyed = false;
};

const int windowWidth = 600, windowHeight = 500;

float paddleX = 250;
float paddleWidth = 100, paddleHeight = 10;

float ballX = 300, ballY = 250;
float ballRadius = 8;
float dx = 3, dy = 4;

std::vector<Brick> bricks;
int rows = 5, cols = 8;
float brickWidth = 60, brickHeight = 20;

int score = 0;
bool gameOver = false;

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x+w, y);
    glVertex2f(x+w, y+h); glVertex2f(x, y+h);
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

void resetGame() {
    bricks.clear();
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            bricks.push_back({ c * (brickWidth + 10) + 35, r * (brickHeight + 10) + 40 });
        }
    }
    ballX = 300;
    ballY = 250;
    dx = 3; dy = 4;
    score = 0;
    gameOver = false;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0, 1);
    drawRect(paddleX, windowHeight - 30, paddleWidth, paddleHeight);

    glColor3f(1, 1, 1);
    drawCircle(ballX, ballY, ballRadius);

    for (const auto& b : bricks) {
        if (!b.destroyed) {
            glColor3f(0, 1, 0);
            drawRect(b.x, b.y, brickWidth, brickHeight);
        }
    }

    glColor3f(1, 1, 1);
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    drawText(10, 10, scoreText);

    if (gameOver) {
        glColor3f(1, 0, 0);
        drawText(200, 250, "Game Over! Press R to Restart");
    }

    glutSwapBuffers();
}

void update(int val) {
    if (gameOver) {
        glutTimerFunc(16, update, 0);
        glutPostRedisplay();
        return;
    }

    ballX += dx;
    ballY += dy;

    // Wall collision
    if (ballX < 0 || ballX > windowWidth) dx *= -1;
    if (ballY < 0) dy *= -1;
    if (ballY > windowHeight) gameOver = true;

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

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int, int) {
    if (key == 'r' || key == 'R') resetGame();
}

void special(int key, int, int) {
    if (key == GLUT_KEY_LEFT && paddleX > 0)
        paddleX -= 15;
    if (key == GLUT_KEY_RIGHT && paddleX + paddleWidth < windowWidth)
        paddleX += 15;
}

void init() {
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    glClearColor(0, 0, 0, 1);
    resetGame();
}

int main(int argc, char** argv) {
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
