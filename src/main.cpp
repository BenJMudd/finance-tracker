#include "DBFilter.h"
#include "DBHandler.h"
#include "DataViewer.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "View.h"
#include "ViewController.h"
#include "sqlite3.h"
#include <string>
#include <tuple>
#include <vector>

int main() {
  DBHandler db("res/finances.db");
  DBFilter filter(db);
  filter.SetStartDate(25);
  filter.OmitSubCategory(2);
  DataViewer viewer(filter);
  auto &transactions = viewer.GetTransactions();
  ViewController viewController;

  class TestView : public View {
  public:
    TestView(DataViewer &dataViewer, int one, int two)
        : m_one(one), m_two(two), View(dataViewer) {}

    void Refresh() override { std::cout << "TestViewOne refresh" << std::endl; }

  private:
    int m_one;
    int m_two;
  };

  class TestViewTwo : public View {
  public:
    TestViewTwo(DataViewer &dataViewer, std::string str)
        : m_hold(str), View(dataViewer) {}

    void Refresh() override { std::cout << "TestViewTwo refresh" << std::endl; }

  private:
    std::string m_hold;
  };

  viewController.CreateView<TestView>(viewer, 1, 2);
  viewController.CreateView<TestViewTwo>(viewer, "hi");
  viewController.RefreshViews();

  return 0;
}