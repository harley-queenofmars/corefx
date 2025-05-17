#pragma once
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "../corefw/corefw.h"   // IWYU pragma: keep
#include "texture2d.h"          // IWYU pragma: keep
#include "shader.h"
#include "rect.h"
#include "tglm.h"

extern CFClassRef CFXArrayRenderer;
typedef struct __CFXArrayRenderer* CFXArrayRendererRef;
/**
 *  class CFXArrayRenderer
 */
typedef struct __CFXArrayRenderer {
    __CFObject obj;
    CFXShaderRef shader;
    GLuint VBO;
    GLuint VAO;
} __CFXArrayRenderer;

extern method void* Ctor(
    CFXArrayRendererRef this, 
    CFXShaderRef shader);

extern method void Draw(
    CFXArrayRendererRef this, 
    CFXTexture2DRef texture, 
    CFXRect* bounds, 
    GLfloat rotate, 
    Vec3 color);

extern method void Draw(
    CFXArrayRendererRef this, 
    CFXTexture2DRef texture, 
    Vec2 position, 
    Vec2 size, 
    GLfloat rotate, 
    Vec3 color);

static inline CFXArrayRendererRef NewCFXArrayRenderer(CFXShaderRef shader)
{
    return Ctor((CFXArrayRendererRef)CFCreate(CFXArrayRenderer), shader);
}
