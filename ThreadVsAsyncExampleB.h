#include<vector>
#include<algorithm>
#include<thread>
#include<future>
#include<iostream>
#include<cassert>

#include"WorkerTasks.h"
#include"HelperFunctions.h"


/**
 * Launch tasks in one place and collect the results in another place.
 */
namespace ThreadVsAsyncExampleB {

    auto launch_split_workers_with_std_thread(std::vector<int>const& data, std::vector<int>& results) {

        std::vector<std::thread> thread_vector;

        assert(results.size() >= 2);

        const auto split_vectors = HelperFunctions::split_vector(data,data.size()/2);

        // The vectors in the split_vectors tuple are will cease to exist after this function goes out scope.
        // Therefore, we can't pass the "split_vectors" by reference to the thread constructor (only if we did thread.join() in this method)
        auto thread_function_A = std::bind(WorkerTasks::accumulate_block_worker, std::get<0>(split_vectors), std::ref(results[0]));
        auto thread_function_B = std::bind(WorkerTasks::accumulate_block_worker, std::get<1>(split_vectors), std::ref(results[1]));

        auto worker_thread_A = std::thread(std::move(thread_function_A));
        auto worker_thread_B = std::thread(std::move(thread_function_B));

        thread_vector.push_back(std::move(worker_thread_A));
        thread_vector.push_back(std::move(worker_thread_B));

        return thread_vector;
    }

    void split_workers_thread_launcher() {

        std::vector<int> data_vector(10);
        std::iota(std::begin(data_vector), std::end(data_vector), /*initial value*/ 0);

        std::vector<int> thread_results(2, 0);

        auto threads = launch_split_workers_with_std_thread(data_vector, thread_results);
        std::for_each(std::begin(threads), std::end(threads), [](std::thread& thread){
            thread.join();
        });

        std::cout << "launch_split_workers_with_std_thread results = " << thread_results[0] << " and " << thread_results[1] << std::endl;
    }

    auto launch_split_workers_with_std_async(std::vector<int> const& data) {
        
        std::vector<std::future<int>> futures_vector;

        const auto split_vectors = HelperFunctions::split_vector(data,data.size()/2);

        auto async_function_A = std::bind(WorkerTasks::accumulate_block_worker_ret, std::get<0>(split_vectors));
        auto async_function_B = std::bind(WorkerTasks::accumulate_block_worker_ret, std::get<1>(split_vectors));

        auto future_A = std::async(std::launch::async, std::move(async_function_A));
        auto future_B = std::async(std::launch::async, std::move(async_function_B));

        futures_vector.push_back(std::move(future_A));
        futures_vector.push_back(std::move(future_B));

        return futures_vector;
    }
    
    void split_workers_async_launcher() {

        std::vector<int> data(10);
        std::iota(begin(data), end(data), 0);

        auto futures = launch_split_workers_with_std_async(data);
        
        std::cout << "launch_split_workers_with_std_async results = " << futures[0].get() << " and " << futures[1].get() << std::endl;
    }

    void async_task_retrieval() {

        std::vector<int> data(10);
        std::iota(std::begin(data), std::end(data), 0);

        auto async_task = std::bind(WorkerTasks::accumulate_block_worker_ret_with_delay, std::ref(data));

        auto task_future = std::async(std::launch::async, std::move(async_task));

        //Wait for the task to complete
        {
            std::cout << "async_task_retrieval result = " << std::endl;
            
            while(task_future.wait_for(std::chrono::microseconds(250)) != std::future_status::ready) {
                std::cout << "\t\t\t\tstill not ready" << std::endl;
            }

            std::cout << "\t\t\t\t" << task_future.get() << std::endl;
        }

    } 
}