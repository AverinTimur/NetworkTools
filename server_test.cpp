#include "HEADERS.h"

#include "Network.cpp"

using namespace std;

int main(){
    std::string buffer;
    function<std::string* (std::string)> test_func = [&buffer](std::string request) {
        std::cout << request << std::endl;
        buffer = request;
        return &buffer;
    };

    auto server = Network::Server("127.0.0.1", 8000, test_func);
    auto code = server.Start();
    cout << code << endl;

    getchar();

    server.Stop();

    return 0;
}
