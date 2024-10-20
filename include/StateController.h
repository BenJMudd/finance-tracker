#pragma once

#include "DBFilter.h"
#include "ViewController.h"
#include <memory>
#include <vector>
class StateController {
public:
  StateController() : m_dbHandler("res/finances.db"), m_viewController(*this) {
    setlocale(LC_ALL, "en_GB.UTF-8");
  }

  ViewController &GetViewController() { return m_viewController; }
  std::shared_ptr<DBFilter> CreateFilter() {
    m_filters.emplace_back(std::make_shared<DBFilter>(m_dbHandler));
    return m_filters.back();
  }

private:
  ViewController m_viewController;
  DBHandler m_dbHandler;
  std::vector<std::shared_ptr<DBFilter>> m_filters;
};
