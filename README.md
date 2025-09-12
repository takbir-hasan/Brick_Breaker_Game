# Brick Breaker Game 🎮

A modern, visually enhanced Brick Breaker game developed using C++ and OpenGL with stunning visual effects and animations.

## Features ✨

- **Modern Visual Design**: Beautiful gradient backgrounds, shadows, and particle effects
- **5 Progressive Levels**: Each with unique patterns and increasing difficulty
- **Particle System**: Dynamic particle effects for collisions and brick destruction
- **Screen Shake Effects**: Immersive feedback for collisions
- **Smooth Animations**: Fluid ball movement and visual transitions
- **Progress Saving**: Automatic level progress persistence
- **Enhanced UI**: Modern color palette and improved text rendering

## Visual Enhancements 🎨

- **Gradient Backgrounds**: Dynamic blue gradient background
- **3D Brick Effects**: Shadows and highlights for depth
- **Particle Effects**: Colorful particles on collisions
- **Screen Shake**: Impact feedback for paddle and brick hits
- **Modern Color Palette**: Carefully selected colors for each level
- **Smooth Animations**: 60 FPS gameplay with fluid motion

## Controls 🎯

- **Arrow Keys**: Move paddle left/right
- **R Key**: Restart current level
- **ESC**: Exit game

## Compilation & Run 🚀

### Windows (MinGW/MSYS2)
```bash
g++ main.cpp level1.cpp level2.cpp level3.cpp level4.cpp level5.cpp -o main.exe -lfreeglut -lopengl32 -lglu32
./main.exe
```

### Linux
```bash
g++ main.cpp level1.cpp level2.cpp level3.cpp level4.cpp level5.cpp -o main -lGL -lGLU -lglut
./main
```

### macOS
```bash
g++ main.cpp level1.cpp level2.cpp level3.cpp level4.cpp level5.cpp -o main -framework OpenGL -framework GLUT
./main
```

## Level Progression 📈

1. **Level 1**: Pyramid pattern (Green bricks) - Easy
2. **Level 2**: Regular grid (Red bricks) - Medium
3. **Level 3**: Dense grid (Orange bricks) - Hard
4. **Level 4**: Diamond pattern (Purple bricks) - Expert
5. **Level 5**: Complex pattern with gaps (Blue bricks) - Master

## Technical Details 🔧

- **Language**: C++17
- **Graphics**: OpenGL with GLUT
- **Architecture**: Modular level system
- **Performance**: 60 FPS with particle effects
- **Memory**: Efficient particle management with automatic cleanup
