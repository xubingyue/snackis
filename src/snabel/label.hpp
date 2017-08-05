#ifndef SNABEL_LABEL_HPP
#define SNABEL_LABEL_HPP

#include <cstdint>
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Label {
    const str tag;
    int64_t depth, pc;
    
    Label(const str &tag, int64_t depth, int64_t pc);
  };
}

#endif
