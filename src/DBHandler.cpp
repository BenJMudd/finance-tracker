#include "DBHandler.h"

DBHandler::DBHandler(std::string_view fileName) : m_db(fileName) {
  BuildCategoryData();
}

void DBHandler::BuildCategoryData() {
  m_categoryMapping.clear();
  m_categoryNames.clear();
  auto categories = ExecuteQuery<CategoryEntryName>("select * from categories");
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
