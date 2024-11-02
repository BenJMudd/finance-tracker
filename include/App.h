#pragma once
#include "StateController.h"
#include "imgui/imgui.h"

class App {
public:
  App();

  void Render();

private:
  void RenderTitleBar();
  void RenderMainView();
  void RenderViewWnd(std::optional<uint8_t> &viewHandle);

  void InputData();

  StateController m_controller;
  std::optional<uint8_t> m_leftViewHandle;
  std::optional<uint8_t> m_topRightViewHandle;
  std::optional<uint8_t> m_bottomRightViewHandle;
  std::map<uint8_t, std::pair<std::vector<bool>, DBFilter *>>
      m_viewsWithCustomFilters;
  bool m_demoWindowOpen;
  std::vector<bool> m_mainViewFilters;
};
