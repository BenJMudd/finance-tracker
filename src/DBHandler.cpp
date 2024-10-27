#include "DBHandler.h"

DBHandler::DBHandler(std::string_view fileName) : m_db(fileName) {
  BuildCategoryData();
}

void DBHandler::BuildCategoryData() {
  m_categoryMapping.clear();
  m_categoryNames.clear();
  auto categories = ExecuteQuery<CategoryNameEntry>("select * from categories");
  for (auto &[category, subcategory] : categories) {
    auto catIt =
        std::find(m_categoryNames.begin(), m_categoryNames.end(), category);
    size_t catId = 0;
    if (catIt == m_categoryNames.end()) {
      catId = m_categoryNames.size();
      m_categoryNames.push_back(category);
    } else {
      catId = catIt - m_categoryNames.begin();
    }

    size_t subcatId = m_categoryNames.size();
    m_categoryNames.push_back(subcategory);
    m_categoryMapping[catId].insert(subcatId);
  }
}

CategoryEntry::CategoryEntry(SQLite::Statement &res,
                             const std::vector<std::string> &catNames) {
  std::string cat = res.getColumn(0).getString();
  std::string subcat = res.getColumn(1).getString();
  auto catIdIt = std::find(catNames.begin(), catNames.end(), cat);
  auto subcatIdIt = std::find(catNames.begin(), catNames.end(), subcat);
  assert(catIdIt != catNames.end());
  assert(subcatIdIt != catNames.end());
  m_cat = catIdIt - catNames.begin();
  m_subCat = subcatIdIt - catNames.begin();
}

TransactionEntry::TransactionEntry(SQLite::Statement &res,
                                   const std::vector<std::string> &catNames)
    : m_date(res.getColumn(0).getInt() + 3601), m_desc(res.getColumn(1).getString()),
      m_value(res.getColumn(2).getDouble()) {
  std::string subcat = res.getColumn(3).getString();
  auto subcatIdIt = std::find(catNames.begin(), catNames.end(), subcat);
  assert(subcatIdIt != catNames.end());
  m_subCat = subcatIdIt - catNames.begin();
}

SingleValueEntry::SingleValueEntry(SQLite::Statement &res,
                                   const std::vector<std::string> &catNames)
    : m_val(res.getColumn(0).getInt()) {}
