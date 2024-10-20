#include "StateController.h"
#include <string>
#include <tuple>
#include <vector>

int main() {

  StateController controller;
  std::vector<std::string> catNames = controller.GetAllCategoryNames();
  DBFilter &showAllFilter = controller.GetViewController().CreateFilter();
  uint8_t allTransactionsView =
      controller.GetViewController().CreateView<ListTransactions>(
          showAllFilter);

  uint8_t mostExpensiveView =
      controller.GetViewController().CreateView<MostExpensivePurchase>(
          showAllFilter, true);

  uint8_t editFilterView =
      controller.GetViewController().CreateView<EditFilter>(&showAllFilter);

  controller.GetViewController()
      .GetView<EditFilter>(editFilterView)
      .EnterSubCatToOmit(19);

  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();
  return 0;
}