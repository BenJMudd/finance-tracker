#include "ViewController.h"
#include "StateController.h"
#include "imgui/imgui.h"

ViewController::ViewController(StateController &controller)
    : m_controller(controller), m_viewIdCounter(0) {
  m_mainFilter = std::make_shared<DBFilter>(m_controller.GetDB());
}

DBFilter::SPtr ViewController::CreateFilter() {
  return std::make_shared<DBFilter>(m_controller.GetDB());
}

void ViewController::RefreshViews() {
  for (auto &[id, view] : m_views) {
    view->Refresh();
  }
}

void ViewController::RenderViews() {
  for (auto &[id, view] : m_views) {
    view->Render();
  }
}
