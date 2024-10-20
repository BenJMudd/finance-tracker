#include "View.h"

void MostExpensivePurchase::Render() {
  std::cout << "######### Most expensive Begin #########" << std::endl;
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(isCacheValid);
  std::cout << "TestViewOne render: cache is "
            << (isCacheValid ? "valid" : "invalid") << std::endl;

  TransactionEntry mostExpensive;
  mostExpensive.m_value = 0;
  for (auto &transaction : transactions) {
    if (transaction.m_value > mostExpensive.m_value) {
      mostExpensive = transaction;
    }
  }

  std::cout << "Most expensive transaction is " << mostExpensive.m_value;
  if (m_verbose) {
    std::cout << "{" << mostExpensive.m_date << "," << mostExpensive.m_desc
              << "," << mostExpensive.m_subCat << "}";
  }

  std::cout << std::endl;
  std::cout << "######### Most expensive end #########\n" << std::endl;
}

void MostExpensivePurchase::Refresh() {
  std::cout << "######### Most expensive refresh #########\n" << std::endl;
}

void ListTransactions::Render() {
  std::cout << "######### Transactions begin #########" << std::endl;
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(isCacheValid);
  std::cout << "TestViewTwo render: cache is "
            << (isCacheValid ? "valid" : "invalid") << std::endl;

  for (auto &transaction : transactions) {
    std::cout << "Date: " << transaction.m_date << ", " << transaction.m_desc
              << ", " << m_dataViewer->GetCategoryName(transaction.m_subCat)
              << ", " << transaction.m_value << std::endl;
  }
  std::cout << "######### Transactions end #########\n" << std::endl;
}

void ListTransactions::Refresh() {
  std::cout << "######### Transactions refresh #########\n" << std::endl;
}

void EditFilter::Refresh() {}

void EditFilter::Render() {}

void EditFilter::EnterSubCatToOverride(uint8_t subcatId) {
  m_dataViewer->OmitSubCategory(subcatId);
}
