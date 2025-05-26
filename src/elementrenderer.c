#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <corefw.h>   // IWYU pragma: keep
#include "corefx.h"             // IWYU pragma: keep
#include "elementrenderer.h"

class2(CFXElementRenderer);

/**
 * @brief Constructor for the CFXElementRenderer object.
 *
 * Initializes the CFXElementRenderer instance with the provided shader, sets up
 * the destructor, and configures the necessary OpenGL buffers and vertex attributes
 * for rendering a textured quad.
 *
 * The function performs the following steps:
 * - Assigns the destructor function pointer.
 * - Stores the shader reference.
 * - Defines vertex and index data for a quad with position and texture coordinates.
 * - Generates and binds OpenGL Vertex Array Object (VAO), Vertex Buffer Object (VBO),
 *   and Element Buffer Object (EBO).
 * - Uploads vertex and index data to the GPU.
 * - Configures vertex attribute pointers for position and texture coordinates.
 *
 * @param this   Pointer to the CFXElementRenderer instance to initialize.
 * @param shader Reference to the shader to be used by the renderer.
 * @return       Pointer to the initialized CFXElementRenderer instance.
 */
proc void* Ctor(CFXElementRendererRef this, CFXShaderRef shader)
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
 * @brief Destructor for the CFXElementRenderer object.
 *
 * This function releases the OpenGL resources associated with the given
 * CFXElementRenderer instance by deleting its Vertex Array Object (VAO),
 * Vertex Buffer Object (VBO), and Element Buffer Object (EBO).
 *
 * @param self Pointer to the CFXElementRenderer instance to be destroyed.
 */
static void dtor(void* self)
{
    CFXElementRendererRef this = self;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

/**
 * @brief Draws a textured quad with specified transformations and color.
 *
 * This function prepares the transformation matrix for rendering a 2D element (quad)
 * by applying translation, rotation (around the center), and scaling, in that order.
 * It then sets up the shader uniforms, binds the texture, and issues a draw call.
 *
 * @param this      Reference to the element renderer.
 * @param texture   Reference to the 2D texture to be drawn.
 * @param bounds    Rectangle specifying the position (x, y) and size (w, h) of the quad.
 * @param rotate    Rotation angle in radians, applied around the center of the quad.
 * @param color     RGB color vector to tint the sprite.
 */
proc void Draw(
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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


/**
 * @brief Draws a textured quad (sprite) with specified transformations and color.
 *
 * This function prepares the transformation matrix for a 2D element, applying translation,
 * rotation (around the center), and scaling in the correct order. It then sets up the shader,
 * passes the transformation matrix and color, binds the texture, and issues a draw call.
 *
 * @param this      Reference to the element renderer.
 * @param texture   Reference to the texture to be drawn.
 * @param position  Position (x, y) where the quad will be drawn.
 * @param size      Size (width, height) of the quad.
 * @param rotate    Rotation angle (in radians) to apply to the quad.
 * @param color     Color (RGB) to tint the sprite.
 */
proc void Draw(
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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

