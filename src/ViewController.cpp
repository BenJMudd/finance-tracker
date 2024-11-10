#include "ViewController.h"
#include "StateController.h"
#include "imgui/imgui.h"

ViewController::ViewController(StateController &controller)
    : m_controller(controller), m_viewIdCounter(0) {
  m_mainFilter = std::make_shared<DBFilter>(m_controller.GetDB());
}

void ViewController::SetViewAsExternal(uint8_t viewId) {
  auto viewIt = m_views.find(viewId);
  assert(viewIt != m_views.end());
  m_externalViews.insert({viewId, viewIt->second.get()});
}

void ViewController::DeleteView(uint8_t viewId) {
  auto exViewIt = m_externalViews.find(viewId);
  if (exViewIt != m_externalViews.end()) {
    m_externalViews.erase(exViewIt);
  }

  auto viewIt = m_views.find(viewId);
  assert(viewIt != m_views.end());
  m_views.erase(viewIt);
}

DBFilter::SPtr ViewController::CreateFilter() {
  return std::make_shared<DBFilter>(m_controller.GetDB());
}

void ViewController::RefreshViews() {
  for (auto &[id, view] : m_views) {
    view->Refresh();
  }
}

void ViewController::RenderExternalViews() {
  for (auto &[id, view] : m_views) {
    view->Render();
  }
}
