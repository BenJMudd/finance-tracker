#include "DBFilter.h"
#include "StateController.h"
#include "View.h"
#include <string>
#include <vector>

int main() {
  StateController controller;
  std::vector<std::string> catNames = controller.GetAllCategoryNames();
  DBFilter &septemberFilter = controller.GetViewController().CreateFilter();
  DBFilter &octoberFilter = controller.GetViewController().CreateFilter();
  septemberFilter.SetStartDate(1725214611);
  septemberFilter.SetEndDate(1727720211);
  octoberFilter.SetStartDate(1727723811);
  octoberFilter.SetEndDate(1730319411);

  uint8_t compView = controller.GetViewController().CreateView<ComparisonView>(
      septemberFilter, octoberFilter);

  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();
  return 0;
}
