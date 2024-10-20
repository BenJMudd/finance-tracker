#include "ViewController.h"

ViewController::ViewController(StateController &controller)
    : m_controller(controller), m_viewIdCounter(0) {}

void ViewController::RefreshViews() {
  for (auto &[id, view] : m_views) {
    view->GetFilter().BuildCache();
    view->Refresh();
  }
}

void ViewController::RenderViews() {
  for (auto &[id, view] : m_views) {
    view->Render();
  }
}
