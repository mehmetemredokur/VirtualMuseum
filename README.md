# Virtual Museum

This project is a C++/OpenGL application that simulates a virtual museum environment with 5 3D models, lighting, a scanning robot, and an ImGui-based UI.

## Project Structure

- `models/`: Place your `.obj` and `.mtl` files here.
- `shaders/`: Contains GLSL vertex and fragment shaders.
- `src/`: Source code for the application.
- `CMakeLists.txt`: Build configuration.

## Build & Run

```bash
mkdir build && cd build
cmake ..
make
./VirtualMuseum
```

Controls:
- **ESC**: Exit
- **Arrow keys**: Manual robot control
- **Mouse**: Interact with UI
