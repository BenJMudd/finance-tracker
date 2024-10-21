#pragma once
#include "StateController.h"

class App {
public:
  App();

  void Render();

private:
  void RenderTitleBar();
  void RenderMainView();
  void RenderFullScreen();
  void RenderViewWnd(std::optional<uint8_t> &viewHandle);
  StateController m_controller;
  std::optional<uint8_t> m_leftViewHandle;
  std::optional<uint8_t> m_topRightViewHandle;
  std::optional<uint8_t> m_bottomRightViewHandle;
};
