#ifndef HEADER_KVC_HPP
#define HEADER_KVC_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace kvc {

std::string version();
unsigned version_major();
unsigned version_minor();
unsigned version_patch();

struct KVC {
	std::string comment;
	std::string key;
	std::string value;
	std::vector<std::string> values;
	bool is_array = false;
};

class Config {
	public:
		Config();
		Config(std::string const& cfg);
		~Config();

		Config& parse(std::string const& cfg);
		Config& parse_file(std::string const& path);

		std::string to_string();

		void each(std::function<void(KVC const&)> callback);

		KVC* get(std::string const& k);

		std::string const& get_value(std::string const& k, std::string const& default_);

		void set(std::string const& k, std::string const& v, std::string const& c="");

		void add(std::string const& k, std::string const& v, std::string const& c="");
		void add(std::string const& k, std::vector<std::string> const& vs, std::string const& c="");

		size_t lines();
	private:
		std::vector<KVC> data_;
		std::unordered_map<std::string, KVC*> keys_; 

		std::string quote_value(std::string const& str);

		void synchronize_data_and_keys();
};

} // namespace kvc

#endif /* HEADER_KVC_HPP */

