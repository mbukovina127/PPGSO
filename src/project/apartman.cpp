// Example gl_scene
// - Introduces the concept of a dynamic scene of objects
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <map>
#include <list>

#include <../ppgso/ppgso.h>

#include "camera.h"
#include "scene.h"

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
    scene.objects.clear();

    // Create a camera
    //auto camera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f); nejde

    // Add space background
    //scene.objects.push_back(std::make_unique<Space>());

    // Add generator to scene?

    // Add player to the scene?
  }

public:
  /*!
   * Construct custom game window
   */
  SceneWindow() : Window{"apartman", WIDTH, HEIGHT} {
    //hideCursor();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

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

  /*!
   * Handles pressed key when the window is focused
   * @param key Key code of the key being pressed/released
   * @param scanCode Scan code of the key being pressed/released
   * @param action Action indicating the key state change
   * @param mods Additional modifiers to consider

  void onKey(int key, int scanCode, int action, int mods) override {

  }
  */


  /*!
   * Handle cursor position changes
   * @param cursorX Mouse horizontal position in window coordinates
   * @param cursorY Mouse vertical position in window coordinates

  void onCursorPos(double cursorX, double cursorY) override {
    scene.cursor.x = cursorX;
    scene.cursor.y = cursorY;
  }
  */

  /*!
   * Handle cursor buttons
   * @param button Mouse button being manipulated
   * @param action Mouse bu
   * @param mods

  void onMouseButton(int button, int action, int mods) override {

  }
  */

  /*!
   * Window update implementation that will be called automatically from pollEvents
   */
  void onIdle() override {
    // Track time
    static auto time = (float) glfwGetTime();

    // Compute time delta
    float dt =  (float) glfwGetTime() - time ;

    time = (float) glfwGetTime();

    // Set gray background
    glClearColor(.5f, .5f, .5f, 0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render all objects
    scene.update(dt);
    scene.render();
  }
};

int main() {
  // Initialize our window
  SceneWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
