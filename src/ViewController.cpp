#include "ViewController.h"

ViewController::ViewController(StateController &controller)
    : m_controller(controller) {}

void ViewController::RefreshViews() {
  for (auto &view : m_views) {
    view->GetFilter().BuildCache();
    view->Refresh();
  }
}

void ViewController::RenderViews() {
  for (auto &view : m_views) {
    view->Render();
  }
}
