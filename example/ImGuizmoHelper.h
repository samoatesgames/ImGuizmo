#pragma once

#include "imgui.h"
#include <math.h>

//
//
// ImGuizmo example helper functions
//
//
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

void Frustum(float left, float right, float bottom, float top, float znear, float zfar, float *m16);
void Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float *m16);
void Cross(const float* a, const float* b, float* r);
float Dot(const float* a, const float* b);
void Normalize(const float* a, float *r);
void LookAt(const float* eye, const float* at, const float* up, float *m16);
void OrthoGraphic(const float l, float r, float b, const float t, float zn, const float zf, float *m16);
