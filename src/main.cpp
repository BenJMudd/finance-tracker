#include "StateController.h"
#include <string>
#include <tuple>
#include <vector>

int main() {

  StateController controller;
  auto filter = controller.CreateFilter();
  uint8_t listTransactionsId =
      controller.GetViewController().CreateView<ListTransactions>(filter);
  uint8_t mostExoId =
      controller.GetViewController().CreateView<MostExpensivePurchase>(filter,
                                                                       true);
  uint8_t editFilterId =
      controller.GetViewController().CreateView<EditFilter>(filter);
  controller.GetViewController().RenderViews();
  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();
  controller.GetViewController()
      .GetView<EditFilter>(editFilterId)
      .EnterSubCatToOverride(1);
  controller.GetViewController().RenderViews();
  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();

  return 0;
}