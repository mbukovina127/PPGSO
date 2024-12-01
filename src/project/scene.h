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
#include <shaders/cubemap_vert_glsl.h>
#include <shaders/cubemap_frag_glsl.h>
#include <shaders/cubemap_geo_glsl.h>

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
    float far_plane;
    float constant;
    float linear;
    float quadratic;
};

class Scene {
public:
    ppgso::Shader shader = ppgso::Shader(light_vert_glsl, light_frag_glsl, "");
    ppgso::Shader depthshader = ppgso::Shader(depth_vert_glsl, depth_frag_glsl, "");
    ppgso::Shader cubemap_shader = ppgso::Shader(cubemap_vert_glsl, cubemap_frag_glsl, cubemap_geo_glsl);
    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<Model>> models;
    //lights
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
    //shadow maps
    std::vector<GLuint> depthMapFBOs_dir;
    GLuint depthMapTex_dir_ARRAY;
    std::vector<glm::mat4> lSpaceMatrices;

    std::vector<GLuint> depthCubemapFBOs_ARRAY;
    GLuint depthCubemap_ARRAY;
    std::vector<glm::mat4> shadowTransforms;


    void setUpLights() {
        directionalLights = {
            {
                {{1,1,1}, .2f, 1.f}, {-2, 3, -4}
            },
            {
                {{1,0.4,0.4}, .0f, .8f}, {4, 5, -4}
            }
        };
        pointLights = {
            {
                {{1,1,1}, 0, .6f}, {0,2,-0.1}, {25.0}, {0.0}, {0.5}, {0.032}
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

        for (int i = 0; i < pointLights.size(); i++) {
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].base.color", pointLights[i].base.color);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].base.ambI", pointLights[i].base.ambI);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].base.difI", pointLights[i].base.difI);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].position", pointLights[i].position);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].far_plane", pointLights[i].far_plane);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].constant", pointLights[i].constant);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].linear", pointLights[i].linear);
            shader.setUniform("PLIGHTS[" + std::to_string(i) + "].quadratic", pointLights[i].quadratic);
        }
        shader.setUniform("numPointL", (GLint)pointLights.size());
        cout << pointLights.size() << '\n';
        // int numofpointlights = 1;
        //
        // // p light 1
        // shader.setUniform("PLIGHTS[0].constant", 0.0f);
        // shader.setUniform("PLIGHTS[0].linear", 0.15f);
        // shader.setUniform("PLIGHTS[0].quadratic", 0.032f);
        //
        //
        //
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

    void setUpDepthMap() {
        // Directional Lights
        // Generate a single GL_TEXTURE_2D_ARRAY for shadow maps
        glGenTextures(1, &depthMapTex_dir_ARRAY);
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapTex_dir_ARRAY);

        // Allocate storage for all layers (one for each directional light)
        unsigned int numLayers = directionalLights.size();
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, numLayers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Generate and configure framebuffers for each layer
        depthMapFBOs_dir.resize(numLayers);
        glGenFramebuffers(numLayers, depthMapFBOs_dir.data());

        for (size_t i = 0; i < numLayers; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs_dir[i]);
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapTex_dir_ARRAY, 0, i);

            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer " << i << " is not complete!" << std::endl;
            }
        }
        // Point Lights
        // create depth cubemap texture
        numLayers = pointLights.size() * 6;
        glGenTextures(1, &depthCubemap_ARRAY);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubemap_ARRAY);
        glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, numLayers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


        depthCubemapFBOs_ARRAY.resize(pointLights.size());
        glGenFramebuffers(pointLights.size(), depthCubemapFBOs_ARRAY.data());

        for (size_t i = 0; i < pointLights.size(); ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBOs_ARRAY[i]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap_ARRAY, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer for point light " << i << " is not complete!" << std::endl;
            }
        }

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

    // Needs to be called only once
    auto computeDepthCubemapMatrix(glm::vec3 lightPos, float far_plane = 25.0f) {
        float near_plane = .1f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, near_plane, far_plane);
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    }

    auto getlightSpaceMatrix(glm::vec3 light_position) {
        float near_plane = 0.1f, far_plane = 20.0f;
        glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(glm::normalize(light_position) * 5.f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 lSpaceMatrix = lightProjection * lightView;
        depthshader.setUniform("lSpaceMatrix", lSpaceMatrix);
        return lSpaceMatrix;

    }
    void cubemapPass() {
        cubemap_shader.use();
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        for (int u = 0; u < pointLights.size(); ++u) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBOs_ARRAY[u]);
            glClear(GL_DEPTH_BUFFER_BIT);
            cubemap_shader.setUniform("lightPos", pointLights[u].position);
            cubemap_shader.setUniform("far_plane", pointLights[u].far_plane);
            cubemap_shader.setUniform("layer", u);
            for (int i = 0; i < 6; i++) {
                // TODO: can add compute of matrices here
                cubemap_shader.setUniform("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[u * 6 + i]);
            }
            for (auto& model : models) {
                model->renderDepth(cubemap_shader);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void shadowPass() {
        depthshader.use();

        computeLightSpaceMatrices();

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        for (int i = 0; i < depthMapFBOs_dir.size(); ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs_dir[i]);
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
        shader.setUniform("dirShadows", 0);
        for (int i = 0; i < directionalLights.size(); i++)
            shader.setUniform("lSpaceMatrices[" + std::to_string(i) + "]", lSpaceMatrices[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapTex_dir_ARRAY);
        shader.setUniform("pointShadows", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubemap_ARRAY);

        for ( auto& model : models )
            model->render(*this);
    }

};

#endif //SCENE_H

