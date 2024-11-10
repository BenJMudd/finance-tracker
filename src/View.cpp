#include "View.h"
#include "ViewController.h"
#include "imgui/ImGuiDatePicker.hpp"
#include "imgui/imgui.h"
#include "imgui/implot.h"
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

  std::string_view viewName = GetViewName();
  if (ImGui::Button(
          std::format("{} (Click to open settings)", viewName).c_str(),
          ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
    ImGui::OpenPopup(std::format("popupid: {}", m_viewId).c_str());
  }
  ImGui::PopStyleColor(3);

  if (ImGui::BeginPopup(std::format("popupid: {}", m_viewId).c_str())) {
    RenderTaskbarExt();
    ImGui::Separator();
    if (ImGui::MenuItem("refresh view")) {
      Refresh();
    }
    ViewRendering::RenderSetDate(*m_dataViewer);
    ViewRendering::RenderSetCateogries(*m_dataViewer);
    if (m_dataViewer == m_viewController.GetMainFilter()) {
      if (ImGui::MenuItem("Create new filter")) {
        // TODO: need to think about the transformers
        DBFilter::SPtr newFilter = m_viewController.CreateFilter();
        SetFilter(newFilter);
        newFilter->BuildCache();
      }
    } else {
      if (ImGui::MenuItem("Reset filter")) {
        SetFilter(m_viewController.GetMainFilter());
      }
    }
    ImGui::EndPopup();
  }
}

void ListTransactionsWindow::RenderMainView() {
  static constexpr ImGuiTableFlags flags =
      ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
      ImGuiTableFlags_ContextMenuInBody;
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(&isCacheValid);
  if (ImGui::BeginTable(
          std::format("ListTransactionsWnd id:{}", m_viewId).c_str(), 4,
          flags)) {
    for (auto transactionIt = transactions.crbegin();
         transactionIt != transactions.crend(); ++transactionIt) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(UnixToStr(transactionIt->m_date).c_str());
      ImGui::TableNextColumn();
      std::string descriptionRaw = transactionIt->m_desc;
      if (m_limitDescLen && descriptionRaw.size() > m_maxLimitDescLen)
        descriptionRaw = std::format(
            "{}...", descriptionRaw.substr(0, m_maxLimitDescLen - 3));
      ImGui::Text(descriptionRaw.c_str());
      ImGui::TableNextColumn();
      ImGui::TextUnformatted(FormatTransaction(transactionIt->m_value).c_str());
      ImGui::TableNextColumn();
      ImGui::Text(
          m_dataViewer->GetCategoryName(transactionIt->m_subCat).c_str());
    }
    ImGui::EndTable();
  }
}

void ViewRendering::RenderSetCateogries(DBFilter &filter) {
  if (ImGui::BeginMenu("Set categories")) {
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
      ImGui::BeginMultiSelect(flags, 0, static_cast<int>(catMappings.size()));
      ImGui::SeparatorText(
          std::format("Category: {}", catNames[it->first]).c_str());

      for (size_t subCatId : it->second) {
        // This is obv very shit
        uint8_t u8subCatId = static_cast<uint8_t>(subCatId);
        std::string label = catNames[subCatId];
        if (ImGui::Selectable(label.c_str(),
                              filter.IsCategoryActive(u8subCatId))) {
          if (filter.IsCategoryActive(u8subCatId)) {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
              filter.SetCategoryState(u8subCatId, false);
            }
          } else {
            filter.SetCategoryState(u8subCatId, true);
          }
        }
      }

      ImGui::EndMultiSelect();
    }
    ImGui::PopItemFlag();
    ImGui::EndMenu();
  }
}

void ViewRendering::RenderSetDate(DBFilter &filter) {
  if (ImGui::BeginMenu("Set date range")) {

    std::time_t startDate(filter.GetStartDate());
    tm startDateTm;
    gmtime_s(&startDateTm, &startDate); // fuck da err

    std::time_t endDate(filter.GetEndDate());
    tm endDateTm;
    gmtime_s(&endDateTm, &endDate); // fuck da err

    if (ImGui::DatePicker("Start date", startDateTm)) {
      filter.SetStartDate(static_cast<uint32_t>(mktime(&startDateTm)) + 1);
      filter.BuildCache();
    }

    if (ImGui::DatePicker("End date", endDateTm)) {
      filter.SetEndDate(static_cast<uint32_t>(mktime(&endDateTm)) + 1);
      filter.BuildCache();
    }
    ImGui::EndMenu();
  }
}

const std::map<ViewRendering::types, std::string_view>
    ViewRendering::VIEW_NAMES = {
        {types::list_transaction, ListTransactionsWindow::VIEW_NAME},
        {types::aggregate_cat, AggregateByCategoryView::VIEW_NAME},
        {types::transaction_value_graph, AggregateGraph::VIEW_NAME},
};

bool ViewRendering::RenderCreateView(ViewController &controller,
                                     DBFilter::SPtr filter, uint8_t &viewId) {
  for (const auto &[type, name] : VIEW_NAMES) {
    if (ImGui::MenuItem(name.data())) {
      viewId = CreateView(controller, filter, type);
      controller.GetView<View>(viewId).Refresh();
      return true;
    }
  }

  return false;
}

uint8_t ViewRendering::CreateView(ViewController &controller,
                                  DBFilter::SPtr filter, types viewType) {
  switch (viewType) {
  case ViewRendering::types::list_transaction:
    return controller.CreateView<ListTransactionsWindow>(filter);
  case ViewRendering::types::aggregate_cat:
    return controller.CreateView<AggregateByCategoryView>(filter);
  case ViewRendering::types::transaction_value_graph:
    return controller.CreateView<AggregateGraph>(filter);
  default:
    break;
  }

  return 0;
}

std::string View::UnixToStr(uint32_t unixTime) {
  std::time_t temp = unixTime;
  std::tm t;
  gmtime_s(&t, &temp); // fuck da err
  std::stringstream ss;
  ss << std::put_time(&t, "%d-%m-%y");
  return ss.str();
}

std::string View::FormatTransaction(double val) {
  std::string rawValue = std::to_string(abs(val));
  std::string substrRawValue = rawValue.substr(0, rawValue.find('.') + 3);
  std::string formattedValue =
      std::format("{}{}", val < 0.0f ? "-" : "", substrRawValue);
  return formattedValue;
}

void AggregateByCategoryView::RenderMainView() {
  AggregateTransformer &transformer =
      m_dataViewer->GetDataTansformer<AggregateTransformer>(m_viewId);
  const Aggregate &aggregate = transformer.GetAggregate();
  auto &catNames = m_dataViewer->GetCategoryNames();
  ImGui::Text("Total: %s", FormatTransaction(aggregate.m_total).c_str());
  for (const auto &[catId, catTransactionData] : aggregate.m_data) {
    if (ImGui::TreeNode(std::to_string(catId).c_str(), "%s: %s",
                        catNames[catId].c_str(),
                        FormatTransaction(catTransactionData.first).c_str())) {

      for (const auto &[subCat, value] : catTransactionData.second) {
        ImGui::Text("%s: %s", catNames[subCat].c_str(),
                    FormatTransaction(value).c_str());
      }
      ImGui::TreePop();
    }

    if (ImGui::BeginPopupContextItem(std::to_string(catId).c_str())) {
      ImGui::Text("This a popup for %s!", catNames[catId].c_str());
      if (ImGui::Button("Close"))
        ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
    }
  }
}

void AggregateByCategoryView::RenderTaskbarExt() {}

void AggregateGraph::RenderMainView() {
  const std::vector<AggregateTransformer::Aggregate> *rawData =
      m_dataViewer->GetDataTansformer<AggregateTransformer>(m_viewId)
          .GetAggregateByInterval();

  assert(rawData);

  std::vector<double> xAxisDate;
  xAxisDate.reserve(rawData->size());

  std::vector<double> yAxisTotal;
  yAxisTotal.reserve(rawData->size());

  for (const auto &aggregateEntry : *rawData) {
    xAxisDate.emplace_back(aggregateEntry.m_startDate);
    yAxisTotal.emplace_back(-aggregateEntry.m_total);
  }

  static constexpr ImPlotAxisFlags yFlags = ImPlotAxisFlags_AutoFit;

  if (ImPlot::BeginPlot("Stock Prices")) {
    ImPlot::SetupAxes("Days", "Price", 0, yFlags);
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
    ImPlot::SetupAxesLimits(*xAxisDate.begin(), xAxisDate.back(), -2000, 2000);
    ImPlot::PlotLine("Total", xAxisDate.data(), yAxisTotal.data(),
                     rawData->size());
    ImPlot::EndPlot();
  }
}
