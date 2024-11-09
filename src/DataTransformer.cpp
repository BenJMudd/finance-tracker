#include "DataTransformer.h"
#include "DBFilter.h"

void AggregateTransformer::BuildCache() {
  m_cache.clear();

  auto &catMappings = m_filter.GetCategoryMapping();
  // build a map of subcat to cat
  std::unordered_map<size_t, size_t> subcatToCatMap;
  for (const auto &[cat, subCatSet] : catMappings) {
    std::vector<std::pair<size_t, double>> subCatValues;
    for (const auto subcat : subCatSet) {
      subcatToCatMap[subcat] = cat;
    }
  }

  const std::vector<TransactionEntry> &transactions =
      m_filter.GetTransactions();

  for (const TransactionEntry &transaction : transactions) {
    size_t transactionCategory = subcatToCatMap[transaction.m_subCat];
    auto catPairIt = m_cache.find(transactionCategory);
    if (catPairIt == m_cache.end()) {
      m_cache.insert({transactionCategory, {0.0f, {}}});
      catPairIt = m_cache.find(transactionCategory);
    }

    catPairIt->second.first += transaction.m_value;
    bool found = false;
    for (auto &[subCat, aggregate] : catPairIt->second.second) {
      if (subCat != transaction.m_subCat)
        continue;

      found = true;
      aggregate += transaction.m_value;
    }

    if (!found) {
      catPairIt->second.second.emplace_back(transaction.m_subCat,
                                            transaction.m_value);
    }
  }
}
