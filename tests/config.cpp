#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

#include <string>
#include <vector>
#include <kvc/kvc.hpp>

TEST_CASE("Empty") {
	kvc::Config cfg;

	size_t count = 0;
    cfg.each([&](kvc::KVC const&) {
    	++count;
	});
	
	CHECK(count == 0);
	CHECK(cfg.lines() == 0);
}

TEST_CASE("Basic") {
	kvc::Config cfg;

	cfg.parse(R"~(
key = value # comment
)~");

	auto test = cfg.get("key");
	auto missing = cfg.get("missing");

	CHECK(cfg.lines() == 1);

	CHECK(missing == nullptr);

	REQUIRE(test != nullptr);
	CHECK(test->key == "key");
	CHECK(test->value == "value");
	CHECK(test->comment == "comment");
	CHECK(test->is_array == false);
}

TEST_CASE("Default") {
	kvc::Config cfg;

	cfg.parse(R"~(
key = value # comment
)~");

	auto value_key = cfg.get_value("key", "error");
	auto value_default = cfg.get_value("missing", "default");

	CHECK(cfg.lines() == 1);

	CHECK(value_key == "value");
	CHECK(value_default == "default");
}

TEST_CASE("Strings") {
	kvc::Config cfg;

	cfg.parse(R"~(
quotes = "value"
apostrophes = 'value'
backticks = `value`
percent_curly_bracket = %{value}
percent_parenthesis = %(value)
percent_square_bracket = %[value]
)~");

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

