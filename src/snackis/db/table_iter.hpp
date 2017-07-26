#ifndef SNACKIS_DB_TABLE_ITER_HPP
#define SNACKIS_DB_TABLE_ITER_HPP

#include <set>

#include "snackis/db/rec.hpp"

namespace snackis {  
namespace db {
  template <typename RecT, typename...KeyT>
  struct TableIter: std::iterator<std::forward_iterator_tag, db::Rec<RecT>> {
    using Imp = typename std::map<typename Key<RecT, KeyT...>::Type,
				  Rec<RecT>>::const_iterator; 

    Imp imp;

    TableIter(const Imp &imp);
    TableIter& operator++();
    const db::Rec<RecT>& operator*() const;
  };

  template <typename RecT, typename...KeyT>
  TableIter<RecT, KeyT...>::TableIter(const Imp &imp):
    imp(imp)
  {}

  template <typename RecT, typename...KeyT>
  TableIter<RecT, KeyT...>& TableIter<RecT, KeyT...>::operator++() {
    ++imp;
    return *this;
  }
    
  template <typename RecT, typename...KeyT>
  const db::Rec<RecT>& TableIter<RecT, KeyT...>::operator*() const {
    return imp->second;
  }
  
  template <typename RecT, typename...KeyT>
  bool operator !=(const TableIter<RecT, KeyT...> &x,
		   const TableIter<RecT, KeyT...> &y) {
    return x.imp != y.imp;
  }
}}

#endif
