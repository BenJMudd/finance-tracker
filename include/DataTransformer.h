#pragma once
#include <map>
#include <string>
#include <vector>

class DBFilter;

class DataTransformer {
public:
  DataTransformer() {}
  virtual ~DataTransformer() {}
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
      : SingleFilterDataTransformer(filter) {}

  // map of category names to a pair, first is a aggregate cost, second is a
  // vector of pairs, first is subcat, second is aggregate cost
  using AggregateData =
      std::map<size_t,
               std::pair<double, std::vector<std::pair<size_t, double>>>>;

  void BuildCache() override;

  AggregateData &GetAggregate() { return m_cache; }

private:
  AggregateData m_cache;
};
