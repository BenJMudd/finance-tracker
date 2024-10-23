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
  void SetStartDate(uint32_t date) {
    m_startDate = date;
    m_cacheValid = false;
  }
  void SetEndDate(uint32_t date) {
    m_endDate = date;
    m_cacheValid = false;
  }

  void SetCategoryState(size_t subCatId, bool state) {
    if (state) {
      m_subcategoryState[subCatId / 8] |= (1 << subCatId % 8);
    } else {
      m_subcategoryState[subCatId / 8] &= ~(1 << subCatId % 8);
    }

    m_cacheValid = false;
  }

  void OmitAllCategories() {
    memset(m_subcategoryState, 0x0, (m_db.GetCategoryNames().size() / 8) + 1);
    m_cacheValid = false;
  }

  void SetAllCategories() {
    memset(m_subcategoryState, 0xff, (m_db.GetCategoryNames().size() / 8) + 1);
    m_cacheValid = false;
  }

  void ClearFilter() {
    memset(m_subcategoryState, 0xff, (m_db.GetCategoryNames().size() / 8) + 1);
    m_startDate = std::nullopt;
    m_endDate = std::nullopt;
    m_cacheValid = false;
  }

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
  std::optional<uint32_t> m_startDate;
  std::optional<uint32_t> m_endDate;
  uint8_t *m_subcategoryState;
};
