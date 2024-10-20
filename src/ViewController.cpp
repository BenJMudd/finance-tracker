#include "ViewController.h"

void ViewController::RefreshViews() {
  for (auto &view : m_views) {
    view->Refresh();
  }
}
