/*******************************************************************
** This code is part of the Dark Overload Framework.
**
MIT License

Copyright (c) 2018 Dark Overlord of Data

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************/
/**
 * tglm is Tiny GLM for c99
 *
 * opengl math helpers inspired by GLM.
 * using clang vector extensions and function overload.
 * 
 */
 #pragma once
 #ifndef _TGLM_H
 #define _TGLM_H
 /**
  *  OpenCL Vector definitions 
  */
 typedef int IVec2 __attribute__((ext_vector_type(2)));
 typedef int IVec3 __attribute__((ext_vector_type(3)));
 typedef float Vec2 __attribute__((ext_vector_type(2)));
 typedef float Vec3 __attribute__((ext_vector_type(3)));
 typedef float Vec4 __attribute__((ext_vector_type(4)));
 typedef float Mat __attribute__((ext_vector_type(16)));
 
 /**
  * clamp number/vector between MIN and MAX values
  *
  * @param  val value
  * @param  minVal minimum 
  * @param  maxVal maximum 
  * @returns clamped value
  * 
  */
 float __attribute__((overloadable)) glm_clamp(float val, float minVal, float maxVal);
 Vec2 __attribute__((overloadable)) glm_clamp(Vec2 v, float minVal, float maxVal);
 Vec2 __attribute__((overloadable)) glm_clamp(Vec2 v, Vec2 minVal, Vec2 maxVal);
 Vec3 __attribute__((overloadable)) glm_clamp(Vec3 v, float minVal, float maxVal);
 Vec4 __attribute__((overloadable)) glm_clamp(Vec4 v, float minVal, float maxVal);
 
 /** 
  * translate transform matrix by v vector
  *
  * @param  m  affine transfrom
  * @param  v  translate vector [x, y, z]
  * @returns translated matrix
  * 
  */
 Mat glm_translate(Mat m, Vec3 v);
 
 /**
  * length - return scalar Euclidean length of a vector
  * 
  * @param v vector
  * @returns norm * norm
  * 
  */
 float __attribute__((overloadable)) glm_length(Vec2 v);
 float __attribute__((overloadable)) glm_length(Vec3 v);
 float __attribute__((overloadable)) glm_length(Vec4 v);
 
 /**
  * vector dot product
  *
  * @param a vector1
  * @param b vector2
  * @returns dot product
  * 
  */
 float __attribute__((overloadable)) glm_dot(Vec2 a, Vec2 b);
 float __attribute__((overloadable)) glm_dot(Vec3 a, Vec3 b);
 float __attribute__((overloadable)) glm_dot(Vec4 a, Vec4 b);
 
 /**
  * norm * norm (magnitude) of vector
  *
  * @param v vector
  * @returns norm * norm
  * 
  */
 float __attribute__((overloadable)) glm_norm2(Vec2 v);
 float __attribute__((overloadable)) glm_norm2(Vec3 v);
 float __attribute__((overloadable)) glm_norm2(Vec4 v);
 
 /**
  * norm (magnitude) of vec3
  *
  * @param vec vector
  * @returns norm
  */
 float __attribute__((overloadable)) glm_norm(Vec2 vec);
 float __attribute__((overloadable)) glm_norm(Vec3 vec);
 float __attribute__((overloadable)) glm_norm(Vec4 vec);
 
 /**
  * return an identity matrix
  */
 Mat mat_identity();
 
 /**
  * normalize vector and store result in same vec
  *
  * @param v vector
  * @returns normalized vector
  * 
  */
 Vec3 __attribute__((overloadable)) glm_normalize(Vec3 v);
 Vec2 __attribute__((overloadable)) glm_normalize(Vec2 v);
 
 /**
  * scales existing transform matrix by v vector
  * and stores result in same matrix
  *
  * @param  m  affine transfrom
  * @param  v  scale vector [x, y, z]
  * @returns scaled Vec3
  */
 Mat glm_scale(Mat m, Vec3 v);
 
 /**
  * set up orthographic projection matrix
  *
  * @param  left    left and right vertical clipping planes
  * @param  right   left and right vertical clipping planes
  * @param  bottom  bottom and top horizontal clipping planes
  * @param  top     bottom and top horizontal clipping planes
  * @param  nearVal nearer and farther depth clipping planes
  * @param  farVal  nearer and farther depth clipping planes
  * @returns projection matrix
  */
 Mat glm_ortho(float left, float right, float bottom, float top, float nearVal, float farVal);
 
 /**
  * Mulitiply a * b
  * The matrix is defined as 16 rather than 4*4, so the naive operator 
  * yield multiplies 2 vec16, not 2 mat4x4
  */
 Mat glm_mat_mul(Mat a, Mat b);
 /**
  * rotate existing transform matrix around given axis by angle
  *
  * @param  m      affine transfrom
  * @param  angle  angle (radians)
  * @param  axis   axis
  */
 Mat glm_rotate(Mat m, float angle, Vec3 v);
 
 Mat glm_rotateZ(Mat m, float angle);
 Mat glm_rotateY(Mat m, float angle);
 
 #endif // _TGLM_H