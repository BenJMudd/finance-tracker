#pragma once

#include "DBFilter.h"
#include "imgui/imgui.h"
#include <vector>

class ViewController;

class View {
public:
  View(uint8_t viewId, ViewController &viewController)
      : m_viewId(viewId), m_viewController(viewController) {}
  virtual void Refresh() = 0;
  virtual void Render() = 0;
  virtual bool GetCacheValidity() = 0;

  static void RenderSetCateogries(DBFilter &filter);
  static void RenderSetDate(DBFilter &filter);

protected:
  std::string UnixToStr(uint32_t unixTime);

  uint8_t m_viewId;
  ViewController &m_viewController;
};

class SingleFilterView : public View {
public:
  SingleFilterView(uint8_t viewId, ViewController &viewController,
                   DBFilter::SPtr dataViewer)
      : m_dataViewer(dataViewer), View(viewId, viewController) {}
  virtual void Refresh() { m_dataViewer->BuildCache(); }
  virtual void Render() override;
  virtual void RenderMainView() = 0;
  void RenderTaskbar();
  virtual void RenderTaskbarExt() = 0;

  virtual DBFilter &GetFilter() { return *m_dataViewer.get(); }
  virtual void SetFilter(DBFilter::SPtr filter) { m_dataViewer = filter; }
  virtual bool GetCacheValidity() override {
    return m_dataViewer->IsCacheValid();
  }
  virtual std::string GetViewName() = 0;

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

  std::string GetViewName() override { return "List transactions view"; }
  void RenderMainView() override;
  void RenderTaskbarExt() override;

private:
  size_t m_maxLimitDescLen;
  bool m_limitDescLen;
};
