#include "imgui.h"
#include "imgui_internal.h"
#define IMAPP_IMPL

#include "ImApp.h"
#include "ImGuizmo.h"
#include "ImGuizmoHelper.h"

#include "example/ExampleCamera.h"

void EditCamera(ExampleCamera& camera)
{
   ImGui::Begin("Camera");

   if (ImGui::RadioButton("Perspective", camera.IsPerspective())) camera.SetPerspectiveCamera(true);
   ImGui::SameLine();
   if (ImGui::RadioButton("Orthographic", !camera.IsPerspective())) camera.SetPerspectiveCamera(false);

   if (camera.IsPerspective())
   {
      ImGui::SliderFloat("Fov", camera.GetFieldOfView(), 20.f, 110.f);
   }
   else
   {
      ImGui::SliderFloat("Ortho width", camera.GetOrthographicViewWidth(), 1, 20);
   }

   ImGui::End();
}

void EditTransform(
   ExampleCamera &camera,
   float* matrix,
   ImGuizmo::OPERATION& currentGizmoOperation,
   ImGuizmo::MODE& currentGizmoMode)
{
   ImGui::Begin("Transform");

   if (ImGui::IsKeyPressed(90))
      currentGizmoOperation = ImGuizmo::TRANSLATE;
   if (ImGui::IsKeyPressed(69))
      currentGizmoOperation = ImGuizmo::ROTATE;
   if (ImGui::IsKeyPressed(82)) // r Key
      currentGizmoOperation = ImGuizmo::SCALE;
   if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
      currentGizmoOperation = ImGuizmo::TRANSLATE;
   ImGui::SameLine();
   if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
      currentGizmoOperation = ImGuizmo::ROTATE;
   ImGui::SameLine();
   if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
      currentGizmoOperation = ImGuizmo::SCALE;
   float matrixTranslation[3], matrixRotation[3], matrixScale[3];
   ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
   ImGui::InputFloat3("Tr", matrixTranslation, 3);
   ImGui::InputFloat3("Rt", matrixRotation, 3);
   ImGui::InputFloat3("Sc", matrixScale, 3);
   ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

   if (currentGizmoOperation != ImGuizmo::SCALE)
   {
      if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
         currentGizmoMode = ImGuizmo::LOCAL;
      ImGui::SameLine();
      if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
         currentGizmoMode = ImGuizmo::WORLD;
   }

   if (ImGui::Button("Look at Cube"))
   {
      camera.SetLookAt(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
   }

   ImGui::End();
}

int main(int, char**)
{
   ImApp::ImApp imApp;

   ImApp::Config config;
   config.mWidth = 1280;
   config.mHeight = 720;
   imApp.Init(config);

   float objectMatrix[16] = {
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f
   };

   static const float identityMatrix[16] = {
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f
   };

   ImGuizmo::OPERATION currentGizmoOperation(ImGuizmo::TRANSLATE);
   ImGuizmo::MODE currentGizmoMode(ImGuizmo::LOCAL);

   ExampleCamera camera;
   camera.SetPerspectiveCamera(true);
   camera.SetPosition(0, 5, -10);

   bool viewDirty = true;

   // Main loop
   while (!imApp.Done())
   {
      imApp.NewFrame();

      ImGuiIO& io = ImGui::GetIO();

      EditCamera(camera);
      EditTransform(camera, objectMatrix, currentGizmoOperation, currentGizmoMode);

      ImGuizmo::BeginFrame();
      ImGuizmo::SetOrthographic(!camera.IsPerspective());
      ImGuizmo::DrawHatchedNegativeAxis(false);
      
      float* cameraView = camera.GetViewMatrix();
      float* cameraProjection = camera.GetProjectionMatrix(io.DisplaySize.x, io.DisplaySize.y);

      ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
      ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
      ImGuizmo::DrawCube(cameraView, cameraProjection, objectMatrix);
      ImGuizmo::Manipulate(cameraView, cameraProjection, currentGizmoOperation, currentGizmoMode, objectMatrix, NULL, NULL, NULL, NULL);
      ImGuizmo::ViewManipulate(camera.GetViewMatrix(), camera.DistanceToLookAt(), ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), NULL);

      glClearColor(0.45f, 0.4f, 0.4f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui::Render();

      imApp.EndFrame();
   }

   imApp.Finish();

   return 0;
}
