#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

#include <string>
#include <vector>
#include "kvc.hpp"

TEST_CASE("Arrays") {
	kvc::Config cfg;

	cfg.parse(R"~(
foo = [1, 2, 3]
bar = ["1", "2", "3"]
)~");

	std::vector<std::string> values = {
		"1", "2", "3"
	};


	auto foo = cfg.get("foo");
	auto bar = cfg.get("bar");

	REQUIRE(foo != nullptr);
	REQUIRE(bar != nullptr);

	REQUIRE(foo->is_array == true);
	REQUIRE(bar->is_array == true);

	auto foos = foo->values;
	auto bars = bar->values;

	CHECK(foos.size() == values.size());
	CHECK(bars.size() == values.size());

	for(size_t i=0; i<values.size(); ++i) {
		CHECK(foos[i] == values[i]);
		CHECK(bars[i] == values[i]);
	}
}


