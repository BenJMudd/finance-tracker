#pragma once
#include "SQLiteCpp/SQLiteCpp.h"
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct CategoryEntry {
  CategoryEntry(SQLite::Statement &res,
                const std::vector<std::string> &catNames);

  size_t m_cat;
  size_t m_subCat;

  // Structured binding implementation
  template <std::size_t Index>
  std::tuple_element_t<Index, CategoryEntry> &get();
};

template <std::size_t Index>
inline std::tuple_element_t<Index, CategoryEntry> &CategoryEntry::get() {
  if constexpr (Index == 0)
    return m_cat;
  if constexpr (Index == 1)
    return m_subCat;
}

struct TransactionEntry {
  TransactionEntry() {}
  TransactionEntry(SQLite::Statement &res,
                   const std::vector<std::string> &catNames);

  uint32_t m_date; // UNIX
  std::string m_desc;
  double m_value;
  size_t m_subCat;

  // Structured binding implementation
  template <std::size_t Index>
  std::tuple_element_t<Index, TransactionEntry> &get();
};

// TODO: tempalte for different values
struct SingleValueEntry {
  SingleValueEntry() {}
  SingleValueEntry(SQLite::Statement &res,
                   const std::vector<std::string> &catNames);

  uint32_t m_val;
};

template <std::size_t Index>
inline std::tuple_element_t<Index, TransactionEntry> &TransactionEntry::get() {
  if constexpr (Index == 0)
    return m_date;
  if constexpr (Index == 1)
    return m_desc;
  if constexpr (Index == 2)
    return m_value;
  if constexpr (Index == 3)
    return m_subCat;
}

class DBHandler {
public:
  DBHandler(std::string_view fileName);
  // TODO: add template constraints
  template <typename T> std::vector<T> ExecuteQuery(std::string_view query);
  const std::map<size_t, std::set<size_t>> &GetCategoryMapping() {
    return m_categoryMapping;
  }
  const std::vector<std::string> &GetCategoryNames() { return m_categoryNames; }

private:
  struct CategoryNameEntry {
    CategoryNameEntry(SQLite::Statement &res,
                      const std::vector<std::string> &catNames)
        : m_cat(res.getColumn(0).getString()),
          m_subCat(res.getColumn(1).getString()){};

    std::string m_cat;
    std::string m_subCat;

    // Structured binding implementation
    template <std::size_t Index>
    std::tuple_element_t<Index, CategoryEntry> &get() {
      if constexpr (Index == 0)
        return m_cat;
      if constexpr (Index == 1)
        return m_subCat;
    }
  };

  void BuildCategoryData();

  SQLite::Database m_db;

  // Probably doesn't belong in this class
  std::map<size_t, std::set<size_t>> m_categoryMapping;
  std::vector<std::string> m_categoryNames; // Index is the category id
};

template <typename T>
inline std::vector<T> DBHandler::ExecuteQuery(std::string_view query) {
  std::vector<T> data;
  try {
    const char *queryRaw = query.data(); // Will not compile without this wtf???
    SQLite::Statement query(m_db, queryRaw);

    while (query.executeStep()) {
      data.emplace_back(query, m_categoryNames);
    }
  } catch (std::exception &e) {
    std::cout << "exception: " << e.what() << std::endl;
  }

  return data;
}
