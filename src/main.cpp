#include "DBFilter.h"
#include "DBHandler.h"
#include "DataViewer.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "sqlite3.h"
#include <string>
#include <tuple>
#include <vector>
int main() {
  DBHandler db("res/finances.db");
  DBFilter filter(db);
  filter.SetStartDate(25);
  filter.OmitSubCategory(2);
  DataViewer viewer(filter);
  auto &transactions = viewer.GetTransactions();

  return 0;
}