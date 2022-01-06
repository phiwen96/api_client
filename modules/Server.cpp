#include <signal.h>

export module Server;

export import Darwin;
export import std;

namespace api 
{

constexpr auto max_data_size = 1024; // max number of bytes we can get at once
constexpr auto backlog = 10;

inline auto sigchld_handler (int s) -> void
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
	errno = saved_errno;
}

// get sockaddr, IPv4 or
inline auto get_in_addr (sockaddr *sa) -> void *
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

export auto serve (char const* port, auto&& callback) -> int
{
	std::cout << "starting restful api" << std::endl;

	int sockfd, new_fd, numbytes; // listen on sock_fd, new connection on new_fd struct addrinfo hints, *servinfo, *p;
	struct addrinfo *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information 
	socklen_t sin_size;
	int yes = 1;
	char s [INET6_ADDRSTRLEN];
	int rv;
	char buf [max_data_size];

	auto hints = addrinfo
	{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};

	
	

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, backlog) == -1)
	{
		perror("listen");
		exit(1);
	}

	struct sigaction sa
	{
		.sa_handler = sigchld_handler,
		.sa_flags = SA_RESTART
	};

	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	printf("server: waiting for connections...\n");

	while (1)
	{ // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork())
		{				   // this is the child process
			close(sockfd); // child doesn't need the listener

			
			if ((numbytes = recv (sockfd, buf, max_data_size-1, 0)) == -1) 
			{ 
				perror("recv");
				exit(1); 
			}

			buf [numbytes] = '\0';

			char const* outgoing = callback (buf); 

			if (send(new_fd, outgoing, strlen (outgoing), 0) == -1)
			{
				perror("send");
			}

			close(new_fd);
			exit(0);
		}
		close(new_fd); // parent doesn't need this
	}

	return 0;
}
}