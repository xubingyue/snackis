#include "file_field.hpp"

namespace snackis {
namespace ui {
  FileField::FileField(Form &frm, const Dim &dim, const str &lbl):
    EnumField<Path>(frm, dim, lbl) { }

  void init(FileField &fld, const Path &dir) {
    for (auto &i: PathIter(dir)) {
      const Path p(i);
      insert(fld, p.filename(), p);
    }
  }
}}
