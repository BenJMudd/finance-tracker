#pragma once

#include "DBFilter.h"
#include "ViewController.h"
#include <memory>
#include <vector>

class StateController {
public:
  static constexpr std::string_view DIRECTORY =
      "C:\\Users\\Benja\\Documents\\prog\\finance-tracker";
  static constexpr std::string_view DB_PATH = "res\\finances.db";
  static constexpr std::string_view INPUT_SCRIPT_PATH =
      "scripts\\auto_input_ui.py";

  StateController() : m_dbHandler(std::format("{}\\{}", DIRECTORY, DB_PATH)) {
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
