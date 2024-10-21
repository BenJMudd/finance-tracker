#pragma once

#include "DBFilter.h"
#include <vector>

class View {
public:
  View(DBFilter::SPtr dataViewer = nullptr) : m_dataViewer(dataViewer) {}
  virtual void Refresh() = 0;
  virtual void Render() = 0;
  virtual bool GetCacheValidity() {
    if (m_dataViewer) {
      return m_dataViewer->IsCacheValid();
    }

    return false;
  }

  virtual DBFilter &GetFilter() { return *m_dataViewer.get(); }
  virtual void SetFilter(DBFilter::SPtr filter) { m_dataViewer = filter; }

protected:
  DBFilter::SPtr m_dataViewer;
};

// test views
class MostExpensivePurchase : public View {
public:
  MostExpensivePurchase(DBFilter::SPtr filter, bool verbose)
      : m_verbose(verbose), View(filter) {}

  void Refresh() override;
  void Render() override;

private:
  bool m_verbose;
};

class ListTransactions : public View {
public:
  ListTransactions(DBFilter::SPtr filter) : View(filter) {}

  void Refresh() override;
  void Render() override;
};

class ListTransactionsWindow : public View {
public:
  ListTransactionsWindow(DBFilter::SPtr filter) : View(filter) {}

  void Refresh() override;
  void Render() override;
};

class ComparisonView : public View {
public:
  ComparisonView(DBFilter &lhs, DBFilter &rhs)
      : m_lhs(lhs), m_rhs(rhs), View() {}

  void Refresh() override;
  void Render() override;

private:
  DBFilter &m_lhs;
  DBFilter &m_rhs;
};

class EditFilter : public View {
public:
  // Not sure how much i like passing a nullptr to View
  template <typename... Args> EditFilter(Args &&...filters) : View() {
    m_filters = {{filters...}};
  }

  void Refresh() override;
  void Render() override;
  void EnterSubCatToOmit(uint8_t subcatId);
  void EnterCatToOmit(uint8_t catId);
  void EnterStartDate(uint32_t startDate);
  void EnterEndDate(uint32_t endDate);

private:
  std::vector<DBFilter::SPtr> m_filters;
};
