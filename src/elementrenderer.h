#pragma once
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "../corefw/corefw.h"       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include "rect.h"
#include "texture2d.h"
#include "tglm.h"

extern CFClassRef CFXElementRenderer;
typedef struct __CFXElementRenderer* CFXElementRendererRef;
/**
 *  class CFXElementRenderer
 */
typedef struct __CFXElementRenderer {
    __CFObject obj;
    CFXShaderRef shader;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
} __CFXElementRenderer;

extern method void* Ctor(
    CFXElementRendererRef this, 
    CFXShaderRef shader);

extern method void Draw(
    CFXElementRendererRef this, 
    CFXTexture2DRef texture, 
    CFXRect bounds, 
    GLfloat rotate, 
    Vec3 color);

extern method void Draw(
    CFXElementRendererRef this, 
    CFXTexture2DRef texture, 
    Vec2 position, 
    Vec2 size, 
    GLfloat rotate, 
    Vec3 color);

static inline CFXElementRendererRef NewCFXElementRenderer(CFXShaderRef shader)
{
    return Ctor((CFXElementRendererRef)CFCreate(CFXElementRenderer), shader);
}

