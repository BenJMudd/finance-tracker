#include "App.h"
#include "imgui/imgui.h"

App::App()
    : m_demoWindowOpen(false),
      m_mainViewFilters(m_controller.GetDB().GetCategoryNames().size(), true) {
  ImGui::GetIO().WantCaptureKeyboard = true;
}

void App::Render() {
  RenderTitleBar();
  if (m_demoWindowOpen)
    ImGui::ShowDemoWindow(&m_demoWindowOpen);

  RenderMainView();
}

void App::RenderTitleBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Render demo window")) {
        m_demoWindowOpen = true;
      }
      if (ImGui::MenuItem("Set date range")) {
      }
      if (ImGui::BeginMenu("SetCategories")) {
        DBFilter::SPtr &mainFilter =
            m_controller.GetViewController().GetMainFilter();
        SingleFilterView::RenderSetCategories(mainFilter);
        ImGui::EndMenu();
      }
      if (ImGui::MenuItem("Refresh all views")) {
        m_controller.GetViewController().RefreshViews();
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
      if (ImGui::MenuItem("Transactions list")) {
        viewHandle =
            m_controller.GetViewController().CreateView<ListTransactionsWindow>(
                m_controller.GetViewController().GetMainFilter());
        m_controller.GetViewController()
            .GetView<ListTransactionsWindow>(*viewHandle)
            .Refresh();
      }
      ImGui::EndMenu();
    }
    return;
  }
  View &view = m_controller.GetViewController().GetView<View>(*viewHandle);
  ImGui::BeginChild(std::format("view {}", *viewHandle).c_str());
  view.Render();
  ImGui::EndChild();
}
