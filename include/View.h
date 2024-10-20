#pragma once

#include "DBFilter.h"
#include <vector>

class View {
public:
  View(std::shared_ptr<DBFilter> dataViewer) : m_dataViewer(dataViewer) {}
  virtual void Refresh() = 0;
  virtual void Render() = 0;
  DBFilter &GetFilter() { return *m_dataViewer.get(); }

protected:
  std::shared_ptr<DBFilter> m_dataViewer;
};

// test views
class MostExpensivePurchase : public View {
public:
  MostExpensivePurchase(std::shared_ptr<DBFilter> dataView, bool verbose)
      : m_verbose(verbose), View(dataView) {}

  void Refresh() override;
  void Render() override;

private:
  bool m_verbose;
};

class ListTransactions : public View {
public:
  ListTransactions(std::shared_ptr<DBFilter> &dataView) : View(dataView) {}

  void Refresh() override;
  void Render() override;
};

class EditFilter : public View {
public:
  EditFilter(std::shared_ptr<DBFilter> &dataView) : View(dataView) {}

  void Refresh() override;
  void Render() override;
  void EnterSubCatToOverride(uint8_t subcatId);
};
