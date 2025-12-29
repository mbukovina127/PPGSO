# OpenGL Apartment Scene

A 3D animated apartment scene demonstration built with C++ and OpenGL, featuring custom model loading, advanced lighting with shadows, keyframe animation, and particle systems.

<img src='PPGSO_GIF.gif' alt="Demo showcase">

## Overview

This project showcases various computer graphics techniques including hierarchical scene representation, Phong lighting model with multiple light sources, shadow mapping (both directional and point light shadows), procedural and keyframe-based animation, and efficient instancing for particle effects.

## Features

### 3D Rendering
- **Custom Model Loader**: Assimp-based 3D model importer with texture support
- **UV Texture Mapping**: Proper texture coordinates for realistic surface appearance
- **Efficient Instancing**: 10,000+ snowflake particles rendered using GPU instancing
- **Skybox**: Immersive environment mapping

### Scene Management
- **Hierarchical Scene Graph**: Parent-child relationships between objects
- **Modular Object System**: Abstract base class with specialized object types (Static, Animated, Chair, Lamp, etc.)
- **Dynamic Scene Updates**: Real-time object transformations and animations

### Animation
- **Keyframe Animation**: Smooth interpolation between defined keyframes for objects and camera
- **Procedural Animation**: Mathematical functions for dynamic movement (book cover, curtains)
- **Camera Animation**: Cinematic camera paths through the scene
- **Custom Easing Functions**: Non-linear interpolation for natural motion

### Lighting & Shadows
- **Multiple Light Types**: 
  - Directional lights (sun-like illumination)
  - Point lights (omni-directional light sources)
- **Phong Lighting Model**: Ambient, diffuse, and specular components
- **Shadow Mapping**: 
  - Directional shadow maps with PCF (Percentage Closer Filtering)
  - Cubemap shadows for point lights
- **Material System**: Per-mesh materials with customizable properties

### Particle System
- **GPU-Accelerated Particles**: Instanced rendering of 10,000 snowflakes
- **Particle Animation**: Falling snow effect with velocity-based updates

## Technical Details

### Graphics Pipeline

**Vertex Shader**
- Model-View-Projection transformations
- Normal transformations for lighting
- Light space transformations for shadow mapping (up to 6 directional lights)

**Fragment Shader** (Main Lighting Shader)
- Phong lighting calculation for multiple lights
- Shadow map sampling with PCF for soft shadows
- Cubemap shadow sampling for point lights
- Texture and material blending

**Shadow Pass**
- Directional light shadow maps (2048×2048 resolution)
- Point light cubemap shadows (6 faces per light)
- Separate geometry shader for cubemap generation

### Architecture

```
Scene
├── Camera (First-person controller)
├── AniCam (Animated camera with keyframes)
├── Models (Scene objects)
│   ├── Static (Non-animated objects)
│   ├── Animated (Keyframe-based animation)
│   ├── Chair (Custom easing function)
│   ├── Lamp (Light-emitting object)
│   └── Custom objects (Obal, Zaves, etc.)
├── Lights
│   ├── DirectionalLight[]
│   └── PointLight[]
└── Skybox
```

### Object Hierarchy Example
```
room
├── lamp
├── chair
│   └── backpack
├── laptop
├── roomba
├── book
│   └── obal (book cover)
├── okno (window)
└── zaves_vrch (curtain top)
    └── zaves_spod (curtain bottom)
```

## Requirements

### Dependencies
- **OpenGL 3.3+**
- **GLEW** - OpenGL Extension Wrangler
- **GLFW3** - Window and input management
- **GLM** - Mathematics library
- **Assimp** - 3D model loading
- **stb_image** - Image loading for textures

### Build Tools
- CMake 3.5+
- C++14 compatible compiler (GCC, Clang, MSVC)

## Installation

### Windows (MinGW/MSVC)

1. Install Mingw64, cmake & Ninja. The recommended way is using MYSYS2 (https://www.msys2.org)
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja
```

2. Dependencies are bundled in the `dependencies/` folder

3. Build with CMake:
```bash
cmake -S .. -B build -G Ninja
cmake --build build
```

## Usage

Run the compiled executable:
```bash
./build/main
```

### Controls
There are two possible view modes one is free movement and the other is presentation mode.
The demo is currently set to presenatation mode. To change it got the file `apartment.cpp` and locate the camera matrix and position variables located at line `239` to the ones from the `camera` object and not `anicam` object

#### Free camera controls
- **W/A/S/D** - Move camera forward/left/backward/right
- **Mouse Movement** - Look around
- **Mouse Scroll** - Adjust field of view
- **ESC** - Exit application

### Scene Features

The scene demonstrates:
- **Animated roomba** moving along a predefined path
- **Rotating chair** with custom easing
- **Animated paper airplane** flying through the window
- **Opening window** animation
- **Book cover** with physics-like animation tied to curtain movement
- **Falling snow** particle system (10,000 instances)
- **Dynamic lighting** with shadows from multiple sources

## Project Structure

```
.
├── src/
│   └── project/
│       ├── apartman.cpp       # Main application
│       ├── scene.h            # Scene management
│       ├── camera.h           # Camera implementation
│       ├── anicam.h           # Animated camera
│       ├── model.h            # Base model class
│       ├── mesh.h             # Mesh representation
│       ├── imgloader.cpp/h    # Texture loading
│       ├── skybox.h           # Skybox rendering
│       └── models/            # Specialized model classes
│           ├── static.h       # Static objects
│           ├── animated.h     # Animated objects
│           ├── chair.h        # Chair with custom animation
│           ├── lamp.h         # Light-emitting object
│           ├── snowflake.h    # Particle system
│           └── ...
├── shader/                    # GLSL shader files
│   ├── light_vert.glsl       # Main vertex shader
│   ├── light_frag.glsl       # Main fragment shader
│   ├── depth_vert.glsl       # Shadow depth vertex
│   ├── depth_frag.glsl       # Shadow depth fragment
│   ├── cubemap_*.glsl        # Cubemap shadow shaders
│   └── ...
├── data/                      # 3D models and textures
├── dependencies/              # Third-party libraries (Windows)
├── CMakeLists.txt            # Build configuration
└── README.md
```

## Implementation Highlights

### Shadow Mapping

Directional light shadows use standard shadow mapping with PCF:
```cpp
float calcDirShadow(sampler2D shadowMap, vec4 fpLightSpace, vec3 ldirection) {
    vec3 projCoords = fpLightSpace.xyz / fpLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}
```

Point light shadows use cubemap depth maps for omnidirectional shadows.

### Keyframe Animation

Objects interpolate between keyframes for smooth animation:
```cpp
struct Keyframe {
    float time;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

void interpolateKeyframes(float dt) {
    animationTime += dt;
    // Find current keyframe pair and interpolate
    float t = (animationTime - prev->time) / (next->time - prev->time);
    position = glm::mix(prev->position, next->position, t);
    rotation = glm::mix(prev->rotation, next->rotation, t);
    scale = glm::mix(prev->scale, next->scale, t);
}
```

### Efficient Instancing

Snowflakes use GPU instancing for optimal performance:
```cpp
// Instance buffer with transformation matrices
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glBufferData(GL_ARRAY_BUFFER, instanceTransforms.size() * sizeof(glm::mat4), 
             &instanceTransforms[0], GL_DYNAMIC_DRAW);

// Single draw call for all instances
glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, 
                        instanceTransforms.size());
```

## Academic Context

This project was developed as coursework for Computer Graphics Programming at STU FIIT (Slovak University of Technology in Bratislava, Faculty of Informatics and Information Technologies).

**Course**: Computer Graphics Programming  
**Authors**: Iveta Lešková & Marko Bukovina  
**Academic Year**: 2024

## Credits and Resources

This project was developed with guidance from the following resources:

- **[LearnOpenGL.com](https://learnopengl.com/)** - Comprehensive OpenGL tutorials covering lighting, shadow mapping, model loading, and advanced techniques. This resource was instrumental in understanding:
  - Phong lighting implementation
  - Shadow mapping (both directional and point light shadows)
  - Model loading with Assimp
  - Texture loading and management
  - Camera systems
  - Coordinate spaces and transformations

- **Assimp** - Open Asset Import Library for 3D model loading
- **stb_image** - Single-header image loading library by Sean Barrett
- **GLM** - OpenGL Mathematics library
- **GLFW** - Multi-platform library for OpenGL window and input
- **GLEW** - OpenGL Extension Wrangler Library

## Known Limitations
- No collision detection between objects
- Limited to 4 point lights and 6 directional lights (shader defines)
- No post-processing effects implemented
- Single-threaded rendering pipeline


This project is academic coursework. Please consult course guidelines for usage and distribution.

## Contact

- **Marko Bukovina** - [GitHub](https://github.com/mbukovina127/PPGSO)
- **Iveta Lešková** - Collaborator

For questions or feedback about this project, please open an issue on GitHub.