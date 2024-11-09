#pragma once
#include "DBHandler.h"
#include "DataTransformer.h"
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

class DBHandler;

class DBFilter {
public:
  using SPtr = std::shared_ptr<DBFilter>;

  DBFilter(DBHandler &db);
  ~DBFilter() {
    free(m_subcategoryState);
    std::cout << "DBFilter deleted" << std::endl;
  }
  void SetStartDate(uint32_t date);
  void SetEndDate(uint32_t date);

  uint32_t GetStartDate() const { return m_startDate; }
  uint32_t GetEndDate() const { return m_endDate; }

  void SetCategoryState(size_t subCatId, bool state);
  void OmitAllCategories();
  void SetAllCategories();

  void ClearFilter();

  const std::vector<TransactionEntry> &
  GetTransactions(bool *isCacheValid = nullptr);
  void BuildCache();
  void InvalidateCache() { m_cacheValid = false; }
  bool IsCacheValid() const { return m_cacheValid; }
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

  // TODO: Multi filter transformers
  template <typename T> T &GetDataTansformer(uint8_t viewId);
  // TODO: implement
  void NotifyViewDropped(uint8_t viewId);

private:
  void DropTransformer(uint8_t viewId, DataTransformer *trans);
  DBHandler &m_db;

  // cached results
  std::vector<TransactionEntry> m_transactions;
  bool m_cacheValid;

  // Filters
  uint32_t m_startDate;
  uint32_t m_endDate;
  uint8_t *m_subcategoryState;

  // Data transformers
  // view id -> data filters they are using
  std::unordered_map<uint8_t, std::set<DataTransformer *>> m_viewTransMap;
  std::unordered_map<DataTransformer *, std::set<uint8_t>> m_transViewMap;
  // TODO: this will be shared ptr for multi filter transformers
  std::vector<std::unique_ptr<DataTransformer>> m_transformers;
};

template <typename T> inline T &DBFilter::GetDataTansformer(uint8_t viewId) {
  auto viewDepsIt = m_viewTransMap.find(viewId);
  if (viewDepsIt != m_viewTransMap.end()) {
    for (auto *transformer : viewDepsIt->second) {
      if (auto *toFind = dynamic_cast<T *>(transformer)) {
        return *toFind;
      }
    }

    // Should never get here
    assert(false);
  }

  // Find existing transformer
  for (auto &transformer : m_transformers) {
    if (auto *toFind = dynamic_cast<T *>(transformer.get())) {
      m_viewTransMap[viewId].insert(toFind);
      m_transViewMap[toFind].insert(viewId);
      return *toFind;
    }
  }

  std::unique_ptr<T> transformer = std::make_unique<T>(*this);
  T *pTransformer = transformer.get();
  m_transformers.push_back(std::move(transformer));

  m_viewTransMap[viewId].insert(pTransformer);
  m_transViewMap[pTransformer].insert(viewId);
  if (m_cacheValid)
    pTransformer->BuildCache();

  return *pTransformer;
}
