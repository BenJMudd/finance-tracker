#pragma once

#include "DBFilter.h"
#include <vector>

class ViewController;

class View {
public:
  View(uint8_t viewId, ViewController &viewController)
      : m_viewId(viewId), m_viewController(viewController) {}
  virtual void Refresh() = 0;
  virtual void Render() = 0;
  virtual bool GetCacheValidity() = 0;
  static void RenderTaskbar(ViewController &viewController, uint8_t viewId);

protected:
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
  static void RenderTaskbar(uint8_t viewId, DBFilter::SPtr &filter,
                            SingleFilterView &view,
                            ViewController &viewController);
  static void RenderSetCategories(DBFilter::SPtr &filter);

  virtual DBFilter &GetFilter() { return *m_dataViewer.get(); }
  virtual void SetFilter(DBFilter::SPtr filter) { m_dataViewer = filter; }
  virtual bool GetCacheValidity() override {
    return m_dataViewer->IsCacheValid();
  }

protected:
  DBFilter::SPtr m_dataViewer;
};

class ListTransactionsWindow : public SingleFilterView {
public:
  ListTransactionsWindow(uint8_t viewId, ViewController &viewController,
                         DBFilter::SPtr filter)
      : SingleFilterView(viewId, viewController, filter) {}

  void RenderMainView() override;
};
