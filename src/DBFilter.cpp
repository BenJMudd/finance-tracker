#include "DBFilter.h"
#include <time.h>

using Time = std::chrono::system_clock;

DBFilter::DBFilter(DBHandler &db)
    : m_db(db), m_cacheValid(false), m_startDate(0),
      m_endDate(static_cast<uint32_t>(Time::to_time_t(Time::now()))) {
  // TODO: use c++ mem management
  m_subcategoryState =
      (uint8_t *)malloc((m_db.GetCategoryNames().size() / 8) + 1);
  memset(m_subcategoryState, 0xff, (m_db.GetCategoryNames().size() / 8) + 1);

  SingleValueEntry firstTrans = m_db.ExecuteQuery<SingleValueEntry>(
      "select MIN(one) from transactions;")[0];
  m_startDate = firstTrans.m_val;
}

void DBFilter::SetStartDate(uint32_t date) {
  m_startDate = date;
  m_cacheValid = false;
}

void DBFilter::SetEndDate(uint32_t date) {
  m_endDate = date;
  m_cacheValid = false;
}

void DBFilter::SetCategoryState(size_t subCatId, bool state) {
  if (state) {
    m_subcategoryState[subCatId / 8] |= (1 << subCatId % 8);
  } else {
    m_subcategoryState[subCatId / 8] &= ~(1 << subCatId % 8);
  }

  m_cacheValid = false;
}

void DBFilter::OmitAllCategories() {
  memset(m_subcategoryState, 0x0, (m_db.GetCategoryNames().size() / 8) + 1);
  m_cacheValid = false;
}

void DBFilter::SetAllCategories() {
  memset(m_subcategoryState, 0xff, (m_db.GetCategoryNames().size() / 8) + 1);
  m_cacheValid = false;
}

void DBFilter::ClearFilter() {
  memset(m_subcategoryState, 0xff, (m_db.GetCategoryNames().size() / 8) + 1);

  TransactionEntry firstTrans = m_db.ExecuteQuery<TransactionEntry>(
      "select MIN(one) from transactions")[0];
  m_startDate = firstTrans.m_date;
  m_endDate = static_cast<uint32_t>(Time::to_time_t(Time::now()));

  m_cacheValid = false;
}

std::string DBFilter::GetCategoryName(size_t id) {
  return m_db.GetCategoryNames()[id];
}

void DBFilter::NotifyViewDropped(uint8_t viewId) {
  // remove unused filters
  std::set<DataTransformer *> viewHeldTransformers = m_viewTransMap[viewId];
  for (DataTransformer *transformer : viewHeldTransformers) {
    DropTransformer(viewId, transformer);
  }
}

void DBFilter::DropTransformer(uint8_t viewId, DataTransformer *trans) {
  std::set<DataTransformer *> &viewHeldTransformers = m_viewTransMap[viewId];
  auto heldTransIt = std::find(viewHeldTransformers.begin(),
                               viewHeldTransformers.end(), trans);
  assert(heldTransIt != viewHeldTransformers.end());
  viewHeldTransformers.erase(heldTransIt);

  if (viewHeldTransformers.empty())
    m_viewTransMap.erase(viewId);

  std::set<uint8_t> &transViews = m_transViewMap[trans];
  auto heldTransViewIt =
      std::find(transViews.begin(), transViews.end(), viewId);
  assert(heldTransViewIt != transViews.end());
  transViews.erase(heldTransViewIt);

  if (transViews.empty()) {
    m_transViewMap.erase(trans);

    // no one is holding the transformer, should be destroyed
    auto transToRemoveIt =
        std::find_if(m_transformers.begin(), m_transformers.end(),
                     [&](const std::unique_ptr<DataTransformer> &transformer) {
                       return transformer.get() == trans;
                     });
    assert(transToRemoveIt != m_transformers.end());
    m_transformers.erase(transToRemoveIt);
  }
}

const std::vector<TransactionEntry> &
DBFilter::GetTransactions(bool *isCacheValid) {
  if (isCacheValid)
    *isCacheValid = m_cacheValid;

  return m_transactions;
}

void DBFilter::BuildCache() {
  if (m_cacheValid)
    return;

  auto &catNames = m_db.GetCategoryNames();
  auto &catMappings = m_db.GetCategoryMapping();
  std::set<size_t> omittedCategories;

  for (uint8_t i = 0; i < catNames.size(); ++i) {
    if (!IsCategoryActive(i))
      omittedCategories.insert(i);
  }

  std::string catFilter;
  for (const size_t categoryToOmit : omittedCategories) {
    assert(categoryToOmit < catNames.size());
    std::string catName = catNames[categoryToOmit];
    catFilter.append(std::format("four <> \'{}\' and ", catName));
  }

  std::string query =
      std::format("select * from transactions where {} one > {} and one < {}",
                  catFilter, m_startDate, m_endDate);
  m_transactions = m_db.ExecuteQuery<TransactionEntry>(query);

  // update cache for data transformers
  for (auto &transformer : m_transformers) {
    transformer->BuildCache();
  }

  m_cacheValid = true;
}
