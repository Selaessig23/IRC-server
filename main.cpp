/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2025/11/05 11:51:04 by mstracke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> //for socket, bind, listen, accept
#include <errno.h>
#include <iostream>

/**
 * @param1: port to use
 * @param2: pw required by any IRC client that tries to connect to this webserver
 */
int	main(int argc, char *argv[])
{
	int	fd_server = 0;

	//test debug-mode
	#ifdef DEBUG
		std::cout << "[DEBUG] Debug mode is ON" << std::endl;
	#endif

	if (argc != 3)
	{
		std::cerr << "Check number of arguments. Required 2: portno | pw" << std::endl;
		return (1);
	}
	(void)argv;
	//TO-DO's:
	//play around with required (& allowed) c-functions
	//int socket(int domain, int type, int protocol);
	//AF_INET == IPv4 | SOCK_STREAM = two-way connection-based byte streams | protocl number (if several)
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_server < 0)
	{
		std::cerr << "Socket creation error" << std::endl;
		return 1;
	}
	//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	//int listen(int sockfd, int backlog);
	//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	return (0);
}
