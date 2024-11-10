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
  void SetViewAsExternal(uint8_t viewId);

  void DeleteView(uint8_t viewId);

  DBFilter::SPtr CreateFilter();
  DBFilter::SPtr &GetMainFilter() { return m_mainFilter; }

  // TODO: add template constraints
  template <typename T> T &GetView(uint8_t viewId) {
    T *view = dynamic_cast<T *>(m_views[viewId].get());
    assert(view);
    return *view;
  }
  void RefreshViews();
  void RenderExternalViews();

private:
  std::map<uint8_t, std::unique_ptr<View>> m_views;
  std::map<uint8_t, View *> m_externalViews;
  StateController &m_controller;
  std::shared_ptr<DBFilter> m_mainFilter;

  uint8_t m_viewIdCounter;
};

template <typename T, typename... Args>
inline uint8_t ViewController::CreateView(Args &&...args) {
  m_views[m_viewIdCounter] =
      std::make_unique<T>(m_viewIdCounter, *this, args...);
  return m_viewIdCounter++;
}
