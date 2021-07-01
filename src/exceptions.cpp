#include <exceptions.h>

#include <sstream>

namespace shvav8 {

std::ostream& operator<<(std::ostream& ostream, const Exception& exception) {
    return ostream << exception.message();
}
}  // namespace shvav8
