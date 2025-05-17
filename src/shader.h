#pragma once
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include "tglm.h"
#include <GLFW/glfw3.h>
#include "../corefw/corefw.h"       // IWYU pragma: keep

extern CFClassRef CFXShader;

typedef struct __CFXShader* CFXShaderRef;

/**
 *  class CFXShader
 */
typedef struct __CFXShader {
    __CFObject obj;
    GLuint Id;
} __CFXShader;

extern method void* Ctor(
    CFXShaderRef this, 
    CFStringRef vShader, 
    CFStringRef fShader);

extern method CFXShaderRef Use(
    CFXShaderRef this);

extern method GLuint GetId(
    CFXShaderRef this);

extern method void Compile(
    CFXShaderRef this, 
    const GLchar* vertexSource, 
    const GLchar* fragmentSource);
    
extern method void SetFloat(
    CFXShaderRef this,
    const GLchar* name,
    const GLfloat value,
    const GLboolean useShader);

extern method void SetInteger(
    CFXShaderRef this,
    const GLchar* name,
    GLint value,
    GLboolean useShader);

extern method void SetInteger(
    CFXShaderRef this,
    const GLchar* name,
    GLint value);

extern method void SetVector2(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLboolean useShader);

extern method void SetVector2(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y);

extern method void SetVector2v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec2* vector,
    GLboolean useShader);

extern method void SetVector2v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec2* vector);

extern method void SetVector3(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLboolean useShader);

extern method void SetVector3(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z);

extern method void SetVector3v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec3* vector,
    GLboolean useShader);

extern method void SetVector3v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec3* vector);

extern method void SetVector4(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat w,
    GLboolean useShader);

extern method void SetVector4(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat w);

extern method void SetVector4v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec4* vector,
    GLboolean useShader);

extern method void SetVector4v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec4* vector);

extern method void SetMatrix(
    CFXShaderRef this,
    const GLchar* name,
    const Mat* matrix,
    GLboolean useShader);

extern method void SetMatrix(
    CFXShaderRef this,
    const GLchar* name,
    const Mat* matrix);

static inline CFXShaderRef NewCFXShader(CFStringRef vShader, CFStringRef fShader)
{
    return Ctor((CFXShaderRef)CFCreate(CFXShader), vShader, fShader);
}
