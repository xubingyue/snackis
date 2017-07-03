#ifndef SNACKIS_GUI_FEED_SELECT_HPP
#define SNACKIS_GUI_FEED_SELECT_HPP

#include "snackis/feed.hpp"
#include "snackis/gui/rec_select.hpp"

namespace snackis {
namespace gui {
  struct FeedSelect: RecSelect<Feed> {
    FeedSelect(Ctx &ctx);
    SearchView<Feed> *search() const override;
  };
}}

#endif
