#include<string>
#include<future>

#include"HelperFunctions.h"

namespace SyncVSAsync {

    size_t sync_copy_file(std::string const& input_filename, std::string const& output_filename) {
        return HelperFunctions::writeFile(HelperFunctions::readFile(input_filename), output_filename);
    }

    /**
     * std::promise
     * 
     * The most flexible way to provide a value for a future.
     * Computation is performed independently from the promise object
     * and the result is simply passed through the object to the future using the set_value() method. 
     */
    size_t future_promise_copy_file(std::string const& input_filename, std::string const& output_filename) {
        
        std::promise<std::vector<char>> read_promise;
        std::future<std::vector<char>> read_future = read_promise.get_future();
        auto read_thread = std::thread([&read_promise, &input_filename](){
            read_promise.set_value(HelperFunctions::readFile(input_filename));
        });

        std::promise<int> write_promise;
        std::future<int> write_future = write_promise.get_future();
        auto write_thread = std::thread([&write_promise, &read_future, &output_filename](){
            write_promise.set_value(HelperFunctions::writeFile(read_future.get(), output_filename));
        });

        // Wait for write operation to finish.
        while(write_future.wait_for(std::chrono::nanoseconds(1)) != std::future_status::ready) {
            std::cout << "file copy not yet completed" << std::endl;
        }
        
        // When we call "get()", the main thread will block
        // if the read and write tasks have not yet been completed.
        const auto copy_result = write_future.get();

        read_thread.join();
        write_thread.join();

        return copy_result;
    }

    /**
     * std::packaged_task
     * 
     * The second most flexible way to provide a value for a future.
     * The constructor takes a function and uses the return value of that function to set the value for the future.
     * Since packaged_tasks must be explicitly invoked, they can be created and then assigned to be run on particular threads.
     */
    size_t packaged_task_copy_file(std::string const& input_filename, std::string const& output_filename) {
        
        auto read_function = std::bind(HelperFunctions::readFile, input_filename);
        auto read_pkg_task = std::packaged_task<std::vector<char>()>(read_function);
        auto read_future = read_pkg_task.get_future();
        auto read_thread = std::thread(std::move(read_pkg_task)); // a packaged_task cannot be copied

        auto write_function = std::bind(HelperFunctions::writeFile, read_future.get(), output_filename);
        auto write_pkg_task = std::packaged_task<size_t()>(write_function);
        auto write_pkg_future = write_pkg_task.get_future();
        auto write_thread = std::thread(std::move(write_pkg_task)); // a packaged_task cannot be copied

        // Wait for write operation to finish.
        while(write_pkg_future.wait_for(std::chrono::nanoseconds(1)) != std::future_status::ready) {
            std::cout << "file copy not yet completed" << std::endl;
        }
        const auto copy_result = write_pkg_future.get();

        read_thread.join();
        write_thread.join();

        return copy_result;
    }

    /**
     * std::async
     * 
     * A high-level utility method to provide a value for a future.
     * Provides the least flexibility, but also the simplest way to execute an asynchronous computation.
     * The method (note: std::async is not an object like the others, it's a function)
     * takes a function and uses the return value of that function to set the value for the future.
     * 
     * The primary distinction between std::async and std::packaged_task is that std::async automatically begins execution upon calling it.
     * Additionally, the caller has no control over where the task is scheduled to run (including on the current thread).
     */
    size_t async_copy_file(std::string const& input_filename, std::string const& output_filename) {
        
        auto read_async_task = std::bind(HelperFunctions::readFile, input_filename);
        auto read_future = std::async(std::launch::async, read_async_task);

        auto write_async_task = std::bind(HelperFunctions::writeFile, read_future.get(), output_filename);
        auto write_future = std::async(std::launch::async, write_async_task);

        // Wait for write operation to finish.
        while(write_future.wait_for(std::chrono::nanoseconds(1)) != std::future_status::ready) {
            std::cout << "file copy not yet completed" << std::endl;
        }

        return write_future.get();
    }
}