#pragma once
#include "DBFilter.h"
#include "DBHandler.h"
#include <vector>

class DataViewer {
public:
  DataViewer(DBFilter &filter) : m_db(filter) {}
  const std::vector<TransactionEntry> &GetTransactions() {
    return m_db.GetTransactions();
  };

private:
  DBFilter &m_db;
};
