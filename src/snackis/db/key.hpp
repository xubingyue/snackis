#ifndef SNACKIS_DB_KEY_HPP
#define SNACKIS_DB_KEY_HPP

#include <tuple>
#include "snackis/db/rec.hpp"

namespace snackis {
namespace db {
  template <typename RecT, typename...ValT>
  struct Key: std::tuple<const Col<RecT, ValT> *...> {
    using Type = std::tuple<ValT...>;

    Key(const Col<RecT, ValT> &...cols);
    Type operator ()(const ValT &...vals) const;
    Type operator ()(const db::Rec<RecT> &rec) const;
    Type operator ()(const RecT &rec) const;
  };

  template <typename RecT, typename...ValT>
  Key<RecT, ValT...>::Key(const Col<RecT, ValT> &...cols):
    std::tuple<const Col<RecT, ValT> *...>(&cols...)
  { }

  template <typename RecT, typename...ValT>
  typename Key<RecT, ValT...>::Type
  Key<RecT, ValT...>::operator ()(const ValT &...vals) const {
    return std::make_tuple(vals...);
  }

  template <typename RecT, typename...ValT>
  typename Key<RecT, ValT...>::Type
  Key<RecT, ValT...>::operator ()(const db::Rec<RecT> &rec) const {
    return map([this, &rec](auto c) {
	auto fnd(rec.find(c));
	return (fnd == rec.end()) ? c->type.null : c->type.from_val(fnd->second);
      },
      *this);
  }

  template <typename RecT, typename...ValT>
  typename Key<RecT, ValT...>::Type
  Key<RecT, ValT...>::operator ()(const RecT &rec) const {
    return map([this, &rec](auto c) { return rec.*c->field; },
	       *this);
  }

  template <typename RecT, typename...ValT>
  Key<RecT, ValT...> make_key(const Col<RecT, ValT> &...cols) {
    return Key<RecT, ValT...>(cols...);
  }

  template <typename RecT, typename...ValT>
  void copy(const Key<RecT, ValT...> &key, Rec<RecT> &dest, const RecT &src) {
    for_each(key, [&dest, &src](auto c) { c->copy(dest, src); });
  }

  template <typename RecT, typename...ValT>
  void copy(const Key<RecT, ValT...> &key, Rec<RecT> &dest, const Rec<RecT> &src) {
    for_each(key, [&dest, &src](auto c) {
	auto found = src.find(c);
	if (found != src.end()) { dest[c] = found->second; }
      });
  }
}}

#endif
