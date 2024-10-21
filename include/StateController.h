#pragma once

#include "DBFilter.h"
#include "ViewController.h"
#include <memory>
#include <vector>

class StateController {
public:
  StateController() : m_dbHandler("res/finances.db") {
    m_viewController = std::make_unique<ViewController>(*this);
  }

  ViewController &GetViewController() { return *m_viewController.get(); }
  DBHandler &GetDB() { return m_dbHandler; }

  std::vector<std::string> GetAllCategoryNames() {
    return m_dbHandler.GetCategoryNames();
  }

private:
  std::unique_ptr<ViewController> m_viewController;
  DBHandler m_dbHandler;
};
