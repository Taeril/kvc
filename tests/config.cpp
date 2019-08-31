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

TEST_CASE("Set") {
	kvc::Config cfg;

	cfg.parse(R"~(
key = value # comment
)~");

	auto test = cfg.get("key");

	CHECK(cfg.lines() == 1);

	REQUIRE(test != nullptr);
	CHECK(test->key == "key");
	CHECK(test->value == "value");
	CHECK(test->comment == "comment");
	CHECK(test->is_array == false);

	cfg.set("key", "new", "test");

	auto test2 = cfg.get("key");

	CHECK(cfg.lines() == 1);

	REQUIRE(test2 != nullptr);
	CHECK(test2->key == "key");
	CHECK(test2->value == "new");
	CHECK(test2->comment == "test");
	CHECK(test2->is_array == false);
}

TEST_CASE("Add") {
	kvc::Config cfg;

	cfg.parse(R"~(
key = value # comment
)~");

	auto test = cfg.get("key");

	CHECK(cfg.lines() == 1);

	REQUIRE(test != nullptr);
	CHECK(test->key == "key");
	CHECK(test->value == "value");
	CHECK(test->comment == "comment");
	CHECK(test->is_array == false);

	cfg.add("key", "new", "test");

	auto test2 = cfg.get("key");

	CHECK(cfg.lines() == 2);

	REQUIRE(test2 != nullptr);
	CHECK(test2->key == "key");
	CHECK(test2->value == "new");
	CHECK(test2->comment == "test");
	CHECK(test2->is_array == false);

	cfg.add("key", "no comment");

	auto test3 = cfg.get("key");

	CHECK(cfg.lines() == 3);

	REQUIRE(test3 != nullptr);
	CHECK(test3->key == "key");
	CHECK(test3->value == "no comment");
	CHECK(test3->comment == "");
	CHECK(test3->is_array == false);

}

TEST_CASE("Update") {
	kvc::Config cfg;

	cfg.parse(R"~(
key = value # comment
)~");

	auto test = cfg.get("key");

	CHECK(cfg.lines() == 1);

	REQUIRE(test != nullptr);
	CHECK(test->key == "key");
	CHECK(test->value == "value");
	CHECK(test->comment == "comment");
	CHECK(test->is_array == false);

	cfg.update("key", "new", "test");

	auto test2 = cfg.get("key");

	CHECK(cfg.lines() == 1);

	REQUIRE(test2 != nullptr);
	CHECK(test2->key == "key");
	CHECK(test2->value == "new");
	CHECK(test2->comment == "test");
	CHECK(test2->is_array == false);

	cfg.update("missing", "value", "do nothing");

	auto test3 = cfg.get("missing");

	CHECK(cfg.lines() == 1);

	REQUIRE(test3 == nullptr);

}

TEST_CASE("Reset") {
	kvc::Config cfg;

	cfg.parse(R"~(
foo = [1, 2, 3]
bar = 1
)~");
	
	auto foo = cfg.get("foo");
	auto bar = cfg.get("bar");

	REQUIRE(foo != nullptr);
	REQUIRE(bar != nullptr);

	REQUIRE(foo->is_array == true);
	REQUIRE(bar->is_array == false);

	REQUIRE(foo->values.size() == 3);
	REQUIRE(bar->values.size() == 0);
}

