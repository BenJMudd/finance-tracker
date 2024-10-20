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
  void SetStartDate(uint32_t date) {
    m_startDate = date;
    m_cacheValid = false;
  }
  void SetEndDate(uint32_t date) {
    m_endDate = date;
    m_cacheValid = false;
  }
  void OmitCategory(size_t catId) {
    m_omitCategories.insert(catId);
    m_cacheValid = false;
  }
  void OmitSubCategory(size_t subCatId) {
    m_omitSubcategories.insert(subCatId);
    m_cacheValid = false;
  }

  const std::vector<TransactionEntry> &GetTransactions(bool &isCacheValid);
  void BuildCache();
  void InvalidateCache() { m_cacheValid = false; }

  std::string GetCategoryName(size_t id);

private:
  DBHandler &m_db;

  // cached results
  std::vector<TransactionEntry> m_transactions;
  bool m_cacheValid;

  // Filters
  std::optional<uint32_t> m_startDate;
  std::optional<uint32_t> m_endDate;
  std::set<size_t> m_omitCategories;
  std::set<size_t> m_omitSubcategories;
};
