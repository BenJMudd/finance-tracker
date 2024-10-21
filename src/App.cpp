#include "App.h"
#include "imgui/imgui.h"

App::App() {}

void App::Render() {
  RenderTitleBar();
  RenderFullScreen();
  RenderMainView();
  ImGui::End(); // for fullscreen
}

void App::RenderTitleBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Set date range")) {
      }
      if (ImGui::MenuItem("Set categories")) {
        m_controller.GetViewController().GetMainFilter()->OmitCategory(4);
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

void App::RenderFullScreen() {

  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoSavedSettings;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  bool *p_open = nullptr;
  ImGui::Begin("Example: Fullscreen window", p_open, flags);
}

void App::RenderViewWnd(std::optional<uint8_t> &viewHandle) {
  if (!viewHandle.has_value()) {
    if (ImGui::BeginMenu("Add view")) {
      if (ImGui::MenuItem("Transactions list")) {
        viewHandle =
            m_controller.GetViewController().CreateView<ListTransactionsWindow>(
                m_controller.GetViewController().GetMainFilter());
      }
      ImGui::EndMenu();
    }
    return;
  }

  View &view = m_controller.GetViewController().GetView<View>(*viewHandle);
  if (ImGui::BeginMenu(
          std::format("View id: {}, cache validity: {}", *viewHandle,
                      view.GetCacheValidity() ? "valid" : "invalid")
              .c_str())) {
    if (ImGui::MenuItem("refresh view")) {
      view.Refresh();
    }
    if (ImGui::MenuItem("Filter")) {
      view.SetFilter(m_controller.GetViewController().CreateFilter());
      view.GetFilter().OmitCategory(4);
    }
    if (ImGui::MenuItem("Reset filter")) {
      view.SetFilter(m_controller.GetViewController().GetMainFilter());
    }
    ImGui::EndMenu();
  }

  ImGui::BeginChild(std::format("view {}", *viewHandle).c_str());
  view.Render();
  ImGui::EndChild();
}
