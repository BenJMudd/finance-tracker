#include "StateController.h"
#include <string>
#include <tuple>
#include <vector>

int main() {

  StateController controller;
  auto filter = controller.CreateFilter();
  controller.GetViewController().CreateView<ListTransactions>(filter);
  controller.GetViewController().CreateView<MostExpensivePurchase>(filter,
                                                                   false);
  controller.GetViewController().RenderViews();
  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();
  filter->OmitCategory(0);
  controller.GetViewController().RenderViews();
  controller.GetViewController().RefreshViews();
  controller.GetViewController().RenderViews();

  return 0;
}