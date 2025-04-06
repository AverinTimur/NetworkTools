# Server
**Server(string address, int port, function_type manage_response, buffer_size)** 

Constructor

buffer_size is size of buffer used on message reading

function_type is: 
* std::function<std::string* (std::string)> if you want to close socket after first message
* std::function<void (std::function<std::string ()> read, std::function<void (std::string*)> send, std::function<void ()> close)> if you want for full server behavior control 

**Start**

Start server

**Stop**

Stop server

# Client
**Client(string address, int port, function_type manage_request, buffer_size)**

Constructor

buffer_size is size of buffer used on message reading

function_type is:
* string if you want to send one message
* std::function<void (std::function<std::string ()> read, std::function<void (std::string*)> send, std::function<void ()> close)> if you want for full server behavior control

**Start**

Send message

**Stop**

Stop client
  
