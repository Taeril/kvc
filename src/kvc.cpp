#include "kvc.hpp"
#include "version.hpp"

#include "utils.hpp"

#include <fmt/core.h>

namespace kvc {

// version
std::string version() { return VERSION; }
unsigned version_major() { return VERSION_MAJOR; }
unsigned version_minor() { return VERSION_MINOR; }
unsigned version_patch() { return VERSION_PATCH; }

// KVC

enum class State {
    Nothing,
    Key,
    KeySpace,
    PreValue,
    Value,
    Quote,
    String,
    PreComment,
    Comment,
    Array,
    Error,
};

Config::Config() {
}

Config::Config(std::string const& cfg) {
	parse(cfg);
}

Config::~Config() {
}


Config& Config::parse(std::string const& cfg) {
    State state = State::Nothing;
    char quote = '\0';
    bool in_array = false;
    bool something = false;

    size_t key_start = 0;
    size_t key_end = 0;
    size_t value_start = 0;
    size_t value_end = 0;
    size_t comment_start = 0;
    size_t comment_end = 0;

    KVC kvc;

    size_t pos =0;
    for(auto const& c : cfg) {
        if(state == State::Nothing) {
            quote = '\0';
            in_array = false;
            something = false;
            key_start = 0;
            key_end = 0;
            value_start = 0;
            value_end = 0;
            comment_start = 0;
            comment_end = 0;

            if(
               (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_'
            ) {
                key_start = pos;
                state = State::Key;
            } else if(c == ' ' || c == '\t' || c == '\n') {
                // ???
                // do nothing?
            } else if(c == '#') {
                state = State::PreComment;
            } else {
                comment_start = pos;
                state = State::Comment; // err
            }
        } else if(state == State::Key) {
            if(
               (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               (c >= '0' && c <= '9') ||
               c == '_' || c == '-'
            ) {
                // do nothing
            } else if(c == ' ' || c == '\t') {
                key_end = pos;
                state = State::KeySpace;
            } else if(c == ':' || c == '=') {
                key_end = pos;
                state = State::PreValue;
            } else if(c == '#') {
                key_end = pos;
                state = State::PreComment;
            } else if(c == '\n') {
                key_end = pos;
                state = State::Nothing;
            } else {
                comment_start = pos;
                state = State::Comment; // err
            }
        } else if(state == State::KeySpace) {
            if(c == ' ' || c == '\t') {
                // do nothing
            } else if(c == ':' || c == '=') {
                state = State::PreValue;
            } else if(c == '#') {
                state = State::PreComment;
            } else {
                comment_start = pos;
                state = State::Comment; // err
            } 
        } else if(state == State::PreValue) {
            if(c == ' ' || c == '\t') {
                // do nothing
            } else if(c == '%') {
                state = State::Quote;
            } else if(c == '"' || c == '\'' || c == '`') {
                quote = c;
                value_start = pos + 1;
                state = State::String;
            } else if(c == '#') {
                state = State::PreComment;
            } else if(c == '[') {
                in_array = true;
                kvc.values.clear();
                kvc.is_array = true;
                state = State::Array;
            } else if(c == '\n') {
                state = State::Nothing;
            } else {
                value_start = pos;
                state = State::Value;
            }
        } else if(state == State::Value) {
            if(c == '\n') {
                value_end = pos;
                state = State::Nothing;
            } else if(c == '#') {
                state = State::PreComment;
            } else if(c != ' ' && c != '\t') {
                value_end = pos + 1;
            }
        } else if(state == State::Quote) {
            if(c == '\n') {
                state = State::Nothing; // err
            } else if(c == '#') {
                state = State::PreComment;
            } else {
                if(c == '(') {
                    quote = ')';
                } else if(c == '{') {
                    quote = '}';
                } else if(c == '[') {
                    quote = ']';
                } else if(c == '<') {
                    quote = '>';
                } else {
                    quote = c;
                }
                value_start = pos + 1;
                state = State::String;
            }
        } else if(state == State::String) {
            if(c == quote) {
                value_end = pos;
                if(in_array) {
                    state = State::Array;
                } else {
                    state = State::PreComment;
                }
            } else if(in_array && quote == ',' && c == ']') {
                value_end = pos;
                state = State::Array;
            } else if(c == '\n') {
                value_end = pos - 1;
                state = State::Nothing; // err
            }
        } else if(state == State::PreComment) {
            if(c == ' ' || c == '\t' || c == '#') {
                // do nothing
            } else if(c == '\n') {
                state = State::Nothing;
            } else {
                comment_start = pos;
                state = State::Comment;
            }
        } else if(state == State::Comment) {
            if(c == '\n') {
                comment_end = pos;
                state = State::Nothing;
            }
        } else if(state == State::Array) {
            if(c == ' ' || c == '\t') {
                // do nothing
            } else if(c == '%') {
                state = State::Quote;
            } else if(c == '"' || c == '\'') {
                quote = c;
                value_start = pos + 1;
                state = State::String;
            } else if(c == '#' || c == '[') {
                state = State::Error; // err?
            } else if(c == ']') {
                state = State::PreComment;
            } else if(c == '\n') {
                state = State::Nothing; //err
            } else if(c == ',') {
                // ???
                // do nothing
            } else {
                quote = ',';
                value_start = pos;
                state = State::String;
            }
        } else if(state == State::Error) {
            throw 1; // TODO: ...
        }

        if(state == State::Array && in_array && something && value_start) {
            kvc.values.push_back(cfg.substr(value_start, value_end-value_start));

            value_start = 0;
            value_end = 0;
        }
        if(state == State::Nothing && (something || in_array)) {
            kvc.key = cfg.substr(key_start, key_end-key_start);
            kvc.value = cfg.substr(value_start, value_end-value_start);
            kvc.comment = cfg.substr(comment_start, comment_end-comment_start);

            data_.push_back(kvc);

            in_array = false;
        }
        something = state != State::Nothing && state != State::Array;
        ++pos;
    }

	synchronize_data_and_keys();

	return *this;
}

Config& Config::parse_file(std::string const& path) {
	return parse(read_file(path));
}

std::string Config::to_string() {
	std::string ret;

    for(auto const& kvc : data_) {
    	if(kvc.key.size() > 0) {
			ret += fmt::format("{} = ", kvc.key);
			if(kvc.is_array) {
				ret += "[";
				bool first = true;
				for(auto const& v : kvc.values) {
					if(!first) ret += ", ";
					ret += quote_value(v);
					first = false;
				}
				ret += "]";
			} else {
				ret += quote_value(kvc.value);
			}

			if(kvc.comment.size() > 0) {
				ret += fmt::format(" # {}", kvc.comment);
			}
		} else if(kvc.comment.size() > 0) {
			ret += fmt::format("# {}", kvc.comment);
		}
		ret += "\n";
	}

	return ret;
}

void Config::each(std::function<void(KVC const&)> callback) {
    for(auto const& kvc : data_) {
		callback(kvc);
	}
}


KVC* Config::get(std::string const& k) {
	auto p = keys_.find(k);
	if(p != keys_.end()) {
		return p->second;
	}

	return nullptr;
}

std::string const& Config::get_value(std::string const& k, std::string const& default_) {
	auto kvc = get(k);
	return kvc ? kvc->value : default_;
}

void Config::set(std::string const& k, std::string const& v, std::string const& c) {
	KVC* kvc = get(k);
	if(kvc) {
		kvc->value = v;
		kvc->comment = c;
	} else {
		add(k, v, c);
	}
}

void Config::add(std::string const& k, std::string const& v, std::string const& c) {
    KVC kvc;
    kvc.key = k;
    kvc.value = v;
    kvc.comment = c;
    kvc.is_array = false;

	data_.push_back(kvc);
	if(k.size() > 0) {
		keys_[kvc.key] = &data_.back();
    //} else {
	//	synchronize_data_and_keys();
	}
}

void Config::add(std::string const& k, std::vector<std::string> const& vs, std::string const& c) {
	if(k.size() > 0) {
		KVC kvc;
		kvc.key = k;
		kvc.comment = c;
		kvc.is_array = true;

		for(auto& v : vs) {
			kvc.values.push_back(v);
		}

		data_.push_back(kvc);
		keys_[kvc.key] = &data_.back();
	}
}


size_t Config::lines() {
	return data_.size();
}

std::string Config::quote_value(std::string const& str) {
	size_t count[94] = {0}; // 93 = '~'-'!'

    for(auto const& c : str) {
    	if(c >= '!' && c<= '~')
			++count[c - '!'];
	}

	char q = 0;
	if(count['"'-'!'] == 0) q = '"';
	else if(count['\''-'!'] == 0) q = '\'';
	else if(count['`'-'!'] == 0) q = '`';

	if(q != 0) {
		return fmt::format("{}{}{}", q, str, q);
	}

	char qalpha[] = "{[|(</~^@$!?=+-_*&:;.%"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789";
	char* p = qalpha;
	char q2 = 0;
	while(*p) {
		if(*p == '(' && count[')'-'!'] != 0) { ++p; continue; }
		if(*p == '{' && count['}'-'!'] != 0) { ++p; continue; }
		if(*p == '[' && count[']'-'!'] != 0) { ++p; continue; }
		if(*p == '<' && count['>'-'!'] != 0) { ++p; continue; }

		if(count[*p-'!'] == 0) break;

		++p;
	}
	if(*p == '(') q2 = ')';
	if(*p == '{') q2 = '}';
	if(*p == '[') q2 = ']';
	if(*p == '<') q2 = '>';

	if(*p) {
		q = *p;
	}

	if(q == 0) {
		// FIXME: what now?
		q = '\a';
	}

	if(q2 == 0) q2 = q;

	return fmt::format("%{}{}{}", q, str, q2);
}

void Config::synchronize_data_and_keys() {
	for(auto& kvc : data_) {
		keys_[kvc.key] = &kvc;
	}
}

} // namespace kvc

