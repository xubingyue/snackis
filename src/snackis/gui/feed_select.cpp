#include "snackis/gui/feed_search.hpp"
#include "snackis/gui/feed_select.hpp"

namespace snackis {
namespace gui {
  FeedSelect::FeedSelect(Ctx &ctx): RecSelect<Feed>(ctx)
  { }

  SearchView<Feed> *FeedSelect::search() const {
    return new FeedSearch(ctx);
  }
}}
