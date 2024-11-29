// Example gl_scene
// - Introduces the concept of a dynamic scene of objects
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <dependencies/include/stb_image.h>

#include <../ppgso/ppgso.h>

#include "models/chair.h"
#include "scene.h"
#include "camera.h"
#include "models/lamp.h"
#include "models/room.h"
#include "models/test_backpack.h"
#include <shaders/ddebug_vert_glsl.h>
#include <shaders/ddebug_frag_glsl.h>

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
  if (quadVAO == 0)
  {
    float quadVertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
/*!
 * Custom windows for our simple game
 */
class SceneWindow : public ppgso::Window {
private:
  Scene scene;
  ppgso::Shader debugDepthQuad;

  /*!
   * Reset and initialize the game scene
   * Creating unique smart pointers to objects that are stored in the scene object list
   */
  void initScene() {
    scene.models.clear();
    scene.camera = std::make_unique<Camera>(); // Create a camera
    //inputs
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //Lights
    scene.setUpLights();
    //Shadows
    scene.setUpDepthMap();
    //LODING OBJECTS
    auto backpack = std::make_unique<BPACK>("../data/testpack/backpack.obj");
    auto chair = std::make_unique<Chair>("../data/diff_chair/chair.obj");
    auto room = std::make_unique<Room>("../data/room/room.obj");
    // auto lamp = std::make_unique<Lamp>("../data/lamp/lampa.obj");
    //positioning
    chair->position.x = 5;
    chair->scale = glm::vec3{3};
    backpack->position = {0,1,-2};
    backpack->scale = {.5f,.5f,.5f};
    // room->addChild(std::move(backpack));
    // room->addChild(std::move(lamp));

    //ADDING THEM TO THE SCENE
    scene.models.push_back(std::move(room));
    scene.models.push_back(std::move(chair));
    scene.models.push_back(std::move(backpack));

  }

public:
  /*!
   * Construct custom game window
   */
  SceneWindow() : Window{"apartman", WIDTH, HEIGHT}, debugDepthQuad{ddebug_vert_glsl, ddebug_frag_glsl} {
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

    // process input
    processInput(this->window, dt);


    //Shadow Pass
    scene.shadowPass();

    glViewport(0, 0, WIDTH, HEIGHT);
    // Set gray background
    glClearColor(.1f, .1f, .1f, 1);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Shadow map debugging
    // debugDepthQuad.use();
    // debugDepthQuad.setUniform("near_plane", 1.f);
    // debugDepthQuad.setUniform("far_plane", 20.f);
    // debugDepthQuad.setUniform("depthMap", 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, scene.depthMap);
    // renderQuad();

    scene.camera->update();
    scene.update(dt);
    scene.render();
  }

  void processInput(GLFWwindow *window, float dt)
  {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * dt);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPostion += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPostion -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraPostion -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPostion += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
};

int main() {
  // Initialize our window
  SceneWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
