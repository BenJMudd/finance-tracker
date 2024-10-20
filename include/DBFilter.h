#pragma once
#include "DBHandler.h"
#include <optional>
#include <set>
#include <string>
#include <vector>

class DBHandler;

class DBFilter {
public:
  DBFilter(DBHandler &db);
  void SetStartDate(uint32_t date) { m_startDate = date; }
  void SetEndDate(uint32_t date) { m_endDate = date; }
  void OmitCategory(size_t catId) { m_omitCategories.insert(catId); }
  void OmitSubCategory(size_t subCatId) {
    m_omitSubcategories.insert(subCatId);
  }

  const std::vector<TransactionEntry> &GetTransactions();

private:
  DBHandler &m_db;

  // cached results
  std::vector<TransactionEntry> m_transactions;

  // Filters
  std::optional<uint32_t> m_startDate;
  std::optional<uint32_t> m_endDate;
  std::set<size_t> m_omitCategories;
  std::set<size_t> m_omitSubcategories;
};
