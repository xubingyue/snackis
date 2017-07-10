#include "snackis/db/change.hpp"

namespace snackis {
namespace db {
  Change::~Change()
  { }

  ChangeSet::ChangeSet(const str &lbl, std::vector<Change *> &chs):
    label(lbl), committed_at(now())
  {
    changes.swap(chs);
  }

  ChangeSet::~ChangeSet() {
    for (auto c: changes) { delete c; }
  }
}}
