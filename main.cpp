import Client;
#include <nlohmann/json.hpp>
using namespace nlohmann;

using std::cout, std::cin, std::endl, std::string;

auto main (int, char **) -> int
{
	string inp;


	// cout << send ("127.0.0.1", "8080", "hello world") << endl;
	auto user = json {};

	auto authenticated = false;

	while (not authenticated)
	{
		cout << "Enter username: ";
		cin >> inp;
		user ["username"] = inp;

		cout << "Enter password: ";
		cin >> inp;
		user ["password"] = inp;

		auto msg = string 
		{
			"POST /login HTTP/1.1\n"
			"Content-type: application/json; charset-UTF-8\n"
			"Content-Length: "
		} + std::to_string (user.get <string> ().size ()) + "\n\n" + user.get <string> ();
	}

	




	return 0;
}