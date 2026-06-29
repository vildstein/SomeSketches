#ifndef SKEL_DEFINES_H
#define SKEL_DEFINES_H

static char* program_name;

#if defined(OS_DEFINED_CMAKE_LIN)

#define INIT()		program_name = \
					strrchr(argv[ 0 ], '/' )  ? \
					program_name++ : \
					( program_name = argv[ 0 ] );

#define BZERO_SOCK_ADDR_IN_STRUCT(sap) bzero( sap, sizeof(*sap) );
#define IS_VALID_SOCKET(socket_descriptor) ( (socket_descriptor) >= 0 )

typedef int SOCKET;

#elif defined(OS_DEFINED_CMAKE_WIN)

#define INIT() WSADATA data;  if ( WSAStartup(MAKEWORD(2, 2), &data) ) { printf("Failed to initialize.\n"); return -1; };
#define BZERO_SOCK_ADDR_IN_STRUCT(sap) memset( sap, 0, sizeof(*sap) );
#define IS_VALID_SOCKET(socket_descriptor) ( (socket_descriptor) != INVALID_SOCKET )

#endif

#define EXIT(s) 	puts("exit");	\
						exit( s );

#define CLOSE(s) if ( close( s ) ) error( 1, errno, \
								"CLOSE FUNC MISTAKE " )

#define SET_ERRNO(e) errno = ( e )
#define NLISTEN 5
//#define TO_STRING(value) #value

typedef struct sockaddr_in SIN; //#define SIN struct sockaddr_in
#define STRUCT_SOCKADDR_CAST (struct sockaddr*)
#define STRUCT_IN_ADDR_CAST (struct in_addr*)

#define DEFAULT_HOST {"127.0.0.1"}
#define DEFAULT_PORT {"9000"}
#define DEFAULT_NETWORK_PROTOCOL {"tcp"};

#define SET_ADDRESS_FORWARD_DECL void set_address(char* hostName, char* portNumber, SIN* sap, char* protocol);
#define ERROR_FORWARD_DECL void error(int status, int err, char* format, ...);
#define CLIENT_FUNC_FORWARD_DECL static void client(SOCKET s, SIN* peer);
#define SERVER_FUNC_FORWARD_DECL static void server(SOCKET s, SIN* peer);

#define TCP_SERVER_FORWARD SOCKET tcp_server(char* hostName, char* portName);
#define TCP_CLIENT_FORWARD SOCKET tcp_client(char* hostName, char* portName);
#define UDP_SERVER_FORWARD SOCKET udp_server(char* hostName, char* portName, struct sockaddr_in* local);
#define UDP_CLIENT_FORWARD SOCKET udp_client(char* hostName, char* portName, struct sockaddr_in* sp);

//#define PROGRAM_NAME char* program_name;

#define TRUE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>

#if defined(OS_DEFINED_CMAKE_WIN)
#include <winsock2.h>
#include <winsock.h>
#include <stddef.h>
typedef size_t SOCKET;
#endif

#if defined(OS_DEFINED_CMAKE_LIN)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#endif

#endif //SKEL_DEFINES_H
