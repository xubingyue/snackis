#ifndef SNABEL_LABEL_HPP
#define SNABEL_LABEL_HPP

#include <cstdint>

namespace snabel {
  struct Label {
    int64_t depth, pc;
    
    Label(int64_t depth, int64_t pc);
  };
}

#endif
