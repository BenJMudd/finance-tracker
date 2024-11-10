#pragma once

#include "DBFilter.h"
#include "imgui/imgui.h"
#include <vector>

class ViewController;

class ViewRendering {
public:
  enum class types { list_transaction, aggregate_cat, transaction_value_graph };
  static void RenderSetCateogries(DBFilter &filter);
  static void RenderSetDate(DBFilter &filter);
  static bool RenderCreateView(ViewController &controller,
                               DBFilter::SPtr filter, uint8_t &viewId);

  static const std::map<types, std::string_view> VIEW_NAMES;

private:
  static uint8_t CreateView(ViewController &controller, DBFilter::SPtr filter,
                            types viewType);
}; // namespace ViewRendering

class View {
public:
  View(uint8_t viewId, ViewController &viewController)
      : m_viewId(viewId), m_viewController(viewController) {}
  virtual void Refresh() = 0;
  virtual void Render() = 0;
  virtual bool GetCacheValidity() = 0;

protected:
  std::string UnixToStr(uint32_t unixTime);
  std::string FormatTransaction(double val);

  uint8_t m_viewId;
  ViewController &m_viewController;
};

class SingleFilterView : public View {
public:
  SingleFilterView(uint8_t viewId, ViewController &viewController,
                   DBFilter::SPtr dataViewer)
      : m_dataViewer(dataViewer), View(viewId, viewController) {}
  void Refresh() override { m_dataViewer->BuildCache(); }
  void Render() override;
  virtual void RenderMainView() = 0;
  void RenderTaskbar();
  virtual void RenderTaskbarExt() = 0;

  DBFilter &GetFilter() { return *m_dataViewer.get(); }
  virtual void SetFilter(DBFilter::SPtr filter) {
    m_dataViewer->NotifyViewDropped(m_viewId);
    m_dataViewer = filter;
  }

  bool GetCacheValidity() override { return m_dataViewer->IsCacheValid(); }
  virtual std::string_view GetViewName() = 0;

protected:
  DBFilter::SPtr m_dataViewer;

private:
  static constexpr ImVec4 m_colButtonMainFilter =
      ImVec4(0.2f, 0.5f, 0.2f, 1.0f);
  static constexpr ImVec4 m_colButtonMainFilterHover =
      ImVec4(0.2f, 0.6f, 0.2f, 1.0f);
  static constexpr ImVec4 m_colButtonMainFilterActive =
      ImVec4(0.2f, 0.7f, 0.2f, 1.0f);

  static constexpr ImVec4 m_colButtonSepFilter = ImVec4(0.2f, 0.2f, 0.5f, 1.0f);
  static constexpr ImVec4 m_colButtonSepFilterHover =
      ImVec4(0.2f, 0.2f, 0.6f, 1.0f);
  static constexpr ImVec4 m_colButtonSepFilterActive =
      ImVec4(0.2f, 0.2f, 0.7f, 1.0f);

  static constexpr ImVec4 m_colButtonInvalid = ImVec4(0.5f, 0.2f, 0.2f, 1.0f);
  static constexpr ImVec4 m_colButtonInvalidHover =
      ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
  static constexpr ImVec4 m_colButtonInvalidActive =
      ImVec4(0.7f, 0.2f, 0.2f, 1.0f);
};

class ListTransactionsWindow : public SingleFilterView {
public:
  ListTransactionsWindow(uint8_t viewId, ViewController &viewController,
                         DBFilter::SPtr filter)
      : m_limitDescLen(true), m_maxLimitDescLen(40),
        SingleFilterView(viewId, viewController, filter) {}

  static constexpr std::string_view VIEW_NAME = "Transactions list";
  std::string_view GetViewName() override { return VIEW_NAME; }

  void RenderMainView() override;
  void RenderTaskbarExt() override{};

private:
  size_t m_maxLimitDescLen;
  bool m_limitDescLen;
};

class AggregateByCategoryView : public SingleFilterView {
public:
  AggregateByCategoryView(uint8_t viewId, ViewController &viewController,
                          DBFilter::SPtr filter)
      : SingleFilterView(viewId, viewController, filter) {}

  static constexpr std::string_view VIEW_NAME = "Transactions By Category";
  std::string_view GetViewName() override { return VIEW_NAME; }
  void RenderMainView() override;
  void RenderTaskbarExt() override;

private:
  using Aggregate = AggregateTransformer::Aggregate;
};

class AggregateGraph : public SingleFilterView {
public:
  AggregateGraph(uint8_t viewId, ViewController &viewController,
                 DBFilter::SPtr filter)
      : SingleFilterView(viewId, viewController, filter) {
    // Set request for interval data
    m_dataViewer->GetDataTansformer<AggregateTransformer>(m_viewId)
        .SetBuildByInterval(true);
  }

  ~AggregateGraph() {
    m_dataViewer->GetDataTansformer<AggregateTransformer>(m_viewId)
        .SetBuildByInterval(false);
  }

  virtual void SetFilter(DBFilter::SPtr filter) {
    SingleFilterView::SetFilter(filter);
    m_dataViewer->GetDataTansformer<AggregateTransformer>(m_viewId)
        .SetBuildByInterval(true);
  }

  static constexpr std::string_view VIEW_NAME = "Transactions value graph";
  std::string_view GetViewName() override { return VIEW_NAME; }
  void RenderMainView() override;
  void RenderTaskbarExt() override{};

private:
  using Aggregate = AggregateTransformer::Aggregate;
};
