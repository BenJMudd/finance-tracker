#include "ViewController.h"
#include "StateController.h"

ViewController::ViewController(StateController &controller)
    : m_controller(controller), m_viewIdCounter(0) {
  // This is a ridiculously stupid hack
  m_filters.reserve(100);
}

DBFilter &ViewController::CreateFilter() {
  m_filters.emplace_back(std::make_unique<DBFilter>(m_controller.GetDB()));
  return *m_filters.back();
}

void ViewController::RefreshViews() {
  for (auto &[id, view] : m_views) {
    DBFilter *filter = view->GetFilter();
    if (filter)
      filter->BuildCache();

    view->Refresh();
  }
}

void ViewController::RenderViews() {
  for (auto &[id, view] : m_views) {
    view->Render();
  }
}
