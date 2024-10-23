#include "View.h"
#include "ViewController.h"
#include "imgui/imgui.h"
#include <Windows.h>
#include <chrono>

using namespace std::chrono;

void SingleFilterView::Render() {
  RenderTaskbar(m_viewId, m_dataViewer, *this, m_viewController);

  ImGui::BeginChild(std::format("viewint {}", m_viewId).c_str());
  RenderMainView();
  ImGui::EndChild();
}

void SingleFilterView::RenderTaskbar(uint8_t viewId, DBFilter::SPtr &filter,
                                     SingleFilterView &view,
                                     ViewController &viewController) {
  ImGui::PushStyleColor(ImGuiCol_Button,
                        !filter->IsCacheValid()
                            ? ImVec4(1.0f, 0.5f, 0.5f, 1.0f)
                            : (filter == viewController.GetMainFilter()
                                   ? ImVec4(0.2f, 0.5f, 0.2f, 1.0f)
                                   : ImVec4(0.2f, 0.2f, 0.5f, 1.0f)));
  if (ImGui::Button(
          std::format("{} (Click to open settings)",
                      viewController.GetView<ListTransactionsWindow>(viewId)
                          .GetViewName(),
                      filter->IsCacheValid() ? "valid" : "invalid")
              .c_str(),
          ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
    ImGui::OpenPopup(std::format("popupid: {}", viewId).c_str());
  }
  ImGui::PopStyleColor();
  if (ImGui::BeginPopup(std::format("popupid: {}", viewId).c_str())) {
    if (ImGui::MenuItem("refresh view")) {
      view.Refresh();
    }
    if (filter == viewController.GetMainFilter()) {

      if (ImGui::MenuItem("Filter")) {
        DBFilter::SPtr newFilter = viewController.CreateFilter();
        viewController.GetView<SingleFilterView>(viewId).SetFilter(newFilter);
        newFilter->BuildCache();
      }
      if (ImGui::MenuItem("Reset filter")) {
        viewController.GetView<SingleFilterView>(viewId).SetFilter(
            viewController.GetMainFilter());
      }
    } else {
      if (ImGui::MenuItem("Reset filter")) {
        viewController.GetView<SingleFilterView>(viewId).SetFilter(
            viewController.GetMainFilter());
      }
      RenderSetCategories(filter);
    }
    ImGui::EndPopup();
  }
}
void SingleFilterView::RenderSetCategories(DBFilter::SPtr &filter) {

  ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
  bool changesMade = false;
  if (ImGui::Button("Select all")) {
    filter->SetAllCategories();
  }
  ImGui::SameLine();
  if (ImGui::Button("Deselect all")) {
    filter->OmitAllCategories();
  }

  const std::vector<std::string> &catNames = filter->GetCategoryNames();
  auto &catMappings = filter->GetCategoryMapping();

  static ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ScopeRect;
  for (auto it = catMappings.cbegin(); it != catMappings.end(); ++it) {
    ImGui::BeginMultiSelect(flags, 0, catMappings.size());
    ImGui::SeparatorText(
        std::format("Category: {}", catNames[it->first]).c_str());

    for (size_t subCatId : it->second) {
      std::string label = catNames[subCatId];
      if (ImGui::Selectable(label.c_str(),
                            filter->IsCategoryActive(subCatId))) {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
          filter->SetCategoryState(subCatId, false);
        } else {
          filter->SetCategoryState(subCatId, true);
        }
        changesMade = true;
      }
    }

    ImGui::EndMultiSelect();
  }
  ImGui::PopItemFlag();
}

void ListTransactionsWindow::RenderMainView() {
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(isCacheValid);
  if (ImGui::BeginTable(std::format("id : {}, Cache validity: {}",
                                    (uint64_t)this,
                                    (isCacheValid ? "true" : "False"))
                            .c_str(),
                        4)) {
    for (auto &transaction : transactions) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("Date %u", transaction.m_date);
      ImGui::TableNextColumn();
      ImGui::Text(transaction.m_desc.c_str());
      ImGui::TableNextColumn();
      ImGui::Text(std::to_string(transaction.m_value).c_str());
      ImGui::TableNextColumn();
      ImGui::Text(m_dataViewer->GetCategoryName(transaction.m_subCat).c_str());
    }
    ImGui::EndTable();
  }
}

void View::RenderTaskbar(ViewController &viewController, uint8_t viewId) {}
