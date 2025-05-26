#pragma once
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <corefw.h>   // IWYU pragma: keep
#include "texture2d.h"          // IWYU pragma: keep
#include "shader.h"
#include "rect.h"
#include "tglm.h"

extern CFClassRef CFXArrayRenderer;
typedef struct __CFXArrayRenderer* CFXArrayRendererRef;

/**
 * @struct __CFXArrayRenderer
 * @brief Represents a renderer for array-based drawing in the CFX framework.
 *
 * This structure encapsulates the necessary resources and references required
 * to render objects using array buffers in OpenGL. It includes a base object,
 * a reference to the shader used for rendering, and OpenGL buffer objects.
 *
 * Members:
 * - obj:        Base object information for the renderer.
 * - shader:     Reference to the shader program used for rendering.
 * - VBO:        OpenGL Vertex Buffer Object identifier.
 * - VAO:        OpenGL Vertex Array Object identifier.
 */
typedef struct __CFXArrayRenderer {
    __CFObject obj;
    CFXShaderRef shader;
    GLuint VBO;
    GLuint VAO;
} __CFXArrayRenderer;

extern proc void* Ctor(
    CFXArrayRendererRef this, 
    CFXShaderRef shader);

extern proc void Draw(
    CFXArrayRendererRef this, 
    CFXTexture2DRef texture, 
    CFXRect* bounds, 
    GLfloat rotate, 
    Vec3 color);

extern proc void Draw(
    CFXArrayRendererRef this, 
    CFXTexture2DRef texture, 
    Vec2 position, 
    Vec2 size, 
    GLfloat rotate, 
    Vec3 color);

/**
 * @brief Creates a new CFXArrayRenderer instance with the specified shader.
 *
 * This function allocates and initializes a new CFXArrayRenderer object,
 * associating it with the provided shader reference.
 *
 * @param shader The shader to be used by the new array renderer.
 * @return A reference to the newly created CFXArrayRenderer.
 */
static inline CFXArrayRendererRef NewCFXArrayRenderer(CFXShaderRef shader)
{
    return Ctor((CFXArrayRendererRef)CFCreate(CFXArrayRenderer), shader);
}
