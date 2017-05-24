#ifndef SNACKIS_UI_HPP
#define SNACKIS_UI_HPP

namespace snackis {
namespace ui {
  struct Dim {
    int h, w;
    Dim(int h, int w);
  };

  struct Pos {
    int y, x;
    Pos(int y, int x);
  };

  Dim dim();
  void open();
  void close();
}}

#endif
