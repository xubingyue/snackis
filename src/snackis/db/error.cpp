#include "snackis/db/error.hpp"

namespace snackis {
namespace db {

DbError::DbError(const str &msg):
    Error(str("DbError: ") + msg) { }

}}
