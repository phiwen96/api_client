import Client;

using std::cout, std::endl, std::string;

auto main (int, char **) -> int
{
	auto callback = [] (string incoming) -> char const*
	{
		cout << incoming << endl;
		return "hej";
	};

	return 0;
}