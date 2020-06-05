
#include "ExampleCamera.h"
#include "ImGuizmoHelper.h"

#include <algorithm>

ExampleCamera::ExampleCamera()
{
   float identityMatrix[16] =
   {
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f
   };
   std::copy(std::begin(identityMatrix), std::end(identityMatrix), std::begin(m_viewMatrix));
   std::copy(std::begin(identityMatrix), std::end(identityMatrix), std::begin(m_projectionMatrix));
}

float ExampleCamera::DistanceToLookAt()
{
   float distance[3]
   {
      m_position[0] - m_lookAt[0],
      m_position[1] - m_lookAt[1],
      m_position[2] - m_lookAt[2],
   };

   return sqrt((distance[0] * distance[0]) + (distance[1] * distance[1]) + (distance[2] * distance[2]));
}

void ExampleCamera::SetPosition(float x, float y, float z)
{
   m_position[0] = x;
   m_position[1] = y;
   m_position[2] = z;
   LookAt(m_position, m_lookAt, m_worldUp, m_viewMatrix);
}

void ExampleCamera::SetLookAt(float x, float y, float z)
{
   m_lookAt[0] = x;
   m_lookAt[1] = y;
   m_lookAt[2] = z;
   LookAt(m_position, m_lookAt, m_worldUp, m_viewMatrix);
}

float* ExampleCamera::GetViewMatrix()
{
   return m_viewMatrix;
}

float* ExampleCamera::GetProjectionMatrix(float displayWidth, float displayHeight)
{
   if (m_isPerspective)
   {
      Perspective(m_fieldOfView, displayWidth / displayHeight, 0.1f, 100.f, m_projectionMatrix);
   }
   else
   {
      float viewHeight = m_orthographicViewWidth * displayHeight / displayWidth;
      OrthoGraphic(-m_orthographicViewWidth, m_orthographicViewWidth, -viewHeight, viewHeight, 1000.f, -1000.f, m_projectionMatrix);
   }

   return m_projectionMatrix;
}
