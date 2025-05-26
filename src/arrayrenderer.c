#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <corefw.h>   // IWYU pragma: keep
#include "corefx.h"             // IWYU pragma: keep
#include <GLFW/glfw3.h>
#include "arrayrenderer.h"

class2(CFXArrayRenderer);

/**
 * @brief Constructor for the CFXArrayRenderer object.
 *
 * Initializes the array renderer by assigning the shader, setting the destructor,
 * and configuring the vertex data and OpenGL buffers required for rendering.
 *
 * Steps performed:
 * - Assigns the provided shader to the renderer instance.
 * - Sets the destructor function pointer.
 * - Defines vertex data for a quad with position and texture coordinates.
 * - Generates and binds OpenGL Vertex Array Object (VAO) and Vertex Buffer Object (VBO).
 * - Uploads vertex data to the GPU.
 * - Configures vertex attribute pointers for shader input.
 * - Unbinds the buffer and VAO to prevent accidental modification.
 *
 * @param this   Pointer to the CFXArrayRenderer instance to initialize.
 * @param shader Reference to the shader to be used by the renderer.
 * @return       Pointer to the initialized CFXArrayRenderer instance.
 */
 proc void* Ctor(CFXArrayRendererRef this, CFXShaderRef shader)
{
    this->shader = shader;
    CFXArrayRenderer->dtor = &dtor;
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return this;
}

/**
 * @brief Destructor for the CFXArrayRenderer object.
 *
 * Releases OpenGL resources associated with the renderer by deleting
 * the Vertex Array Object (VAO) and Vertex Buffer Object (VBO).
 *
 * @param self Pointer to the CFXArrayRenderer instance to be destroyed.
 */
static void dtor(void* self)
{
    CFXArrayRendererRef this = self;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

/**
 * @brief Draws a textured quad with specified transformations and color.
 *
 * This function prepares the transformation matrix for rendering a quad,
 * applies translation, rotation (around the center), and scaling based on the
 * provided bounds and rotation angle. It then sets the transformation matrix
 * and color uniform in the shader, binds the texture, and renders the quad.
 *
 * @param this      Pointer to the array renderer instance.
 * @param texture   Reference to the texture to be rendered.
 * @param bounds    Pointer to a CFXRect structure specifying the position (x, y)
 *                  and size (w, h) of the quad.
 * @param rotate    Rotation angle in radians to apply to the quad.
 * @param color     RGB color vector to modulate the sprite.
 */
proc void Draw(
    CFXArrayRendererRef this,
    CFXTexture2DRef texture,
    CFXRect* bounds,
    GLfloat rotate,
    Vec3 color)
{
    // Prepare transformations
    Use(this->shader);
    Mat model = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    Vec3 size = { bounds->w, bounds->h, 1 };
    Vec3 position = { bounds->x, bounds->y, 0 };

    model = glm_translate(model, (Vec3) { position.x, position.y, 0.0f }); // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    model = glm_translate(model, (Vec3) { 0.5f * size.x, 0.5f * size.y, 0.0f }); // Move origin of rotation to center of quad
    model = glm_rotate(model, rotate, (Vec3) { 0.0f, 0.0f, 1.0f }); // Then rotate
    model = glm_translate(model, (Vec3) { -0.5f * size.x, -0.5f * size.y, 0.0f }); // Move origin back
    model = glm_scale(model, (Vec3) { size.x, size.y, 1.0f }); // Last scale

    SetMatrix(this->shader, "model", &model, true);

    // Render textured quad
    SetVector3v(this->shader, "spriteColor", &color, true);

    glActiveTexture(GL_TEXTURE0);
    Bind(texture);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


/**
 * @brief Draws a textured quad at the specified position, size, rotation, and color.
 *
 * This function prepares the transformation matrix for the quad, sets shader uniforms,
 * binds the appropriate texture, and renders the quad using OpenGL.
 *
 * @param this      Pointer to the array renderer instance.
 * @param texture   Reference to the 2D texture to be drawn.
 * @param position  The position (x, y) where the quad will be rendered.
 * @param size      The size (width, height) of the quad.
 * @param rotate    The rotation angle (in radians) to apply to the quad.
 * @param color     The color (RGB) to tint the quad.
 */
proc void Draw(
    CFXArrayRendererRef this,
    CFXTexture2DRef texture,
    Vec2 position, 
    Vec2 size, 
    GLfloat rotate,
    Vec3 color)
{
    // Prepare transformations
    Use(this->shader);
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

    SetMatrix(this->shader, "model", &model, true);

    // Render textured quad
    SetVector3v(this->shader, "spriteColor", &color, true);

    glActiveTexture(GL_TEXTURE0);
    // CFXTexture2D_Bind(texture);
    Bind(texture);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
