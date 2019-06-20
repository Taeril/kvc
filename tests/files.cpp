#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

#include <cstdlib>
#include "kvc.hpp"

TEST_CASE("Strings") {
	kvc::Config cfg;

	cfg.parse_file("data/strings.txt");

	std::vector<std::string> keys = {
		"quotes",
		"apostrophes",
		"backticks",
		"percent_curly_bracket",
		"percent_parenthesis",
		"percent_square_bracket",
	};
	CHECK(cfg.lines() == keys.size());

	for(auto& key : keys) {
		auto test = cfg.get(key);
		REQUIRE(test != nullptr);

		CHECK(test->key == key);
		CHECK(test->value == "value");
	}
}


