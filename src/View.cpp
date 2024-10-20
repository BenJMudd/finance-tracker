#include "View.h"
#include <Windows.h>
#include <chrono>

using namespace std::chrono;

// Function to set the console text and background color
void SetColor(int textColor, int bgColor) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

void MostExpensivePurchase::Render() {
  std::cout << "######### Most expensive Begin #########" << std::endl;
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(isCacheValid);
  if (isCacheValid)
    SetColor(FOREGROUND_GREEN, 0);
  else
    SetColor(FOREGROUND_RED, 0);

  TransactionEntry mostExpensive;
  mostExpensive.m_value = 0;
  for (auto &transaction : transactions) {
    if (transaction.m_value < mostExpensive.m_value) {
      mostExpensive = transaction;
    }
  }

  std::cout << "Most expensive transaction is " << mostExpensive.m_value;
  if (m_verbose) {
    std::cout << "(" << sys_seconds{seconds(mostExpensive.m_date)} << ","
              << mostExpensive.m_desc << ","
              << m_dataViewer->GetCategoryName(mostExpensive.m_subCat) << ")\n";
  }

  SetColor(7, 0);
  std::cout << "######### Most expensive end #########\n" << std::endl;
}

void MostExpensivePurchase::Refresh() {
  std::cout << "######### Most expensive refresh #########\n" << std::endl;
}

void ListTransactions::Render() {
  std::cout << "######### Transactions begin #########" << std::endl;
  bool isCacheValid = false;
  auto &transactions = m_dataViewer->GetTransactions(isCacheValid);
  if (isCacheValid)
    SetColor(FOREGROUND_GREEN, 0);
  else
    SetColor(FOREGROUND_RED, 0);
  for (auto &transaction : transactions) {
    std::cout << "Date: " << transaction.m_date << ", " << transaction.m_desc
              << ", " << m_dataViewer->GetCategoryName(transaction.m_subCat)
              << ", " << transaction.m_value << std::endl;
  }

  SetColor(7, 0);
  std::cout << "######### Transactions end #########\n" << std::endl;
}

void ListTransactions::Refresh() {
  std::cout << "######### Transactions refresh #########\n" << std::endl;
}

void EditFilter::Refresh() {}

void EditFilter::Render() {}

void EditFilter::EnterSubCatToOmit(uint8_t subcatId) {
  for (auto &filter : m_filters) {
    filter->OmitSubCategory(subcatId);
  }
}
void EditFilter::EnterCatToOmit(uint8_t catId) {
  for (auto &filter : m_filters) {
    filter->OmitSubCategory(catId);
  }
}

void EditFilter::EnterStartDate(uint32_t startDate) {
  for (auto &filter : m_filters) {
    filter->SetStartDate(startDate);
  }
}

void EditFilter::EnterEndDate(uint32_t endDate) {
  for (auto &filter : m_filters) {
    filter->SetEndDate(endDate);
  }
}
