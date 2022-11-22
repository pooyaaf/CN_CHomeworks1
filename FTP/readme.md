# Socket Programming Mini Projects 
Computer Networks - Fall 2022
* Mohammad Pooya Afshari - 810198577
* Mohammad Reza Sheshpari - 
## FTP
Implemented FTP simple using cpp. with two ports for transferring data and commands between server & clients.
### Explain files
* /root folder is the user base location and contains:
    - a.txt : a sample text file for testing retr command 
    - config.json : root of server files starts from here, Has a "config.json" for configuring server information
* /server folder contains all the server programs:
    - /server/commands: logging, user control and user commands:
        - Commands : interface of commands
        - control : methods for checking user access control to the files
        - file_system : file system commands implemented here.
        - logging : logging methods implemented here
        - user : implemented user commands 
    - /server/context :
        - AppContext : saves admin file desc and user context.
        - User : a class saves user info like if user is admin and if user is registered.
        - path : current path of the user, and methods for getting virtual and absolute path of the file system commands.
    - Application : each thread is an application which serves users commands.
    - Configuration : reads "configure.json" for server configuration
    - main : start and listen to socket
    - Message : response messages for defined commands
- Client : client which connect to server through cms socket and send/receive responses.
- Makefile : compile program and creates "server.out" and "client.out"
- Utility : methods of the socket programming 
- log.txt : resualt user log file
- help.txt : response user help command
