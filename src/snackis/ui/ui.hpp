#ifndef SNACKIS_UI_HPP
#define SNACKIS_UI_HPP

#define KEY_CTRL(c)				\
  ((c) & 037)					\

namespace snackis {
namespace ui {
  const int KEY_RETURN(10), KEY_SPACE(32), KEY_TAB(9);
 
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
