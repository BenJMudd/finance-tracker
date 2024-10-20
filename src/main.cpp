#include "StateController.h"
#include <string>
#include <tuple>
#include <vector>

int main() {

  StateController controller;
  std::vector<std::string> catNames = controller.GetAllCategoryNames();
  DBFilter::SPtr showAllFilter = controller.CreateFilter();
  DBFilter::SPtr noIncomeFilter = controller.CreateFilter();
  uint8_t allTransactionsView =
      controller.GetViewController().CreateView<ListTransactions>(
          showAllFilter);
  uint8_t outgoingsView =
      controller.GetViewController().CreateView<ListTransactions>(
          noIncomeFilter);

  auto incomeIt = std::find(catNames.begin(), catNames.end(), "Income");
  assert(incomeIt != catNames.end());
  noIncomeFilter->OmitCategory(incomeIt - catNames.begin());

  uint8_t filterEditView =
      controller.GetViewController().CreateView<EditFilter>(showAllFilter,
                                                            noIncomeFilter);

  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();

  controller.GetViewController()
      .GetView<EditFilter>(filterEditView)
      .EnterStartDate(25);

  controller.GetViewController().RenderViews();
  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();
  return 0;
}