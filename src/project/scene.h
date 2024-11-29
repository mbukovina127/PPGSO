#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "camera.h"
#include "model.h"


#include <shaders/light_vert_glsl.h>
#include <shaders/light_frag_glsl.h>
#include <shaders/depth_vert_glsl.h>
#include <shaders/depth_frag_glsl.h>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class Scene {
public:
    ppgso::Shader shader = ppgso::Shader(light_vert_glsl, light_frag_glsl);
    ppgso::Shader depthshader = ppgso::Shader(depth_vert_glsl, depth_frag_glsl);
    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<Model>> models;
    GLuint depthMapFBO;
    GLuint depthMap;

    glm::vec3 light1_direction = {2, 2, -3};

    void setUpDepthMap() {

        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cerr << "Framebuffer is not complete!" << endl;

    }

    auto getlightSpaceMatrix(glm::vec3 light_position) {
        float near_plane = 1.0f, far_plane = 20.0f;
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(light_position * 2.f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 lSpaceMatrix = lightProjection * lightView;
        depthshader.setUniform("lSpaceMatrix", lSpaceMatrix);
        return lSpaceMatrix;

    }

    void shadowPass() {
        depthshader.use();

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            // glCullFace(GL_FRONT);
            glClear(GL_DEPTH_BUFFER_BIT);
            getlightSpaceMatrix(light1_direction);
        // RenderScene()
            for ( auto& model : models ) { model->renderDepth(depthshader); }
        // please work
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glCullFace(GL_BACK);
    }

    void setUpLights() {
        shader.use();
        // Set lights
        int numofdirlights = 1;
        shader.setUniform("numDirL", numofdirlights);
        // Light 1
        shader.setUniform("DLIGHTS[0].direction", light1_direction);
        shader.setUniform("DLIGHTS[0].base.color", glm::vec3(1, 1, 1));
        shader.setUniform("DLIGHTS[0].base.ambI", 0.1f);
        shader.setUniform("DLIGHTS[0].base.difI", 1.0f);

        // // Light 2
        // shader.setUniform("DLIGHTS[1].direction", glm::vec3(-2, 3, -2));
        // shader.setUniform("DLIGHTS[1].base.color", glm::vec3(1, 0, 0));
        // shader.setUniform("DLIGHTS[1].base.ambI", 0.05f);
        // shader.setUniform("DLIGHTS[1].base.difI", 0.2f);
        //
        //
        // int numofpointlights = 2;
        // shader.setUniform("numPointL", numofpointlights);
        //
        // //p light 1
        // shader.setUniform("PLIGHTS[0].position", glm::vec3(2, 2, 2));
        // shader.setUniform("PLIGHTS[0].base.color", glm::vec3(1, 1, 1));
        // shader.setUniform("PLIGHTS[0].base.ambI", 0.1f);
        // shader.setUniform("PLIGHTS[0].base.difI", 0.9f);
        // shader.setUniform("PLIGHTS[0].constant", 0.1f);
        // shader.setUniform("PLIGHTS[0].linear", 0.09f);
        // shader.setUniform("PLIGHTS[0].quadratic", 0.032f);
        //
        // //p light 2
        // shader.setUniform("PLIGHTS[1].position", glm::vec3(-0.5, 1, 0));
        // shader.setUniform("PLIGHTS[1].base.color", glm::vec3(1, 0.8f, 0.8f));
        // shader.setUniform("PLIGHTS[1].base.ambI", 0.1f);
        // shader.setUniform("PLIGHTS[1].base.difI", 0.8f);
        // shader.setUniform("PLIGHTS[1].constant", 0.1f);
        // shader.setUniform("PLIGHTS[1].linear", 0.09f);
        // shader.setUniform("PLIGHTS[1].quadratic", 0.032f);
    }

    void update(float time) {
        camera->update();

        // Use iterator to update all objects so we can remove while iterating
        auto i = std::begin(models);

        while (i != std::end(models)) {
            // Update and remove from list if needed
            auto model = i->get();
            if (!model->update(*this, time))
                i = models.erase(i); // NOTE: no need to call destructors as we store shared pointers in the scene
            else
                ++i;
        }
    }


    void render() {
        shader.use();
        shader.setUniform("ProjectionMatrix", camera->projectionMatrix);
        shader.setUniform("ViewMatrix", camera->viewMatrix);
        shader.setUniform("viewPos", cameraPostion);
        shader.setUniform("lSpaceMatrix", getlightSpaceMatrix(light1_direction));
        shader.setUniform("shadows", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        for ( auto& model : models )
            model->render(*this);
    }

};

#endif //SCENE_H

