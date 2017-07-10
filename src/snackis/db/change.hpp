#ifndef SNACKIS_DB_CHANGE_HPP
#define SNACKIS_DB_CHANGE_HPP

#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
namespace db {
  struct Change {
    virtual ~Change();
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
  };

  struct ChangeSet {
    str label;
    Time committed_at;
    std::vector<Change *> changes;

    ChangeSet(const str &lbl, std::vector<Change *> &chs);
    ~ChangeSet();
  };
}}

#endif
