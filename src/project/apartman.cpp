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


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

/*!
 * Custom windows for our simple game
 */
class SceneWindow : public ppgso::Window {
private:
  Scene scene;

  void loadObjects() {
    auto backpack = std::make_unique<BPACK>("../data/testpack/backpack.obj");
    auto chair = std::make_unique<Chair>("../data/diff_chair/chair.obj");
    auto room = std::make_unique<Room>("../data/room2/room.obj");
    auto lamp = std::make_unique<Lamp>("../data/lamp/lampa.obj");
    auto laptop = std::make_unique<Lamp>("../data/laptop4/laptop.obj");
    auto roomba = std::make_unique<Chair>("../data/roomba/roomba.obj");
    auto plane = std::make_unique<Chair>("../data/plane/plane.obj");
    auto book = std::make_unique<Chair>("../data/book/book.obj");
    auto obal = std::make_unique<Chair>("../data/book/obal.obj");
    //positioning
    backpack->scale = {0.2, 0.2, 0.2};
    backpack->position = {0.0, 0.85, 0.0};
    backpack->rotation = {-0.3, 0.0, 0.0};
    laptop->position = {-1.04, 0.95, -0.54};
    laptop->rotation = {0, 0, 0}; //otočiť
    book->position = {-0.98, 0.97, -0.82};
    book->scale = {0.25, 0.25, 0.25};
    book->rotation = {0, 0, glm::radians(160.0f)};

    //animation
    chair->keyframes.push_back(Keyframe(0.0f, {0.2,0,-0.5}, {0, 0, glm::radians(-30.0f)}, {1.2, 1.2, 1.2}));
    chair->keyframes.push_back(Keyframe(10.0f, {0.2,0,-0.5}, {0, 0, glm::radians(-30.0f)}, {1.2, 1.2, 1.2}));
    chair->keyframes.push_back(Keyframe(20.0f, {0.2,0,-0.5}, {0, 0, glm::radians(30.0f)}, {1.2, 1.2, 1.2}));

    roomba->keyframes.push_back(Keyframe(0.0f, {-0.01, 0, 1.36}, {0, 0, glm::radians(90.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(10.0f, {-0.01, 0, 1.36}, {0, 0, glm::radians(90.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(15.0f, {-0.94, 0, 1.36}, {0, 0, glm::radians(90.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(17.0f, {-0.94, 0, 1.36}, {0, 0, glm::radians(0.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(21.0f, {-0.94, 0, 0.5}, {0, 0, glm::radians(0.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(23.0f, {-0.94, 0, 0.5}, {0, 0, glm::radians(-90.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(33.0f, {1.38, 0, 0.5}, {0, 0, glm::radians(-90.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(35.0f, {1.38, 0, 0.5}, {0, 0, glm::radians(0.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(42.0f, {1.38, 0, -1}, {0, 0, glm::radians(0.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(44.0f, {1.38, 0, -1}, {0, 0, glm::radians(-90.0f)}, {1.2, 1.2, 1.2}));
    roomba->keyframes.push_back(Keyframe(50.0f, {2.76, 0, -1}, {0, 0, glm::radians(-90.0f)}, {1.2, 1.2, 1.2}));

    plane->keyframes.push_back(Keyframe(0.0f, {1.36, 2.09, 2.68}, {0, 0, 0}, {1, 1, 1}));
    plane->keyframes.push_back(Keyframe(10.0f, {1.36, 2.09, 2.68}, {0, 0, 0}, {1, 1, 1}));
    plane->keyframes.push_back(Keyframe(14.0f, {0.93, 1.59, -0.05}, {0, glm::radians(15.0f), 0}, {1, 1, 1}));
    plane->keyframes.push_back(Keyframe(17.0f, {0.35, 1.05, -1.8}, {0, glm::radians(20.0f), glm::radians(15.0f)}, {1, 1, 1}));

    obal->keyframes.push_back(Keyframe(0.0f, {0.0, 0.06, -0.36}, {0, 0, 0}, {1, 1, 1}));
    obal->keyframes.push_back(Keyframe(5.0f, {0.0, 0.06, -0.36}, {0, 0, 0}, {1, 1, 1}));
    obal->keyframes.push_back(Keyframe(6.0f, {0.0, 0.06, -0.36}, {glm::radians(-20.0f), 0, 0}, {1, 1, 1}));
    obal->keyframes.push_back(Keyframe(7.0f, {0.0, 0.06, -0.36}, {0, 0, 0}, {1, 1, 1}));

    room->addChild(std::move(lamp));
    chair->addChild(std::move(backpack));
    room->addChild(std::move(chair));
    room->addChild(std::move(laptop));
    room->addChild(std::move(roomba));
    book->addChild(std::move(obal));
    room->addChild(std::move(book));
    //ADDING THEM TO THE SCENE
    scene.models.push_back(std::move(room));
    scene.models.push_back(std::move(plane));
  }
  /*!
   * Reset and initialize the game scene
   * Creating unique smart pointers to objects that are stored in the scene object list
   */
  void initScene() {
    scene.models.clear();
    // Create a camera
    scene.camera = std::make_unique<Camera>();
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //LODING OBJECTS
    loadObjects();
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

    // process input
    processInput(this->window, dt);

    std::cout << cameraPostion.x << " " << cameraPostion.y << " " << cameraPostion.z << std::endl;
    // Set gray background
    glClearColor(.5f, .5f, .5f, 1);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    scene.camera->update();

    scene.update(dt);
    scene.render();
    // Update and render all objects
    // scene.update(dt);
    // scene.render();
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
