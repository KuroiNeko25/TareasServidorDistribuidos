/*
AUTHOR: Abhijeet Rastogi (http://www.google.com/profiles/abhijeet.1989)

This is a very simple HTTP server. Default port is 10000 and ROOT for the server is your current working directory..

You can provide command line arguments like:- $./a.aout -p [port] -r [path]

for ex. 
$./a.out -p 50000 -r /home/
to start a server at port 50000 with root directory as "/home"

$./a.out -r /home/shadyabhi
starts the server at port 10000 with ROOT as /home/shadyabhi

Modificado para efectos de la clase 2019-03-07 
Por John Sanabria - john.sanabria@correounivalle.edu.co

 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define BYTES 1024

char *ROOT;
int listenfd, client;
void error(char *);
void startServer(char *);
void respond();

int fibonacci(int x);
int ackerman(int x, int y);


int main(int argc, char* argv[])
{
  struct sockaddr_in clientaddr;
  socklen_t addrlen;
  char c;    

  //Default Values PATH = ~/ and PORT=10000
  char PORT[6];
  ROOT = getenv("PWD");
  strcpy(PORT,"9988");

  int slot=0;
  printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
  // Setting all elements to -1: signifies there is no client connected
  startServer(PORT);

  // ACCEPT connections
  while (1)
  {
    addrlen = sizeof(clientaddr);
    client = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

    if (client<0) // error en la solicitud
      error ("accept() error");
    else // efectivamente se atiende la solicitud
    {
      // incluir el codigo de modo que el hijo
      // del servidor web atienda la peticion
      fork();
      respond();
    }
  }

  return 0;
}

//start server
void startServer(char *port)
{
  struct addrinfo hints, *res, *p;

  // getaddrinfo for host
  memset (&hints, 0, sizeof(hints));
  // Se indica que son sockets de tipo TCP
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  // Esta asignacion 'ai_flags' y el 'NULL' en 'getaddrinfo()'
  // permiten que la variable "res" pueda ser usada  con 
  // 'bind()' y 'accept()'
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo( NULL, port, &hints, &res) != 0)
  {
    perror ("getaddrinfo() error");
    exit(1);
  }
  // socket and bind
  // se asocia el servidor a las interfaces de red que 
  // tiene disponibles el equipo donde corre la aplicacion
  for (p = res; p!=NULL; p=p->ai_next)
  {
    listenfd = socket (p->ai_family, p->ai_socktype, 0);
    if (listenfd == -1) continue;
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
  }
  if (p==NULL)
  {
    perror ("socket() or bind()");
    exit(1);
  }

  // se limpia la variable
  freeaddrinfo(res);

  // listen for incoming connections
  if ( listen (listenfd, 1000000) != 0 )
  {
    perror("listen() error");
    exit(1);
  }
}


//client connection
void respond()
{
  char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999], result[80], *operacion, *factores;
  int rcvd, fd, bytes_read;
  int number = 0, res = 0, x=0, y=0;

  memset( (void*)mesg, (int)'\0', 99999 );

  rcvd=recv(client, mesg, 99999, 0);

  if (rcvd<0)    // receive error
    fprintf(stderr,("recv() error\n"));
  else if (rcvd==0)    // receive socket closed
    fprintf(stderr,"Client disconnected upexpectedly.\n");
  else    // message received
  {
    printf("%s", mesg);
    reqline[0] = strtok (mesg, " \t\n");
    printf("\n\n(%s)\n\n",reqline[0],reqline[1],reqline[2]);
    if ( strncmp(reqline[0], "GET\0", 4)==0 )
    {
      reqline[1] = strtok (NULL, " \t");
      reqline[2] = strtok (NULL, " \t\n");

      //printf("\n\n(%s) (%s) (%s)\n\n",reqline[0],reqline[1],reqline[2]);
      if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
      {
        write(client, "HTTP/1.0 400 Bad Request\n", 25);
      }
      else
      {
        if ( strncmp(reqline[1], "/\0", 2)==0 )
	write(client, "HTTP/1.0 400 Bad Request\n", 25);          
	//reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

        strcpy(path, ROOT);
        strcpy(&path[strlen(ROOT)], reqline[1]);
        printf("file: %s\n", path);	

	operacion = strtok(path, "/");
	//printf("aqui %s", operacion);
	while(operacion != NULL)
	{
	  printf("ok -> ");
	  if(strncmp( operacion, "fibo", 4)!=0 && strncmp( operacion, "ack", 3)!=0)
	  {
	      operacion = strtok(NULL, "/");
	  }
  	  else
	  {
	    if(strncmp( operacion, "fibo", 4)==0)
	    {
	      operacion = strtok(NULL, "/");
	      factores = strtok(operacion, ".");
	      x = atoi(factores);
	      if(x < 0)
	      {
		write(client, "HTTP/1.0 400 Bad Request\n", 25);          
	      }
	      else
	      {
		res = fibonacci(x);
		sprintf(result, "%d\n", res);
		write(client, result, sizeof(result));
	      }
	    }
	    else
	    {
	      operacion = strtok(NULL, "/");
	      x = atoi(operacion);
	      operacion = strtok(NULL, "/");
	      factores = strtok(operacion, ".");
	      y = atoi(factores);
	      if((x < 0) || (y < 0))
	      {
		write(client, "HTTP/1.0 400 Bad Request\n", 25);          
	      }
	      else
	      {
		res = ackerman(x,y);
		sprintf(result, "%d\n", res);
		write(client, result, sizeof(result));
	      }
	    }
	  }
	}
      }
    }
  }


  //Closing SOCKET
  shutdown (client, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
  close(client);
}

//función fibonacci
int fibonacci(int x)
{
  int resultado=0;
  if(x == 0) 
  {
    resultado = 0;
  }
  else
  {
    if(x == 1)
    {
      resultado = 1;
    }
    else
    {
      resultado = fibonacci(x-1) + fibonacci(x-2);
    }
  }
  return resultado;
}

//funcion ackerman
int ackerman(int x, int y)
{
  int resultado;
  if(x == 0)
  {
    resultado = y + 1;
  }
  else
  {
    if((x > 0) && (y == 0))
    {
      resultado = ackerman((x - 1), 1);
    }
    else
    {
      resultado = ackerman((x - 1), ackerman(x, y - 1));
    }
  }
  return resultado;
}

