#include "HEADERS.h"

#include "Network.cpp"

using namespace std;


int main(){
    string message = "Test Message";
    auto client = Network::Client("127.0.0.1", 8000, &message);
    auto code = client.Start();
    cout <<  code << endl;

    getchar();

    client.Stop();

    return 0;
}
