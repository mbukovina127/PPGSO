// Example gl_scene
// - Introduces the concept of a dynamic scene of objects
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <stb_image.h>

#include <../ppgso/ppgso.h>

#include "scene.h"
#include "test_backpack.h"


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

/*!
 * Custom windows for our simple game
 */
class SceneWindow : public ppgso::Window {
private:
  Scene scene;

  /*!
   * Reset and initialize the game scene
   * Creating unique smart pointers to objects that are stored in the scene object list
   */
  void initScene() {

    stbi_set_flip_vertically_on_load(true);
    scene.objects.clear();
    // Create a camera
    // auto camera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f); nejde

    //add obejcts to scene
    auto backpack = std::make_unique<BPACK>("../data/testpack/backpack.obj");
    backpack->scale = {0.5, 0.5, 0.5};
    scene.objects.push_back(std::move(backpack));

  }

public:
  /*!
   * Construct custom game window
   */
  SceneWindow() : Window{"apartman", WIDTH, HEIGHT} {
    //hideCursor();
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    initScene();
  }

  void onIdle() override {
    // Track time
    static auto time = (float) glfwGetTime();

    // Compute time delta
    float dt =  (float) glfwGetTime() - time ;

    time = (float) glfwGetTime();

    // Set gray background
    glClearColor(.5f, .5f, .5f, 1);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    scene.camera->update();

    scene.shader->use();
    scene.shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.update(dt);
    scene.objects[0]->render(scene);
    // Update and render all objects
    // scene.update(dt);
    // scene.render();
  }
};

int main() {
  // Initialize our window
  SceneWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
