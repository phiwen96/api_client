import Client;

using std::cout, std::endl, std::string;

auto main (int, char **) -> int
{

	cout << send ("127.0.0.1", "8080", "hello world") << endl;

	return 0;
}