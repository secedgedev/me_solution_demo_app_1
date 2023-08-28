
#define MODE_SERVER "server"
#define MODE_CLIENT "client"
#define BUF_SIZE 65535

#define FILE_MESSAGE "./message.txt"
#define FILE_STATUS "./status.txt"
#define FILE_CLIENT "./client.txt"


#define STATUS_OK 1
#define STATUS_ERROR 0
#define STATUS_CONNECTED "Client connected"
#define STATUS_DISCONNECTED "Client disconnected"

extern int GetIFs();
extern int check_network_interface(char *netIf);
extern int write_status(const char status[]);
extern int write_client_info(const char clientinfo[]);
