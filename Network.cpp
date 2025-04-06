#include "HEADERS.h"
using server_loop_type = std::function<void (std::function<std::string ()> read, std::function<void (std::string*)> send, std::function<void ()> close)>;

namespace Network {
    enum NetworkErrors {
        SUCCESS = 0,
        SOCKET_CREATION_ERROR = -1,
        BINDING_ERROR = -2,
        LISTENING_ERROR = -3,
        CONNECTION_ERROR = -4,
    };
    class Server {
    private:
        std::string address;
        int port;
        server_loop_type manage_response;
        long long buffer_size;

        int socket_fd;
        std::thread thread;
        bool running;

    public:
        Server(const std::string address, const int port, const server_loop_type manage_response, long long buffer_size = 1024) {
            this->address = address;
            this->port = port;
            this->manage_response = manage_response;
            this->running = false;
        }
        Server(const std::string address, const int port, const std::function<std::string* (std::string)> manage_response, long long buffer_size = 1024) :
            Server(address, port, [manage_response](std::function<std::string ()> read, std::function<void (std::string*)> send, std::function<void ()> close) {
                send(manage_response(read()));
                close();
            }){};
        ~Server() {
            this->Stop();
        }

        int Start() {
            socket_fd = socket(AF_INET , SOCK_STREAM , 0);
            if(socket_fd == 0) return SOCKET_CREATION_ERROR;

            sockaddr_in socket_address;
            socket_address.sin_family = AF_INET;
            socket_address.sin_addr.s_addr = inet_addr(address.c_str());
            socket_address.sin_port = htons(port);

            size_t socket_address_len = sizeof(socket_address);
            if (bind(socket_fd, reinterpret_cast<sockaddr*>(&socket_address), socket_address_len) < 0) return BINDING_ERROR;
            if (listen(socket_fd, 3) < 0) return LISTENING_ERROR;

            running = true;
            thread = std::thread([](bool &running, const int socket_fd, sockaddr_in socket_address, size_t socket_address_len, server_loop_type manage_response, long long buffer_size = 1024){
                char* buffer = new char[buffer_size];
                while(running) {
                    int new_socket = accept(socket_fd, reinterpret_cast<sockaddr*>(&socket_address), reinterpret_cast<socklen_t*>(&socket_address_len));
                    manage_response([new_socket, &buffer, buffer_size] (){
                        read(new_socket, buffer, buffer_size);
                        return buffer;
                    }, [new_socket](std::string *request){
                        send(new_socket, &(*request)[0], request->size(), 0);
                    }, [new_socket](){
                        close(new_socket);
                    });
                }
            }, std::ref(running), socket_fd, socket_address, socket_address_len, std::ref(manage_response), buffer_size);

            return SUCCESS;
        }
        void Stop() {
            if (running) {
                running = false;
                shutdown(socket_fd, SHUT_RDWR);
                socket_fd = -1;
                thread.join();
            }
        }
    };
    class Client {
    private:
        std::string address;
        int port;
        server_loop_type manage_request;
        long long buffer_size = 1024;

        int socket_fd;
        std::thread thread;

    public:
        Client(const std::string address, const int port, server_loop_type manage_request, long long buffer_size = 1024) {
            this->address = address;
            this->port = port;
            this->manage_request = manage_request;
        }
        Client(const std::string address, const int port, std::string *buffer, long long buffer_size = 1024):
            Client(address, port, [buffer](std::function<std::string ()> read, std::function<void (std::string* )> send, std::function<void ()> close) {
                send(buffer);
                *buffer = read();
                close();
            }, buffer_size){};
        ~Client() {
            this->Stop();
        }

        int Start() {
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);

            if(socket_fd == 0) return SOCKET_CREATION_ERROR;

            sockaddr_in socket_address;
            socket_address.sin_family = AF_INET;
            socket_address.sin_port = htons(port);
            socket_address.sin_addr.s_addr = inet_addr(address.c_str());

            auto socket_address_len = sizeof(socket_address);
            if(connect(socket_fd, reinterpret_cast<sockaddr *>(&socket_address), socket_address_len) < 0) return CONNECTION_ERROR;

            thread = std::thread([](const int socket_fd, sockaddr_in socket_address, size_t socket_address_len, server_loop_type manage_request, size_t buffer_size){
                char *buffer = new char[buffer_size];
                manage_request([socket_fd, &buffer, buffer_size] (){
                    read(socket_fd, buffer, buffer_size);
                    return buffer;
                }, [socket_fd](std::string *request){
                    const char * a = request->c_str();
                    send(socket_fd, a, request->size(), 0);
                }, [socket_fd](){
                    close(socket_fd);
                });
            }, socket_fd, socket_address, socket_address_len, std::ref(manage_request), buffer_size);

            return SUCCESS;
        }
        void Stop() {
            if (socket_fd != -1) {
                shutdown(socket_fd, SHUT_RDWR);
                socket_fd = -1;
                thread.join();
            }
        }
    };
}
