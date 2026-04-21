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

TEST_CASE("to_physical scale=2 doubles dimensions") {
    auto ps = to_physical(LogicalSize{640, 360}, 2.0f);
    CHECK(ps.w == 1280);
    CHECK(ps.h == 720);
}

TEST_CASE("to_logical scale=2 halves dimensions") {
    auto ls = to_logical(PhysicalSize{1280, 720}, 2.0f);
    CHECK(ls.w == 640);
    CHECK(ls.h == 360);
}

TEST_CASE("to_physical scale=0 clamps to 1") {
    auto ps = to_physical(LogicalSize{100, 50}, 0.0f);
    CHECK(ps.w == 100);
    CHECK(ps.h == 50);
}

TEST_CASE("to_logical scale=0 clamps to 1") {
    auto ls = to_logical(PhysicalSize{100, 50}, 0.0f);
    CHECK(ls.w == 100);
    CHECK(ls.h == 50);
}

TEST_CASE("to_logical round-trips through to_physical within 1px") {
    LogicalSize orig{100, 200};
    auto ps = to_physical(orig, 1.5f);
    auto ls = to_logical(ps, 1.5f);
    CHECK(std::abs(ls.w - orig.w) <= 1);
    CHECK(std::abs(ls.h - orig.h) <= 1);
}

// ---------------------------------------------------------------------------
// initial_geometry
// ---------------------------------------------------------------------------

TEST_CASE("initial_geometry no saved data returns physical defaults") {
    Settings::WindowGeometry saved{};
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.size.w == Settings::WindowGeometry::kDefaultPhysicalWidth);
    CHECK(g.size.h == Settings::WindowGeometry::kDefaultPhysicalHeight);
    CHECK(g.has_position == false);
    CHECK(g.maximized == false);
}

TEST_CASE("initial_geometry valid saved size returned as-is") {
    Settings::WindowGeometry saved{};
    saved.width = 1920; saved.height = 1080;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.size.w == 1920);
    CHECK(g.size.h == 1080);
}

TEST_CASE("initial_geometry negative x sets has_position=false") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 720;
    saved.x = -1; saved.y = 100;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.has_position == false);
}

TEST_CASE("initial_geometry negative y sets has_position=false") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 720;
    saved.x = 100; saved.y = -1;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.has_position == false);
}

TEST_CASE("initial_geometry valid position sets has_position=true") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 720;
    saved.x = 50; saved.y = 80;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.has_position == true);
    CHECK(g.position.x == 50);
    CHECK(g.position.y == 80);
}

TEST_CASE("initial_geometry maximized flag propagated") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 720;
    saved.maximized = true;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.maximized == true);
}

TEST_CASE("initial_geometry zero-width saved falls back to defaults") {
    Settings::WindowGeometry saved{};
    saved.width = 0; saved.height = 720;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.size.w == Settings::WindowGeometry::kDefaultPhysicalWidth);
    CHECK(g.size.h == Settings::WindowGeometry::kDefaultPhysicalHeight);
}

TEST_CASE("initial_geometry zero-height saved falls back to defaults") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 0;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.size.w == Settings::WindowGeometry::kDefaultPhysicalWidth);
    CHECK(g.size.h == Settings::WindowGeometry::kDefaultPhysicalHeight);
}

TEST_CASE("initial_geometry clamp_fn is called when provided") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 720;
    bool called = false;
    auto clamp = [&](int* w, int* h, int* x, int* y) {
        called = true;
        *w = 800; *h = 600; *x = -1; *y = -1;
    };
    auto g = initial_geometry(saved, clamp);
    CHECK(called == true);
    CHECK(g.size.w == 800);
    CHECK(g.size.h == 600);
    CHECK(g.has_position == false);
}

TEST_CASE("initial_geometry clamp_fn nullptr is safe") {
    Settings::WindowGeometry saved{};
    saved.width = 1280; saved.height = 720;
    auto g = initial_geometry(saved, nullptr);
    CHECK(g.size.w == 1280);
}
