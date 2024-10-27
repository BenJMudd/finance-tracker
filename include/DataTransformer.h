#pragma once
#include <string>

class DBFilter;

class DataTransformer {
public:
  DataTransformer() {}
  virtual ~DataTransformer() {}
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

  std::string GetAggregate();
};
