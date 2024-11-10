#include "DataTransformer.h"
#include "DBFilter.h"
#include <span>

void AggregateTransformer::BuildCache() {
  // TODO: this runs twice if the filter has a valid cache and you request an
  // interval data
  m_cache.m_data.clear();
  m_cache.m_total = 0.0f;
  m_intervalCache.clear();

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

  if (transactions.empty())
    return;

  if (m_buildByIntervalCounter == 0) {
    m_cache = BuildAggregateByRange(transactions, subcatToCatMap, 0,
                                    transactions.size());
    return;
  }

  m_cache.m_startDate = transactions[0].m_date;

  size_t lastEndIdx = 0;
  for (size_t i = 0; i < transactions.size(); ++i) {
    if (transactions[lastEndIdx].m_date + 604800 >=
        transactions[i].m_date) // every week
      continue;

    m_intervalCache.emplace_back(BuildAggregateByRange(
        transactions, subcatToCatMap, lastEndIdx, i + 1, &m_cache));
    lastEndIdx = i;
  }

  if (lastEndIdx != transactions.size()) {
    m_intervalCache.emplace_back(
        BuildAggregateByRange(transactions, subcatToCatMap, lastEndIdx,
                              transactions.size(), &m_cache));
  }
}

void AggregateTransformer::SetBuildByInterval(bool toSet) {
  m_buildByIntervalCounter += toSet ? 1 : -1;
  if (toSet && m_buildByIntervalCounter == 1 && m_filter.IsCacheValid()) {
    // rebuild cache if a window requires an interval cache
    BuildCache();
  }
}

AggregateTransformer::Aggregate AggregateTransformer::BuildAggregateByRange(
    const std::vector<TransactionEntry> &transactions,
    std::unordered_map<size_t, size_t> &subcatToCatMap, size_t startIdx,
    size_t endIdx, Aggregate *totalAggregate) {

  const std::span<const TransactionEntry> transactionSpan(
      transactions.cbegin() + startIdx, transactions.cbegin() + endIdx);

  auto AppendToAggregate = [](Aggregate &aggregate,
                              const TransactionEntry &transaction,
                              size_t transactionCategory) {
    auto catPairIt = aggregate.m_data.find(transactionCategory);
    if (catPairIt == aggregate.m_data.end()) {
      aggregate.m_data.insert({transactionCategory, {0.0f, {}}});
      catPairIt = aggregate.m_data.find(transactionCategory);
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
  };

  auto SetTotal = [](Aggregate &target, Aggregate &dataBase) {
    for (const auto &[catId, catData] : dataBase.m_data) {
      target.m_total += catData.first;
    }
  };

  Aggregate aggregate;
  aggregate.m_startDate = transactionSpan[0].m_date;
  for (const TransactionEntry &transaction : transactionSpan) {
    size_t transactionCategory = subcatToCatMap[transaction.m_subCat];
    AppendToAggregate(aggregate, transaction, transactionCategory);
    if (totalAggregate)
      AppendToAggregate(*totalAggregate, transaction, transactionCategory);
  }

  SetTotal(aggregate, aggregate);
  if (totalAggregate)
    SetTotal(*totalAggregate, aggregate);

  return aggregate;
}
