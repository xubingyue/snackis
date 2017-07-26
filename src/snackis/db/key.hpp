#ifndef SNACKIS_DB_KEY_HPP
#define SNACKIS_DB_KEY_HPP

#include <tuple>
#include "snackis/db/rec.hpp"

namespace snackis {
namespace db {
  template <typename RecT, typename...KeyT>
  struct Key: std::tuple<const Col<RecT, KeyT> *...> {
    using Type = std::tuple<KeyT...>;

    Key(const Col<RecT, KeyT> &...cols);
    Type operator ()(const KeyT &...vals) const;
    Type operator ()(const db::Rec<RecT> &rec) const;
    Type operator ()(const RecT &rec) const;
  };

  template <typename RecT, typename...KeyT>
  Key<RecT, KeyT...>::Key(const Col<RecT, KeyT> &...cols):
    std::tuple<const Col<RecT, KeyT> *...>(&cols...)
  { }

  template <typename RecT, typename...KeyT>
  typename Key<RecT, KeyT...>::Type
  Key<RecT, KeyT...>::operator ()(const KeyT &...vals) const {
    return std::make_tuple(vals...);
  }

  template <typename RecT, typename...KeyT>
  typename Key<RecT, KeyT...>::Type
  Key<RecT, KeyT...>::operator ()(const db::Rec<RecT> &rec) const {
    return map([this, &rec](auto c) {
	auto fnd(rec.find(c));
	return (fnd == rec.end()) ? c->type.null : c->type.from_val(fnd->second);
      },
      *this);
  }

  template <typename RecT, typename...KeyT>
  typename Key<RecT, KeyT...>::Type
  Key<RecT, KeyT...>::operator ()(const RecT &rec) const {
    return map([this, &rec](auto c) { return rec.*c->field; },
	       *this);
  }

  template <typename RecT, typename...KeyT>
  Key<RecT, KeyT...> make_key(const Col<RecT, KeyT> &...cols) {
    return Key<RecT, KeyT...>(cols...);
  }

  template <typename RecT, typename...KeyT>
  void copy(const Key<RecT, KeyT...> &key, Rec<RecT> &dest, const RecT &src) {
    for_each(key, [&dest, &src](auto c) { c->copy(dest, src); });
  }

  template <typename RecT, typename...KeyT>
  void copy(const Key<RecT, KeyT...> &key, Rec<RecT> &dest, const Rec<RecT> &src) {
    for_each(key, [&dest, &src](auto c) {
	auto found = src.find(c);
	if (found != src.end()) { dest[c] = found->second; }
      });
  }
}}

#endif
