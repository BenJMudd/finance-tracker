#include "DBFilter.h"

DBFilter::DBFilter(DBHandler &db)
    : m_db(db), m_cacheValid(false), m_startDate(std::nullopt),
      m_endDate(std::nullopt) {}

std::string DBFilter::GetCategoryName(size_t id) {
  return m_db.GetCategoryNames()[id];
}

const std::vector<TransactionEntry> &
DBFilter::GetTransactions(bool &isCacheValid) {
  isCacheValid = m_cacheValid;
  return m_transactions;
}

void DBFilter::BuildCache() {
  if (m_cacheValid)
    return;

  auto &catNames = m_db.GetCategoryNames();
  auto &catMappings = m_db.GetCategoryMapping();
  std::set<size_t> omittedCategories;

  for (const size_t cat : m_omitCategories) {
    auto it = catMappings.find(cat);
    assert(it != catMappings.end());
    std::set<size_t> subcategories = it->second;
    for (const size_t subcat : subcategories) {
      omittedCategories.insert(subcat);
    }
  }

  for (const size_t subcat : m_omitSubcategories) {
    omittedCategories.insert(subcat);
  }

  std::string catFilter;
  for (const size_t categoryToOmit : omittedCategories) {
    assert(categoryToOmit < catNames.size());
    std::string catName = catNames[categoryToOmit];
    catFilter.append(std::format("four <> \'{}\' and ", catName));
  }

  std::string query =
      std::format("select * from transactions where {} one > {} and one < {}",
                  catFilter, m_startDate.value_or(0), m_endDate.value_or(1000));
  m_transactions = m_db.ExecuteQuery<TransactionEntry>(query);
  m_cacheValid = true;
}
