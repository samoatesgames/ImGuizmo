#include "imgui.h"
#include "imgui_internal.h"
#define IMAPP_IMPL

#include "ImApp.h"
#include "ImGuizmo.h"
#include "ImGuizmoHelper.h"

void EditTransform(const float *cameraView, float *cameraProjection, float* matrix, bool editTransformDecomposition)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

   if (editTransformDecomposition)
   {
	   if (ImGui::IsKeyPressed(90))
		   mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	   if (ImGui::IsKeyPressed(69))
		   mCurrentGizmoOperation = ImGuizmo::ROTATE;
	   if (ImGui::IsKeyPressed(82)) // r Key
		   mCurrentGizmoOperation = ImGuizmo::SCALE;
	   if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		   mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	   ImGui::SameLine();
	   if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		   mCurrentGizmoOperation = ImGuizmo::ROTATE;
	   ImGui::SameLine();
	   if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		   mCurrentGizmoOperation = ImGuizmo::SCALE;
	   float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	   ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	   ImGui::InputFloat3("Tr", matrixTranslation, 3);
	   ImGui::InputFloat3("Rt", matrixRotation, 3);
	   ImGui::InputFloat3("Sc", matrixScale, 3);
	   ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

	   if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	   {
		   if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			   mCurrentGizmoMode = ImGuizmo::LOCAL;
		   ImGui::SameLine();
		   if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			   mCurrentGizmoMode = ImGuizmo::WORLD;
	   }
	   if (ImGui::IsKeyPressed(83))
		   useSnap = !useSnap;
	   ImGui::Checkbox("", &useSnap);
	   ImGui::SameLine();

	   switch (mCurrentGizmoOperation)
	   {
	   case ImGuizmo::TRANSLATE:
		   ImGui::InputFloat3("Snap", &snap[0]);
		   break;
	   case ImGuizmo::ROTATE:
		   ImGui::InputFloat("Angle Snap", &snap[0]);
		   break;
	   case ImGuizmo::SCALE:
		   ImGui::InputFloat("Scale Snap", &snap[0]);
		   break;
	   }
	   ImGui::Checkbox("Bound Sizing", &boundSizing);
	   if (boundSizing)
	   {
		   ImGui::PushID(3);
		   ImGui::Checkbox("", &boundSizingSnap);
		   ImGui::SameLine();
		   ImGui::InputFloat3("Snap", boundsSnap);
		   ImGui::PopID();
	   }
   }
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing?bounds:NULL, boundSizingSnap?boundsSnap:NULL);
}
	
int main(int, char**)
{
	ImApp::ImApp imApp;

	ImApp::Config config;
	config.mWidth = 1280;
	config.mHeight = 720;
	imApp.Init(config);

   int lastUsing = 0;
	float objectMatrix[4][16] = {
	  { 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f },

      { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      2.f, 0.f, 0.f, 1.f },

      { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      2.f, 0.f, 2.f, 1.f },

      { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 2.f, 1.f }
   };

	static const float identityMatrix[16] =
	{ 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };

	float cameraView[16] = 
	  { 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };

	float cameraProjection[16];

	// Camera projection
	bool isPerspective = true;
	float fov = 27.f;
	float viewWidth = 10.f; // for orthographic
	float camYAngle = 165.f / 180.f * 3.14159f;
	float camXAngle = 32.f / 180.f * 3.14159f;
	float camDistance = 8.f;
   int gizmoCount = 1;

   bool firstFrame = true;
	
	// Main loop
	while (!imApp.Done())
	{
		imApp.NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		if (isPerspective)
		{
			Perspective(fov, io.DisplaySize.x / io.DisplaySize.y, 0.1f, 100.f, cameraProjection);
		}
		else
		{
			float viewHeight = viewWidth*io.DisplaySize.y / io.DisplaySize.x;
			OrthoGraphic(-viewWidth, viewWidth, -viewHeight, viewHeight, 1000.f, -1000.f, cameraProjection);
		}
		ImGuizmo::SetOrthographic(!isPerspective);
		ImGuizmo::BeginFrame();

      ImGui::SetNextWindowPos(ImVec2(1024, 100));
      ImGui::SetNextWindowSize(ImVec2(256, 256));

		// create a window and insert the inspector
		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(320, 340));
		ImGui::Begin("Editor");
		ImGui::Text("Camera");
      bool viewDirty = false;
		if (ImGui::RadioButton("Perspective", isPerspective)) isPerspective = true;
		ImGui::SameLine();
		if (ImGui::RadioButton("Orthographic", !isPerspective)) isPerspective = false;
		if (isPerspective)
		{
			ImGui::SliderFloat("Fov", &fov, 20.f, 110.f);
		}
		else
		{
			ImGui::SliderFloat("Ortho width", &viewWidth, 1, 20);
		}
      viewDirty |= ImGui::SliderFloat("Distance", &camDistance, 1.f, 10.f);
      ImGui::SliderInt("Gizmo count", &gizmoCount, 1, 4);

      if (viewDirty || firstFrame)
      {
         float eye[] = { cosf(camYAngle) * cosf(camXAngle) * camDistance, sinf(camXAngle) * camDistance, sinf(camYAngle) * cosf(camXAngle) * camDistance };
         float at[] = { 0.f, 0.f, 0.f };
         float up[] = { 0.f, 1.f, 0.f };
         LookAt(eye, at, up, cameraView);
         firstFrame = false;
      }

      ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
      ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
      ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
      ImGui::Separator();
      for (int matId = 0; matId < gizmoCount; matId++)
      {
         ImGuizmo::SetID(matId);
		   
		   EditTransform(cameraView, cameraProjection, objectMatrix[matId], lastUsing == matId);
         if (ImGuizmo::IsUsing())
         {
            lastUsing = matId;
         }
      }
      
      ImGui::End();

      ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), NULL);
      
		// render everything
		glClearColor(0.45f, 0.4f, 0.4f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();

		imApp.EndFrame();
	}

	imApp.Finish();

	return 0;
}
