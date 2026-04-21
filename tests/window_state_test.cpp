#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "window_state.h"
#include <cmath>

using namespace window_state;

// ---------------------------------------------------------------------------
// to_physical / to_logical
// ---------------------------------------------------------------------------

TEST_CASE("to_physical scale=1 is identity") {
    auto ps = to_physical(LogicalSize{1280, 720}, 1.0f);
    CHECK(ps.w == 1280);
    CHECK(ps.h == 720);
}
