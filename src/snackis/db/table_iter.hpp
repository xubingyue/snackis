#ifndef SNACKIS_DB_TABLE_ITER_HPP
#define SNACKIS_DB_TABLE_ITER_HPP

#include <set>

#include "snackis/db/rec.hpp"

namespace snackis {  
namespace db {
  template <typename RecT>
  struct TableIter: std::iterator<std::forward_iterator_tag, db::Rec<RecT>> {
    using Imp = typename std::set<Rec<RecT>>::const_iterator; 

    Imp imp;

    TableIter(const Imp &imp);
    TableIter& operator++();
    const db::Rec<RecT>& operator*() const;
  };

  template <typename RecT>
  TableIter<RecT>::TableIter(const Imp &imp):
    imp(imp)
  {}

  template <typename RecT>
  TableIter<RecT>& TableIter<RecT>::operator++() {
    ++imp;
    return *this;
  }
    
  template <typename RecT>
  const db::Rec<RecT>& TableIter<RecT>::operator*() const {
    return *imp;
  }
  
  template <typename RecT>
  bool operator !=(const TableIter<RecT> &x, const TableIter<RecT> &y) {
    return x.imp != y.imp;
  }
}}

#endif
