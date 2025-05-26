#pragma once
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <corefw.h>       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include "rect.h"
#include "texture2d.h"
#include "tglm.h"

extern CFClassRef CFXElementRenderer;
typedef struct __CFXElementRenderer* CFXElementRendererRef;

/**
 * @struct __CFXElementRenderer
 * @brief Represents an element renderer in the CoreFX rendering system.
 *
 * This structure encapsulates the necessary OpenGL objects and shader reference
 * required to render graphical elements. It includes a base object, a shader reference,
 * and OpenGL buffer object identifiers.
 *
 * Members:
 * - obj:        Base object for CoreFX objects, providing common functionality.
 * - shader:     Reference to the shader used for rendering elements.
 * - VBO:        OpenGL Vertex Buffer Object identifier.
 * - VAO:        OpenGL Vertex Array Object identifier.
 * - EBO:        OpenGL Element Buffer Object identifier.
 */
typedef struct __CFXElementRenderer {
    __CFObject obj;
    CFXShaderRef shader;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
} __CFXElementRenderer;

extern proc void* Ctor(
    CFXElementRendererRef this, 
    CFXShaderRef shader);

extern proc void Draw(
    CFXElementRendererRef this, 
    CFXTexture2DRef texture, 
    CFXRect bounds, 
    GLfloat rotate, 
    Vec3 color);

extern proc void Draw(
    CFXElementRendererRef this, 
    CFXTexture2DRef texture, 
    Vec2 position, 
    Vec2 size, 
    GLfloat rotate, 
    Vec3 color);

/**
 * @brief Creates a new CFXElementRenderer instance using the specified shader.
 *
 * This function allocates and initializes a new CFXElementRenderer object,
 * associating it with the provided shader reference.
 *
 * @param shader A reference to the CFXShader to be used by the renderer.
 * @return A reference to the newly created CFXElementRenderer.
 */
static inline CFXElementRendererRef NewCFXElementRenderer(CFXShaderRef shader)
{
    return Ctor((CFXElementRendererRef)CFCreate(CFXElementRenderer), shader);
}

