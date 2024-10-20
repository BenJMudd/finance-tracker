#pragma once
// clang-format off
#include "View.h"
#include <memory>
#include <utility>
#include <vector>
// clang-format on

class StateController;

class ViewController {
public:
  ViewController(StateController &controller);
  // TODO: add template constraints
  template <typename T, typename... Args> uint8_t CreateView(Args &&...args);
  DBFilter &CreateFilter();

  // TODO: add template constraints
  template <typename T> T &GetView(uint8_t viewId) {
    T *view = dynamic_cast<T *>(m_views[viewId].get());
    assert(view);
    return *view;
  }
  void RefreshViews();
  void RenderViews();

private:
  std::map<size_t, std::unique_ptr<View>> m_views;
  std::vector<std::unique_ptr<DBFilter>> m_filters;
  StateController &m_controller;

  uint8_t m_viewIdCounter;
};

template <typename T, typename... Args>
inline uint8_t ViewController::CreateView(Args &&...args) {
  m_views[++m_viewIdCounter] = std::make_unique<T>(args...);
  return m_viewIdCounter;
}
