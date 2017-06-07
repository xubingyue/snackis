#ifndef SNACKIS_UI_FILE_FIELD_HPP
#define SNACKIS_UI_FILE_FIELD_HPP

#include "snackis/core/path.hpp"
#include "snackis/ui/enum_field.hpp"

namespace snackis {
namespace ui {
  struct FileField: public EnumField<Path> {
    FileField(Form &frm, const Dim &dim, const str &lbl);
  };

  void init(FileField &fld, const Path &dir);
}}

#endif
