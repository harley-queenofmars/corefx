#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "../corefw/corefw.h"   // IWYU pragma: keep
#include "corefx.h"             // IWYU pragma: keep
#include "elementrenderer.h"

class2(CFXElementRenderer, dtor);

/**
 * ElementRenderer
 * 
 * @param shader to use for rendering
 * 
 */
 method void* Ctor(CFXElementRendererRef this, CFXShaderRef shader)
{
    CFXElementRenderer->dtor = dtor;
    this->shader = shader;
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    return this;
}

/**
 * destructor
 */
static void dtor(void* self)
{
    CFXElementRendererRef this = self;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

/**
 * Draw
 * 
 * @param texture the image to render
 * @param position to render at
 * @param size to render
 * @param rotate amount to rotate by
 * @param color to tint
 * 
 */
method void Draw(
    CFXElementRendererRef this,
    CFXTexture2DRef texture,
    CFXRect bounds,
    GLfloat rotate,
    Vec3 color)
{
    // Prepare transformations

    Vec3 size = { bounds.w, bounds.h, 1 };
    Vec3 position = { bounds.x, bounds.y, 0 };
    Mat model = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    model = glm_translate(model, (Vec3) { position.x, position.y, 0.0f }); // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    model = glm_translate(model, (Vec3) { 0.5f * size.x, 0.5f * size.y, 0.0f }); // Move origin of rotation to center of quad
    model = glm_rotate(model, rotate, (Vec3) { 0.0f, 0.0f, 1.0f }); // Then rotate
    model = glm_translate(model, (Vec3) { -0.5f * size.x, -0.5f * size.y, 0.0f }); // Move origin back
    model = glm_scale(model, (Vec3) { size.x, size.y, 1.0f }); // Last scale

    Use(this->shader);
    SetMatrix(this->shader, "model", &model); //, true);
    SetVector3v(this->shader, "spriteColor", &color, true);
    glActiveTexture(GL_TEXTURE0);
    Bind(texture);
    // CFXTexture2D_Bind(texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

method void Draw(
    CFXElementRendererRef this,
    CFXTexture2DRef texture,
    Vec2 position, 
    Vec2 size, 
    GLfloat rotate,
    Vec3 color)
{
    // Prepare transformations

    Mat model = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    model = glm_translate(model, (Vec3) { position.x, position.y, 0.0f }); // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    model = glm_translate(model, (Vec3) { 0.5f * size.x, 0.5f * size.y, 0.0f }); // Move origin of rotation to center of quad
    model = glm_rotate(model, rotate, (Vec3) { 0.0f, 0.0f, 1.0f }); // Then rotate
    model = glm_translate(model, (Vec3) { -0.5f * size.x, -0.5f * size.y, 0.0f }); // Move origin back
    model = glm_scale(model, (Vec3) { size.x, size.y, 1.0f }); // Last scale

    Use(this->shader);
    SetMatrix(this->shader, "model", &model); //, true);
    SetVector3v(this->shader, "spriteColor", &color, true);
    glActiveTexture(GL_TEXTURE0);
    Bind(texture);
    // CFXTexture2D_Bind(texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

