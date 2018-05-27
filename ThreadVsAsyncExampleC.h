#include<vector>
#include<algorithm>
#include<thread>
#include<future>
#include<iostream>
#include<cassert>

#include"WorkerTasks.h"

/**
 * Catch an exception thrown somewhere else.
 */
namespace ThreadVsAsyncExampleC {

    /**
     * When called, this method will output:
     *  terminate called after throwing an instance of 'std::runtime_error'
     *  what():  something broke
     * 
     *  The C++ standard clearly states, "~thread(), if joinable(), calls std::terminate()".
     *  So trying to catch the exception in another thread won't help.
     */
    void catch_exception_from_std_thread() {

        std::vector<int> data(10);
        std::iota(std::begin(data), std::end(data), 0);

        int thread_result = 0;

        try
        {
            auto thread_function = std::bind(WorkerTasks::accumulate_block_worker_exception, std::ref(data), std::ref(thread_result));
            auto thread = std::thread(std::move(thread_function));
            
            // Wait for thread to complete.
            thread.join();

            std::cout << "catch_exception_from_std_thread result = " << thread_result << std::endl;
        }
        catch(const std::runtime_error& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    /**
     * The exeception is propagated to the calling thread
     * through the std::future and re-thrown 
     * when the future's "get()" method is called.
     */
    void catch_exception_from_std_async() {

        std::vector<int> data(10);
        std::iota(std::begin(data), std::end(data), 0);

        try
        {
            auto async_task = std::bind(WorkerTasks::accumulate_block_worker_ret_exception, std::ref(data));
            auto task_future = std::async(std::launch::async, std::move(async_task));

            std::cout << "catch_exception_from_std_async result = " << task_future.get() << std::endl;
        }
        catch(std::runtime_error const& e)
        {
            std::cerr << "Caught an error: " << e.what() << std::endl;
        }
        
    }
}