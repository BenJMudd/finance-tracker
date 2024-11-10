#include "App.h"
#include "imgui/imgui.h"
#include "imgui/implot.h"
#include <stdio.h>
#include <time.h>

App::App()
    : m_demoWindowOpen(false), m_plotDemoOpen(false),
      m_mainViewFilters(m_controller.GetDB().GetCategoryNames().size(), true) {
  ImGui::GetIO().WantCaptureKeyboard = true;
}

void App::Render() {
  RenderTitleBar();
  if (m_demoWindowOpen) {
    ImGui::ShowDemoWindow(&m_demoWindowOpen);
  }

  if (m_plotDemoOpen) {
    ImPlot::ShowDemoWindow(&m_plotDemoOpen);
  }

  RenderMainView();
}

void App::RenderTitleBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Render demo window")) {
        m_demoWindowOpen = true;
      }
      if (ImGui::MenuItem("Render plot demo window")) {
        m_plotDemoOpen = true;
      }
      DBFilter::SPtr &mainFilter =
          m_controller.GetViewController().GetMainFilter();
      ViewRendering::RenderSetDate(*mainFilter);
      ViewRendering::RenderSetCateogries(*mainFilter);
      if (ImGui::MenuItem("Refresh all views")) {
        m_controller.GetViewController().RefreshViews();
      }
      if (ImGui::MenuItem("Input transactions")) {
        InputData();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void App::RenderMainView() {
  static ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  if (ImGui::Begin("MainFrame", nullptr, flags)) {

    // Left
    {
      static int selected = 0;
      ImGui::BeginChild("left pane", ImVec2(150, 0),
                        ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
      RenderViewWnd(m_leftViewHandle);
      ImGui::EndChild();
    }
    ImGui::SameLine();
    {
      static int selected = 0;

      ImGui::BeginGroup();
      ImGui::BeginChild("Right pane");
      ImGui::BeginChild("right pane up", ImVec2(0, 0),
                        ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY);

      RenderViewWnd(m_topRightViewHandle);
      ImGui::EndChild();

      ImGui::BeginChild("right pane below", ImVec2(0, 0),
                        ImGuiChildFlags_Borders);
      RenderViewWnd(m_bottomRightViewHandle);
      ImGui::EndChild();
      ImGui::EndChild();
      ImGui::EndGroup();
    }
  }
  ImGui::End();
}

void App::RenderViewWnd(std::optional<uint8_t> &viewHandle) {
  if (!viewHandle.has_value()) {
    if (ImGui::BeginMenu("Add view")) {
      uint8_t viewId;
      if (ViewRendering::RenderCreateView(
              m_controller.GetViewController(),
              m_controller.GetViewController().GetMainFilter(), viewId)) {
        viewHandle = viewId;
      }
      ImGui::EndMenu();
    }
    return;
  }

  View &view = m_controller.GetViewController().GetView<View>(*viewHandle);
  view.Render();
}

void App::InputData() {
  system(std::format("py {}\\{}", StateController::DIRECTORY,
                     StateController::INPUT_SCRIPT_PATH)
             .c_str());
}
