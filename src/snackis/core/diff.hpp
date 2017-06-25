#ifndef SNACKIS_DIFF_HPP
#define SNACKIS_DIFF_HPP

namespace snackis {
  template <typename T>
  using Diff = std::pair<std::vector<T>, std::vector<T>>;

  template <typename T>
  Diff<T> diff(const std::set<T> &x, const std::set<T> &y) {
    std::vector<T> add, rem;
    std::set_difference(y.begin(), y.end(), x.begin(), x.end(),
			std::back_inserter(add));
    std::set_difference(x.begin(), x.end(), y.begin(), y.end(),
			std::back_inserter(rem));
    return std::make_pair(add, rem);
  }

  template <typename T>
  void patch(std::set<T> &in, const Diff<T> &diff) {
    for (auto it: diff.first) { in.insert(it); }
    for (auto it: diff.second) { in.erase(it); }
  }
}

#endif
