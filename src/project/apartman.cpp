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
#include <shaders/ddebug_vert_glsl.h>
#include <shaders/ddebug_frag_glsl.h>

#include "models/animated.h"
#include "models/obal.h"
#include "models/static.h"
#include "models/zaves_spod.h"

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

  void loadObjects() {
  /*!
   * Reset and initialize the game scene
   * Creating unique smart pointers to objects that are stored in the scene object list
   */
    //LODING OBJECTS
    auto backpack =     std::make_unique<Static>("../data/testpack/backpack.obj");
    auto room =         std::make_unique<Static>("../data/room/room.obj");
    auto laptop =       std::make_unique<Static>("../data/laptop4/laptop.obj");
    auto book =         std::make_unique<Static>("../data/book/book.obj");
    auto zaves_vrch =   std::make_unique<Static>("../data/zaves/cast1.obj");
    auto roomba =       std::make_unique<Animated>("../data/roomba/roomba.obj");
    auto plane =        std::make_unique<Animated>("../data/plane/plane.obj");
    auto okno =         std::make_unique<Animated>("../data/okno/okno.obj");
    auto chair =        std::make_unique<Chair>("../data/diff_chair/chair.obj");
    auto lamp =         std::make_unique<Lamp>("../data/lamp/lampa.obj");
    auto zaves_spod =   std::make_unique<Zaves>("../data/zaves/cast2.obj");
    auto obal =         std::make_unique<Obal>("../data/book/obal.obj", zaves_spod->fulltime);

    //positioning
    backpack->scale = {0.2, 0.2, 0.2};
    backpack->position = {0.0, 0.85, 0.0};
    backpack->rotation = {-0.3, 0.0, 0.0};
    laptop->position = {-1.2, 0.94, -0.54};
    laptop->rotation = {glm::radians(-101.0f), glm::radians(180.0f), glm::radians(90.0f)};
    book->position = {-0.9, 0.97, -0.9};
    book->scale = {0.25, 0.25, 0.25};
    book->rotation = {0, 0, glm::radians(160.0f)};
    zaves_vrch->position = {-0.37, 3, -1.7};
    zaves_vrch->scale = {1.5, 1, 1};
    zaves_vrch->rotation = {glm::radians(90.0f), 0, 0};
    obal->position = {0.0, 0.06, -0.36};
    lamp->position = {-1.41, 0.94, -1.07};
    lamp->rotation = {0, 0, glm::radians(90.0f)};

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

    float plane_offset = 50.f;
    plane->keyframes.push_back(Keyframe(0.0f, {1.36, 2.09, 2.68}, {0, 0, 0}, {1, 1, 1}));
    plane->keyframes.push_back(Keyframe(plane_offset + 10.0f, {1.36, 2.09, 2.68}, {0, 0, 0}, {1, 1, 1}));
    plane->keyframes.push_back(Keyframe(plane_offset + 14.0f, {0.93, 1.59, -0.05}, {0, glm::radians(15.0f), 0}, {1, 1, 1}));
    plane->keyframes.push_back(Keyframe(plane_offset + 17.0f, {0.35, 1.05, -1.8}, {0, glm::radians(20.0f), glm::radians(15.0f)}, {1, 1, 1}));

    float okno_offset = -7.f;
    okno->keyframes.push_back(Keyframe(0.0f, {-0.3, 1.1, -2.09}, {0, 0, 0}, {1, 1, 1}));
    okno->keyframes.push_back(Keyframe(okno_offset + 10.0f, {-0.3, 1.1, -2.09}, {0, 0, 0}, {1, 1, 1}));
    okno->keyframes.push_back(Keyframe(okno_offset + 20.0f, {-0.3, 1.94, -2.09}, {0, 0, 0}, {1, 1, 1}));

    scene.anicam->keyframe_position.push_back(cam_keyframe(0.f, {0.67, 1.67, 3.36}, {0,0,0}));
    scene.anicam->keyframe_position.push_back(cam_keyframe(30.f, {-0.720415, 1.17124,0.60603}, {0,0,0}));
    scene.anicam->keyframe_position.push_back(cam_keyframe(40.f, {-1.26455, 1.72814, -0.169158}, {0,0,0}));
    scene.anicam->keyframe_position.push_back(cam_keyframe(50.f, {-0.870189, 1.f, -1.3531}, {0,0,0}));
    scene.anicam->keyframe_position.push_back(cam_keyframe(70.f, {-1.43896, 1.07107,-0.748533}, {0,0,0}));

    scene.anicam->keyframe_target.push_back(cam_keyframe(0.f, {0,0,0}, {-0.247105, 0.38785, -0.556736}));
    scene.anicam->keyframe_target.push_back(cam_keyframe(20.f, {0,0,0}, {-0.247105, 0.38785, -0.556736}));
    scene.anicam->keyframe_target.push_back(cam_keyframe(40.f, {0,0,0}, {-0.571761, 1.0181,-0.70207}));
    scene.anicam->keyframe_target.push_back(cam_keyframe(50.f, {0,0,0}, {-0.28732, 1.10886, -1.8558}));

    // scene.anicam->keyframe_position.push_back(cam_keyframe(0.f, {0.67, 1.67, 3.36}, {-0.247105, 0.38785, -0.556736}));
    // scene.anicam->keyframe_position.push_back(cam_keyframe(20.f, {-0.720415, 1.17124,0.60603}, {-0.247105, 0.38785, -0.556736}));

    room->addChild(std::move(lamp));
    chair->addChild(std::move(backpack));
    room->addChild(std::move(chair));
    room->addChild(std::move(laptop));
    room->addChild(std::move(roomba));
    book->addChild(std::move(obal));
    room->addChild(std::move(book));
    room->addChild(std::move(okno));
    zaves_vrch->addChild(std::move(zaves_spod));
    room->addChild(std::move(zaves_vrch));

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
    scene.anicam = std::make_unique<Anicam>();
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    scene.setUpLights();
    //Shadows
    scene.setUpDepthMap();
    scene.setUpDepthMap();
    scene.computeDepthCubemapMatrix(scene.pointLights[0].position, scene.pointLights[0].far_plane);
    scene.computeDepthCubemapMatrix(scene.pointLights[1].position, scene.pointLights[1].far_plane);
    scene.computeDepthCubemapMatrix(scene.pointLights[2].position, scene.pointLights[2].far_plane);

    //LODING OBJECTS
    loadObjects();
  }

public:
  /*!
   * Construct custom game window
   */
  SceneWindow() : Window{"apartman", WIDTH, HEIGHT}, debugDepthQuad{ddebug_vert_glsl, ddebug_frag_glsl, ""} {
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
    scene.cubemapPass();

    glViewport(0, 0, WIDTH, HEIGHT);
    std::cout << cameraPostion.x << ", " << cameraPostion.y << "," << cameraPostion.z << std::endl;
    // Set gray background
    glClearColor(.2f, .2f, .2f, 1);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Shadow map debugging
    // debugDepthQuad.use();
    // debugDepthQuad.setUniform("near_plane", .1f);
    // debugDepthQuad.setUniform("far_plane", 20.f);
    // debugDepthQuad.setUniform("depthMap", 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, scene.depthMapTex_dir[0]);
    // renderQuad();

    scene.camera->update();
    scene.anicam->update(dt);
    scene.update(dt);
    scene.shader.use();
    // scene.shader.setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    // scene.shader.setUniform("ViewMatrix", scene.camera->viewMatrix);
    // scene.shader.setUniform("viewPos", cameraPostion);
    scene.shader.setUniform("ProjectionMatrix", scene.anicam->projectionMatrix);
    scene.shader.setUniform("ViewMatrix", scene.anicam->viewMatrix);
    scene.shader.setUniform("viewPos", scene.anicam->position);
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
