#ifndef SNACKIS_TABLE_COLUMN_HPP
#define SNACKIS_TABLE_COLUMN_HPP

#include <string>

namespace snackis {
  using namespace std;
  
  template <typename RecT>
  struct TableColumn {
    const string name;
    TableColumn(const string &name);
  };

  template <typename RecT>
  TableColumn<RecT>::TableColumn(const string &name): name(name) {
  }
}

#endif
