#include <iostream>
#include <future>
#include <thread>
#include <atomic>

class Connection {
    public:
       Connection() { a = 200; } 
       int a;
};

class asyncConnection {
    public:
        Connection *conn;
};

Connection g;

Connection *get_live_connection()
{
    return &g;
}

// Modify asyncFunction to take the sleep duration in milliseconds
asyncConnection asyncFunction(std::atomic<bool>& cancel_flag) {
    asyncConnection asyncConn;
    asyncConn.conn = nullptr;
    
    for (int i = 0; i < 50; ++i) {
        if (cancel_flag.load()) {
            std::cout << "Async function cancelled." << std::endl;
            return asyncConn; // Indicate that the function was cancelled
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    asyncConn.conn = get_live_connection();
    return asyncConn;
}

int main(int argc, char* argv[])
{
    std::cout << "Async Test";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <wait_time_ms>" << std::endl;
        return 1;
    }

    int wait_time_ms = std::stoi(argv[1]);
    //int sleep_ms = std::stoi(argv[2]);

    std::atomic<bool> cancel_flag(false);
    std::future<asyncConnection> result = std::async(std::launch::async, asyncFunction, std::ref(cancel_flag));
    std::cout << "Waiting for async result..." << std::endl;

    if (result.wait_for(std::chrono::milliseconds(wait_time_ms)) == std::future_status::ready) {
        asyncConnection value = result.get();
        std::cout << "Async result: " << value.conn->a << std::endl;
    } else {
        cancel_flag.store(true); // Signal the async function to cancel
    }

    return 0;
}
