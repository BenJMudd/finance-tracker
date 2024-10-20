#pragma once

#include "DBHandler.h"
#include "DataViewer.h"
#include <vector>

class View {
public:
  View(DataViewer &dataViewer) : m_dataViewer(dataViewer) {}
  virtual void Refresh() = 0;

private:
  DataViewer &m_dataViewer;
};
