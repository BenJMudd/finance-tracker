#pragma once
#include "DBHandler.h"
#include <optional>
#include <set>
#include <string>
#include <vector>

class DBHandler;

class DBFilter {
public:
  using SPtr = std::shared_ptr<DBFilter>;

  DBFilter(DBHandler &db);
  ~DBFilter() { free(m_subcategoryState); }
  void SetStartDate(uint32_t date);
  void SetEndDate(uint32_t date);

  uint32_t GetStartDate() const { return m_startDate; }
  uint32_t GetEndDate() const { return m_endDate; }

  void SetCategoryState(size_t subCatId, bool state);
  void OmitAllCategories();
  void SetAllCategories();

  void ClearFilter();

  const std::vector<TransactionEntry> &GetTransactions(bool &isCacheValid);
  void BuildCache();
  void InvalidateCache() { m_cacheValid = false; }
  bool IsCacheValid() { return m_cacheValid; }
  bool IsCategoryActive(uint8_t id) {
    return m_subcategoryState[id / 8] & (1 << (id % 8));
  }
  std::string GetCategoryName(size_t id);
  const std::vector<std::string> &GetCategoryNames() {
    return m_db.GetCategoryNames();
  }
  const std::map<size_t, std::set<size_t>> &GetCategoryMapping() {
    return m_db.GetCategoryMapping();
  }

private:
  DBHandler &m_db;

  // cached results
  std::vector<TransactionEntry> m_transactions;
  bool m_cacheValid;

  // Filters
  uint32_t m_startDate;
  uint32_t m_endDate;
  uint8_t *m_subcategoryState;
};
