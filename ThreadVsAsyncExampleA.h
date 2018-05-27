#include<vector>
#include<algorithm>
#include<thread>
#include<future>
#include<iostream>

#include"WorkerTasks.h"

/**
 * Have a worker perform a task.
 */
namespace ThreadVsAsyncExampleA {

    void use_worker_in_std_thread() {
        std::vector<int> data_vector(10);
        std::iota(std::begin(data_vector), std::end(data_vector), /*initial value*/ 0);

        int thread_result = 0;

        // C++ threads don't output a result so a reference to a variable must be used instead.
        auto thread_function = std::bind(WorkerTasks::accumulate_block_worker, std::ref(data_vector), std::ref(thread_result));
        std::thread worker_thread(std::move(thread_function));
        worker_thread.join();

        std::cout << "use_worker_in_std_thread result = " << thread_result << std::endl;
    }

    void use_worker_in_std_async(){

        std::vector<int> data_vector(10);
        std::iota(std::begin(data_vector), std::end(data_vector), /*initial value*/ 0);

        auto async_function = std::bind(WorkerTasks::accumulate_block_worker_ret, std::ref(data_vector));
        
        std::future<int> result_future = std::async(std::launch::async, std::move(async_function));
        std::cout << "use_worker_in_std_async result = " << result_future.get() << std::endl;
    }
}