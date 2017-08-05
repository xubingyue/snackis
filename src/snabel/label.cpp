#include "snabel/label.hpp"

namespace snabel {
  Label::Label(const str &tag, int64_t dep, int64_t pc):
    tag(tag), depth(dep), pc(pc)
  { }
}
