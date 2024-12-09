#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include <shader.h>
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

class Snowflake {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 velocity;
    ppgso::Shader shader;

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<glm::mat4> instanceTransforms;

    unsigned int VAO, VBO, EBO, instanceVBO;

    Snowflake(const glm::vec3& startPos, const glm::vec3& vel, const glm::vec3& scale)
        : position(startPos), velocity(vel), scale(scale), shader(color_vert_glsl, color_frag_glsl, ""), VAO(0), VBO(0), EBO(0), instanceVBO(0) {
        defineCube();
        generateIndices();
        setupBuffers();

        for (int i = 0; i < 1000; i++) {
            float x = -2.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (2.0f - (-2.0f))));
            float y = -3.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (3.0f - (-3.0f))));
            float z = -1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (1.0f - (-1.0f))));

            glm::vec3 position(x, y, z);

            // Create a transformation matrix with translation
            glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);

            // Add the matrix to the vector
            instanceTransforms.push_back(matrix);
        }
    }

    void defineCube() {
        vertices = {
            // Front face
            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},

            // Back face
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},

            // Left face
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

            // Right face
            {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

            // Top face
            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

            // Bottom face
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
        };
    }

    void generateIndices() {
        indices = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Back face
            4, 5, 6, 6, 7, 4,
            // Left face
            8, 9, 10, 10, 11, 8,
            // Right face
            12, 13, 14, 14, 15, 12,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 21, 22, 22, 23, 20
        };
    }


    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);

        // Instance buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // Dynamic size
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
            glEnableVertexAttribArray(3 + i);
            glVertexAttribDivisor(3 + i, 1); // One mat4 per instance
        }
        glBindVertexArray(0);
    }

    void render(glm::mat4 proj, glm::mat4 view) const {
        shader.use();
        shader.setUniform("ProjectionMatrix", proj);
        shader.setUniform("ViewMatrix", view);
        shader.setUniform("model", glm::translate(glm::scale(glm::mat4(1.f), scale), position));
        shader.setUniform("OverallColor", glm::vec3(1.0f, 1.0f, 1.0f));


        // Update instance buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceTransforms.size() * sizeof(glm::mat4), &instanceTransforms[0], GL_DYNAMIC_DRAW);

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceTransforms.size());
        glBindVertexArray(0);
    }

    void update(float deltaTime) {
        position.y -= 2*deltaTime;
        if (position.y <= 0.0f) {
            position.y = 20.5f;
        }
    }

    ~Snowflake() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &instanceVBO);
    }
};

#endif //SNOWFLAKE_H
