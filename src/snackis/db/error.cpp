#include "snackis/db/error.hpp"

namespace snackis {
namespace db {

DbError::DbError(const std::string &msg):
    Error(std::string("DbError: ") + msg) { }

}}
