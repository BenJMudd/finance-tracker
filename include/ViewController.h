#pragma once
#include "View.h"
#include <memory>
#include <utility>
#include <vector>

class ViewController {
public:
  ViewController() {}
  template <typename T, typename... Args> void CreateView(Args &&...args);
  void RefreshViews();

private:
  std::vector<std::unique_ptr<View>> m_views;
};

template <typename T, typename... Args>
inline void ViewController::CreateView(Args &&...args) {
  m_views.emplace_back(std::make_unique<T>(args...));
}
