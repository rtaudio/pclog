
inline std::ostream& operator << (std::ostream& os, const std::vector<std::string>& v)
{
	os << "";
	for (std::vector<std::string>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
	{
		os << " '" << *ii<<"'";
		if(ii+1 != v.end()) os << ",";
	}
	os << "";
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

static std::basic_ostream<char>& operator << (std::basic_ostream<char>& os, const std::array<float,4>& v)
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

//template std::ostream& operator<< <float,4>(std::ostream&, const std::array<float,4>&);

//template<int L>
//std::ostream& operator<<<float,std::array>(std::ostream&, const std::array<float, L>&);

