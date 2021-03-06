#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>

template<typename T>
struct has_to_string_method {
    template<typename U, size_t (U::*)() const>
    struct SFINAE {
    };

    template<typename U>
    static char Test(SFINAE<U, &U::to_string> *);

    template<typename U>
    static int Test(...);

    static const bool value = sizeof(Test<T>(0)) == sizeof(char);
};


inline std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &v) {
    os << "[";
    for (std::vector<std::string>::const_iterator ii = v.begin(); ii != v.end(); ++ii) {
        os << " '" << *ii << "'";
        if (ii + 1 != v.end()) os << ",";
    }
    os << "]";
    return os;
}



inline std::ostream &operator<<(std::ostream &os, const std::vector<int> &v) {
    os << "[";
    for (auto ii = v.begin(); ii != v.end(); ++ii) {
        os << " " << *ii << "";
        if (ii + 1 != v.end()) os << ",";
    }
    os << "]";
    return os;
}


template<class T>
inline std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    os << "[";
    for (auto ii = v.begin(); ii != v.end(); ++ii) {
        os << " " << ii->to_string() << "";
        if (ii + 1 != v.end()) os << std::endl;
    }
    os << "]";
    return os;
}

/*
template <typename T, template<typename> typename C>
std::ostream& operator << (std::ostream& os, const C<T>& v)
{
	int i = 0;
	os << "[";
	for(auto &el : v) {
		os << " " << el;
		i++;
		if (i%50 == 0)
			os << " ..." << std::endl;
	}
	os << "]'";
	return os;
}
*/

template<std::size_t S>
static std::basic_ostream<char> &operator<<(std::basic_ostream<char> &os, const std::array<float, S> &v) {
    int i = 0;
    os << "[";
    for (auto &el : v) {
        os << " " << el;
        i++;
        if (i % 50 == 0)
            os << " ..." << std::endl;
    }
    os << "]'";
    return os;
}

static std::basic_ostream<char> &operator<<(std::basic_ostream<char> &os, const std::vector<float> &v) {
    int i = 0;
    os << "[";
    for (auto &el : v) {
        os << " " << el;
        i++;
        if (i % 50 == 0)
            os << " ..." << std::endl;
    }
    os << "]'";
    return os;
}

//template std::ostream& operator<< <float,4>(std::ostream&, const std::array<float,4>&);

//template<int L>
//std::ostream& operator<<<float,std::array>(std::ostream&, const std::array<float, L>&);

template<typename T>
inline std::ostream &operator<<(std::ostream &os, const std::set<T> &set) {
    os << "set{";
    for (auto &v:set) os << " '" << v << "' ";
    os << "}";
    return os;
}


template<typename T>
inline std::ostream &operator<<(std::ostream &os, const std::unordered_set<T> &set) {
    os << "uset{";
    for (auto &v:set) os << " '" << v << "' ";
    os << "}";
    return os;
}


template<typename T,  std::size_t N>
inline std::ostream &operator<<(std::ostream &os, const std::array<T,N> &arr) {
    os << "array<" <<  (typeid(T) == typeid(std::string) ? "string" : typeid(T).name()) << ">[";
    for (auto &v:arr) os << " '" << v << "', ";
    os << "]";
    return os;
}



template<typename K,  typename T>
inline std::ostream &operator<<(std::ostream &os, const std::unordered_map<K,T> &map) {
    os << "unordered_map{";
    for (auto &p:map) os << " [[" << p.first << "]]: " << p.second << ", ";
    os << "}";
    return os;
}


//<std::string, influxdb::client::fetchResult>


