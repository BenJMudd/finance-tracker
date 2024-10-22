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
        RenderSetCategories(
            m_mainViewFilters,
            *m_controller.GetViewController().GetMainFilter().get());
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
  if (ImGui::BeginMenu(
          std::format("View id: {}, cache validity: {}", *viewHandle,
                      view.GetCacheValidity() ? "valid" : "invalid")
              .c_str())) {
    if (ImGui::MenuItem("refresh view")) {
      view.Refresh();
    }
    auto viewIt = m_viewsWithCustomFilters.find(*viewHandle);
    if (viewIt != m_viewsWithCustomFilters.end()) {
      RenderSetCategories(viewIt->second.first, *viewIt->second.second);
    } else {
      if (ImGui::MenuItem("Filter")) {

        DBFilter::SPtr newFilter =
            m_controller.GetViewController().CreateFilter();
        std::vector<bool> filterTracking(
            m_controller.GetDB().GetCategoryNames().size(), true);
        m_controller.GetViewController()
            .GetView<View>(*viewHandle)
            .SetFilter(newFilter);
        m_viewsWithCustomFilters.insert(
            {*viewHandle, {filterTracking, newFilter.get()}});
      }
      if (ImGui::MenuItem("Reset filter")) {
        view.SetFilter(m_controller.GetViewController().GetMainFilter());
      }
    }
    ImGui::EndMenu();
  }

  ImGui::BeginChild(std::format("view {}", *viewHandle).c_str());
  view.Render();
  ImGui::EndChild();
}

void App::RenderSetCategories(std::vector<bool> &viewFilter, DBFilter &filter) {
  ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
  bool changesMade = false;
  if (ImGui::Button("Select all")) {
    std::fill(viewFilter.begin(), viewFilter.end(), true);
    changesMade = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Deselect all")) {
    std::fill(viewFilter.begin(), viewFilter.end(), false);
    changesMade = true;
  }

  const std::vector<std::string> &catNames =
      m_controller.GetDB().GetCategoryNames();
  auto &catMappings = m_controller.GetDB().GetCategoryMapping();

  static ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ScopeRect;
  for (auto it = catMappings.cbegin(); it != catMappings.end(); ++it) {
    ImGui::BeginMultiSelect(flags, 0, catMappings.size());
    ImGui::SeparatorText(
        std::format("Category: {}", catNames[it->first]).c_str());

    for (size_t subCatId : it->second) {
      std::string label = catNames[subCatId];
      if (ImGui::Selectable(label.c_str(), viewFilter[subCatId])) {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
          viewFilter[subCatId] = false;
        } else {
          viewFilter[subCatId] = true;
        }
        changesMade = true;
      }
    }

    // Apply multi-select requests
    ImGui::EndMultiSelect();
  }
  ImGui::PopItemFlag();

  if (changesMade)
    UpdateFilter(viewFilter, filter);
}

void App::UpdateFilter(std::vector<bool> &viewFilter, DBFilter &filter) {
  filter.ClearFilter();
  for (size_t i = 0; i < viewFilter.size(); ++i) {
    if (!viewFilter[i]) {
      // Is category or subcategory
      bool isCategory = m_controller.GetDB().GetCategoryMapping().find(i) !=
                        m_controller.GetDB().GetCategoryMapping().end();

      if (!isCategory)
        filter.OmitSubCategory(i);
    }
  }
}
