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
  template <typename T, typename... Args> void CreateView(Args &&...args);
  void RefreshViews();
  void RenderViews();

private:
  std::vector<std::unique_ptr<View>> m_views;
  StateController &m_controller;
};

template <typename T, typename... Args>
inline void ViewController::CreateView(Args &&...args) {
  m_views.emplace_back(std::make_unique<T>(args...));
}
