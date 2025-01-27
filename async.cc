#include <iostream>
#include <future>
#include <thread>
#include <atomic>

int g = 100;

// Modify asyncFunction to take the sleep duration in milliseconds
int asyncFunction(int sleep_ms, std::atomic<bool>& cancel_flag) {
    for (int i = 0; i < sleep_ms; ++i) {
        if (cancel_flag.load()) {
            std::cout << "Async function cancelled." << std::endl;
            return -1; // Indicate that the function was cancelled
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    g = 200;
    return 42;
}

int main(int argc, char* argv[])
{
    std::cout << "Async Test";

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <wait_time_ms> <sleep_ms>" << std::endl;
        return 1;
    }

    int wait_time_ms = std::stoi(argv[1]);
    int sleep_ms = std::stoi(argv[2]);

    std::atomic<bool> cancel_flag(false);
    std::future<int> result = std::async(std::launch::async, asyncFunction, sleep_ms, std::ref(cancel_flag));
    std::cout << "Waiting for async result..." << std::endl;

    if (result.wait_for(std::chrono::milliseconds(wait_time_ms)) == std::future_status::ready) {
        int value = result.get();
        std::cout << "Async result: " << value << " global value = " << g << std::endl;
    } else {
        cancel_flag.store(true); // Signal the async function to cancel
        std::cout << "Async result not ready within the timeout period. global value g = " << g << std::endl;
    }

    return 0;
}
