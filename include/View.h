#pragma once

#include "DBFilter.h"
#include <vector>

class View {
public:
  View(std::shared_ptr<DBFilter> dataViewer = nullptr)
      : m_dataViewer(dataViewer) {}
  virtual void Refresh() = 0;
  virtual void Render() = 0;
  DBFilter *GetFilter() { return m_dataViewer.get(); }

protected:
  std::shared_ptr<DBFilter> m_dataViewer;
};

// test views
class MostExpensivePurchase : public View {
public:
  MostExpensivePurchase(std::shared_ptr<DBFilter> filter, bool verbose)
      : m_verbose(verbose), View(filter) {}

  void Refresh() override;
  void Render() override;

private:
  bool m_verbose;
};

class ListTransactions : public View {
public:
  ListTransactions(std::shared_ptr<DBFilter> &filter) : View(filter) {}

  void Refresh() override;
  void Render() override;
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

private:
  std::vector<DBFilter::SPtr> m_filters;
};
