#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class DBFilter;
struct TransactionEntry;
namespace DataTransformerFlags {}

class DataTransformer {
public:
  DataTransformer() {}
  virtual ~DataTransformer() {
    std::cout << "Data Transformer deleted" << std::endl;
  }

  virtual void BuildCache() = 0;
};

class SingleFilterDataTransformer : public DataTransformer {
public:
  SingleFilterDataTransformer(DBFilter &filter) : m_filter(filter) {}

protected:
  DBFilter &m_filter;
};

class AggregateTransformer : public SingleFilterDataTransformer {
public:
  AggregateTransformer(DBFilter &filter)
      : m_buildByIntervalCounter(0), SingleFilterDataTransformer(filter) {}

  // map of category names to a pair, first is a aggregate cost, second is a
  // vector of pairs, first is subcat, second is aggregate cost
  using AggregateData =
      std::map<size_t,
               std::pair<double, std::vector<std::pair<size_t, double>>>>;

  struct Aggregate {
    uint32_t m_startDate;
    double m_total = 0.0f;
    AggregateData m_data;
  };

  void BuildCache() override;

  const Aggregate &GetAggregate() { return m_cache; }

  void SetBuildByInterval(bool toSet);

  const std::vector<Aggregate> *GetAggregateByInterval() {
    if (m_buildByIntervalCounter == 0)
      return nullptr;

    return &m_intervalCache;
  }

private:
  Aggregate
  BuildAggregateByRange(const std::vector<TransactionEntry> &transactions,
                        std::unordered_map<size_t, size_t> &subcatToCatMap,
                        size_t startIdx, size_t endIdx,
                        Aggregate *totalAggregate = nullptr);

  size_t m_buildByIntervalCounter;
  std::vector<Aggregate> m_intervalCache;
  Aggregate m_cache;
};
