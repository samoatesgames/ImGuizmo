#pragma once

#include "imgui.h"

class ExampleCamera {

private:

   float m_viewMatrix[16];
   float m_projectionMatrix[16];

   bool m_isPerspective{ false };
   float m_fieldOfView{ 60.0f };
   float m_orthographicViewWidth{ 10.0f };

   float m_position[3]{ 0.0f, 0.0f, -1.0f };
   float m_lookAt[3]{ 0.0f, 0.0f, 0.0f };
   float m_worldUp[3]{ 0.0f, 1.0f, 0.0f };

public:
   ExampleCamera();

   void SetPerspectiveCamera(bool isPerspectiveCamera) { m_isPerspective = isPerspectiveCamera; }
   bool IsPerspective() { return m_isPerspective; }

   void SetFieldOfView(float fov) { m_fieldOfView = fov; }
   float* GetFieldOfView() { return &m_fieldOfView; }

   void SetOrthographicViewWidth(float viewWidth) { m_orthographicViewWidth = viewWidth; }
   float* GetOrthographicViewWidth() { return &m_orthographicViewWidth; }

   float DistanceToLookAt();

   void SetPosition(float x, float y, float z);
   void SetLookAt(float x, float y, float z);

public:

   float* GetViewMatrix();
   float* GetProjectionMatrix(float displayWidth, float displayHeigh);

};
