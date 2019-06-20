#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_MATCHERS
#include "catch.hpp"

#include <string>
#include "kvc.hpp"

TEST_CASE("Version") {
	std::string version = kvc::version();
	std::string ver
		= std::to_string(kvc::version_major()) + "."
		+ std::to_string(kvc::version_minor()) + "."
		+ std::to_string(kvc::version_patch());

	CHECK(version == ver);
}

