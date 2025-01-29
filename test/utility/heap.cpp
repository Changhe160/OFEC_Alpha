#include "../utility/catch.hpp"
#include "../utility/heap/median_heap.h"

using namespace ofec;

TEST_CASE("Median Heap", "[heap]") {
	MedianHeap<double> h;
	h.insert(5);
	REQUIRE(h.median() == 5);
	h.insert(9);
	REQUIRE(h.median() == 7);
	h.insert(3);
	REQUIRE(h.median() == 5);
	h.insert(7);
	REQUIRE(h.median() == 6);
	h.insert(6);
	REQUIRE(h.median() == 6);
	h.clear();
	h.insert(5);
	REQUIRE(h.median() == 5);
	h.insert(9);
	REQUIRE(h.median() == 7);
	h.insert(3);
	REQUIRE(h.median() == 5);
	h.insert(7);
	REQUIRE(h.median() == 6);
	h.insert(6);
	REQUIRE(h.median() == 6);
}