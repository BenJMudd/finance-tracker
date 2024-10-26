#include "View.h"
#include "ViewController.h"
#include "imgui/imgui.h"
#include <Windows.h>
#include <time.h>

using namespace std::chrono;

void SingleFilterView::Render() {
  RenderTaskbar();

  ImGui::BeginChild(std::format("viewint {}", m_viewId).c_str());
  RenderMainView();
  ImGui::EndChild();
}

void SingleFilterView::RenderTaskbar() {

  if (!m_dataViewer->IsCacheValid()) {
    ImGui::PushStyleColor(ImGuiCol_Button, m_colButtonInvalid);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colButtonInvalidHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colButtonInvalidActive);
  } else if (m_dataViewer == m_viewController.GetMainFilter()) {
    ImGui::PushStyleColor(ImGuiCol_Button, m_colButtonMainFilter);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colButtonMainFilterHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colButtonMainFilterActive);
  } else {
    ImGui::PushStyleColor(ImGuiCol_Button, m_colButtonSepFilter);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colButtonSepFilterHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colButtonSepFilterActive);
  }

  std::string viewName = GetViewName();
  if (ImGui::Button(
          std::format("{} (Click to open settings)", viewName).c_str(),
          ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
    ImGui::OpenPopup(std::format("popupid: {}", m_viewId).c_str());
  }
  ImGui::PopStyleColor(3);

  if (ImGui::BeginPopup(std::format("popupid: {}", m_viewId).c_str())) {
    RenderTaskbarExt();
    ImGui::Separator();
    if (m_dataViewer == m_viewController.GetMainFilter()) {

      if (ImGui::MenuItem("Create new filter")) {
        DBFilter::SPtr newFilter = m_viewController.CreateFilter();
        SetFilter(newFilter);
        newFilter->BuildCache();
      }
    } else {
      if (ImGui::Button("refresh view")) {
        Refresh();
      }
      ImGui::SameLine();
      if (ImGui::Button("Reset filter")) {
        SetFilter(m_viewController.GetMainFilter());
      }
      ImGui::Separator();
      RenderSetCateogries(*m_dataViewer);
    }
    ImGui::EndPopup();
  }
}

void ListTransactionsWindow::RenderMainView() {
  static ImGuiTableFlags flags =
      ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
      ImGuiTableFlags_ContextMenuInBody;
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(isCacheValid);
  if (ImGui::BeginTable(
          std::format("ListTransactionsWnd id:{}", m_viewId).c_str(), 4,
          flags)) {
    for (auto &transaction : transactions) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(UnixToStr(transaction.m_date).c_str());
      ImGui::TableNextColumn();
      std::string descriptionRaw = transaction.m_desc;
      if (m_limitDescLen && descriptionRaw.size() > m_maxLimitDescLen)
        descriptionRaw = std::format(
            "{}...", descriptionRaw.substr(0, m_maxLimitDescLen - 3));
      ImGui::Text(descriptionRaw.c_str());
      ImGui::TableNextColumn();
      std::string rawValue = std::to_string(abs(transaction.m_value));
      std::string substrRawValue = rawValue.substr(0, rawValue.find('.') + 3);
      std::string formattedValue = std::format(
          "{}{}", transaction.m_value < 0.0f ? "-" : "", substrRawValue);
      ImGui::TextUnformatted(formattedValue.c_str());
      ImGui::TableNextColumn();
      ImGui::Text(m_dataViewer->GetCategoryName(transaction.m_subCat).c_str());
    }
    ImGui::EndTable();
  }
}

void ListTransactionsWindow::RenderTaskbarExt() {
  ImGui::Checkbox("Limit description length", &m_limitDescLen);
  ImGui::Text("Set description length limit");
  ImGui::SliderInt("##mylabel", (int *)&m_maxLimitDescLen, 0, 60, "%d");
}

void View::RenderSetCateogries(DBFilter &filter) {
  ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
  if (ImGui::Button("Select all")) {
    filter.SetAllCategories();
  }
  ImGui::SameLine();
  if (ImGui::Button("Deselect all")) {
    filter.OmitAllCategories();
  }

  const std::vector<std::string> &catNames = filter.GetCategoryNames();
  auto &catMappings = filter.GetCategoryMapping();

  static ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ScopeRect;
  for (auto it = catMappings.cbegin(); it != catMappings.end(); ++it) {
    ImGui::BeginMultiSelect(flags, 0, catMappings.size());
    ImGui::SeparatorText(
        std::format("Category: {}", catNames[it->first]).c_str());

    for (size_t subCatId : it->second) {
      std::string label = catNames[subCatId];
      if (ImGui::Selectable(label.c_str(), filter.IsCategoryActive(subCatId))) {
        if (filter.IsCategoryActive(subCatId)) {
          if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            filter.SetCategoryState(subCatId, false);
          }
        } else {
          filter.SetCategoryState(subCatId, true);
        }
      }
    }

    ImGui::EndMultiSelect();
  }
  ImGui::PopItemFlag();
}

std::string View::UnixToStr(uint32_t unixTime) {
  std::time_t temp = unixTime;
  std::tm t;
  gmtime_s(&t, &temp); // fuck da err
  std::stringstream ss;
  ss << std::put_time(&t, "%d-%m-%y");
  return ss.str();
}
