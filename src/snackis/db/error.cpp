#include "snackis/db/error.hpp"

namespace snackis {

DbError::DbError(const str &msg):
    Error(str("DbError: ") + msg) { }

}
