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

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

struct BaseLight {
    glm::vec3 color;
    float ambI;
    float difI;
};

struct DirectionalLight {
    BaseLight base;
    glm::vec3 direction;
};

struct PointLight {
    BaseLight base;
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
};

class Scene {
public:
    ppgso::Shader shader = ppgso::Shader(light_vert_glsl, light_frag_glsl);
    ppgso::Shader depthshader = ppgso::Shader(depth_vert_glsl, depth_frag_glsl);
    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<Model>> models;
    //lights
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
    //shadow maps
    std::vector<GLuint> depthMapFBOS_dir;
    std::vector<GLuint> depthMapTex_dir;
    std::vector<glm::mat4> lSpaceMatrices;
    // GLuint depthMapFBO;
    // GLuint depthMap;


    void setUpLights() {
        directionalLights = {
            {
                {{1,0.4,0.4}, .1f, 1.f}, {-2, 5, -4}
            },
            {
                {{0.4,1,0.4}, .1f, 1.f}, {4, 5, -4}
            }
        };
        shader.use();
        for (int i = 0; i < directionalLights.size(); i++) {
            shader.setUniform("DLIGHTS[" + std::to_string(i) + "].direction", directionalLights[i].direction);
            shader.setUniform("DLIGHTS[" + std::to_string(i) + "].base.color", directionalLights[i].base.color);
            shader.setUniform("DLIGHTS[" + std::to_string(i) + "].base.ambI", directionalLights[i].base.ambI);
            shader.setUniform("DLIGHTS[" + std::to_string(i) + "].base.difI", directionalLights[i].base.difI);
        }
        shader.setUniform("numDirL", (GLint)directionalLights.size());
        // Set lights
        // int numofdirlights = 1;
        // shader.setUniform("numDirL", numofdirlights);
        // // Light 1
        // shader.setUniform("DLIGHTS[0].direction", light1_direction);
        // shader.setUniform("DLIGHTS[0].base.color", glm::vec3(1, 1, 1));
        // shader.setUniform("DLIGHTS[0].base.ambI", 0.1f);
        // shader.setUniform("DLIGHTS[0].base.difI", 1.0f);

        // // Light 2
        // shader.setUniform("DLIGHTS[1].direction", glm::vec3(-2, 3, -2));
        // shader.setUniform("DLIGHTS[1].base.color", glm::vec3(1, 0, 0));
        // shader.setUniform("DLIGHTS[1].base.ambI", 0.05f);
        // shader.setUniform("DLIGHTS[1].base.difI", 0.2f);
        //
        //
        // int numofpointlights = 1;
        // shader.setUniform("numPointL", numofpointlights);
        //
        // // p light 1
        // shader.setUniform("PLIGHTS[0].position", glm::vec3(0, 2, 0));
        // shader.setUniform("PLIGHTS[0].base.color", glm::vec3(1, 0.9, 0.9));
        // shader.setUniform("PLIGHTS[0].base.ambI", 0.05f);
        // shader.setUniform("PLIGHTS[0].base.difI", 0.5f);
        // shader.setUniform("PLIGHTS[0].constant", 0.0f);
        // shader.setUniform("PLIGHTS[0].linear", 0.15f);
        // shader.setUniform("PLIGHTS[0].quadratic", 0.032f);

        // //p light 2
        // shader.setUniform("PLIGHTS[1].position", glm::vec3(-0.5, 1, 0));
        // shader.setUniform("PLIGHTS[1].base.color", glm::vec3(1, 0.8f, 0.8f));
        // shader.setUniform("PLIGHTS[1].base.ambI", 0.1f);
        // shader.setUniform("PLIGHTS[1].base.difI", 0.8f);
        // shader.setUniform("PLIGHTS[1].constant", 0.1f);
        // shader.setUniform("PLIGHTS[1].linear", 0.09f);
        // shader.setUniform("PLIGHTS[1].quadratic", 0.032f);
    }

    void setUpDepthMap() {
        depthMapFBOS_dir.resize(directionalLights.size());
        depthMapTex_dir.resize(directionalLights.size());

        glGenFramebuffers(directionalLights.size(), depthMapFBOS_dir.data());
        glGenTextures(directionalLights.size(), depthMapTex_dir.data());

        for (int i = 0; i < directionalLights.size(); i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOS_dir[i]);
            glGenTextures(1, &depthMapTex_dir[i]);
            glBindTexture(GL_TEXTURE_2D, depthMapTex_dir[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex_dir[i], 0);

            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cerr << "Framebuffer " << i << " is not complete!" << std::endl;
        }

        // // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        // glGenFramebuffers(1, &depthMapFBO);
        // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        //
        // glGenTextures(1, &depthMap);
        // glBindTexture(GL_TEXTURE_2D, depthMap);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        //
        // glDrawBuffer(GL_NONE);
        // glReadBuffer(GL_NONE);
        // // Always check that our framebuffer is ok
        // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        //     cerr << "Framebuffer is not complete!" << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


    }

    void computeLightSpaceMatrices() {
        lSpaceMatrices.clear();
        for (const auto& dLights : directionalLights) {
            auto direction = dLights.direction;
            float near_plane = 0.1f, far_plane = 20.0f;
            glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(glm::normalize(direction) * 7.5f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            lSpaceMatrices.push_back(lightProjection * lightView);
        }
    }

    auto getlightSpaceMatrix(glm::vec3 light_position) {
        float near_plane = 0.1f, far_plane = 20.0f;
        glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(glm::normalize(light_position) * 5.f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 lSpaceMatrix = lightProjection * lightView;
        depthshader.setUniform("lSpaceMatrix", lSpaceMatrix);
        return lSpaceMatrix;

    }

    void shadowPass() {
        depthshader.use();

        computeLightSpaceMatrices();

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        // //SINGLE
        // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        //     glClear(GL_DEPTH_BUFFER_BIT);
        //     getlightSpaceMatrix(light1_direction);
        //     for ( auto& model : models ) { model->renderDepth(depthshader); }
        //MULTIPLE
        for (int i = 0; i < depthMapFBOS_dir.size(); ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOS_dir[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            depthshader.setUniform("lSpaceMatrix", lSpaceMatrices[i]);
            for (auto& model : models) {
                model->renderDepth(depthshader);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glCullFace(GL_BACK);
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
        for (int i = 0; i < directionalLights.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, depthMapTex_dir[i]);
            shader.setUniform("shadows[" + std::to_string(i) + "]", i);
            shader.setUniform("lSpaceMatrices[" + std::to_string(i) + "]", lSpaceMatrices[i]);
        }

        // shader.setUniform("lSpaceMatrix", getlightSpaceMatrix(light1_direction));
        // shader.setUniform("shadows", 0);

        for ( auto& model : models )
            model->render(*this);
    }

};

#endif //SCENE_H

