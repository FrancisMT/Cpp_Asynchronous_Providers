#include<iostream>
#include<vector>
#include<numeric>
#include<algorithm>
#include<functional>
#include<thread>
#include<future>
#include<cassert>
#include<stdexcept>

namespace HelperFunctions {
    template<class T>
    std::tuple<std::vector<T>, std::vector<T>> split_vector(std::vector<T> const& original_vector, const size_t split_at_index) {
        
        auto split_left = std::vector<T>(std::begin(original_vector), std::begin(original_vector)+split_at_index);
        auto split_right = std::vector<T>(std::begin(original_vector) + split_at_index, std::end(original_vector));

        return std::make_tuple(split_left, split_right);
    }
}

namespace WorkerTasks {
    void accumulate_block_worker(std::vector<int> const& data, int& result) {
        result = std::accumulate(std::begin(data), std::end(data), /*initial value of the sum*/0);
    }

    int accumulate_block_worker_ret(std::vector<int> const& data) {
        return std::accumulate(std::begin(data), std::end(data), /*initial value of the sum*/0);
    }

    int accumulate_block_worker_ret_with_delay(std::vector<int> const& data) {
        //Force delay for demonstration purposes
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return std::accumulate(std::begin(data), std::end(data), /*initial value of the sum*/0);
    }

    void accumulate_block_worker_exception(std::vector<int> const& data, int& result) {
        //Force throw for demonstration purposes
        throw std::runtime_error("something broke");     

        result = std::accumulate(std::begin(data), std::end(data), /*initial value of the sum*/0);
    }

      int accumulate_block_worker_ret_exception(std::vector<int> const& data) {
        //Force throw for demonstration purposes
        throw std::runtime_error("something broke");     

        return std::accumulate(std::begin(data), std::end(data), /*initial value of the sum*/0);
    }
}

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
        std::thread worker_thread(thread_function);
        worker_thread.join();

        std::cout << "use_worker_in_std_thread result = " << thread_result << std::endl;
    }

    void use_worker_in_std_async(){

        std::vector<int> data_vector(10);
        std::iota(std::begin(data_vector), std::end(data_vector), /*initial value*/ 0);

        auto async_function = std::bind(WorkerTasks::accumulate_block_worker_ret, std::ref(data_vector));
        
        std::future<int> result_future = std::async(std::launch::async, async_function);
        std::cout << "use_worker_in_std_async result = " << result_future.get() << std::endl;
    }
}

/**
 * Launch tasks in one place and collect the results in another place.
 */
namespace ThreadVsAsyncExampleB {

    auto launch_split_workers_with_std_thread(std::vector<int>const& data, std::vector<int>& results) {

        std::vector<std::thread> thread_vector;

        assert(results.size() >= 2);

        const auto split_vectors = HelperFunctions::split_vector(data,data.size()/2);

        // The vectors in the split_vectors tuple are will cease to exist after this function goes out scope.
        //Therefore, we can't pass the "split_vectors" by reference to the thread constructor (only if we did thread.join() in this method)
        auto thread_function_A = std::bind(WorkerTasks::accumulate_block_worker, std::get<0>(split_vectors), std::ref(results[0]));
        auto thread_function_B = std::bind(WorkerTasks::accumulate_block_worker, std::get<1>(split_vectors), std::ref(results[1]));

        std::thread worker_thread_A(thread_function_A);
        std::thread worker_thread_B(thread_function_B);

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

        auto future_A = std::async(std::launch::async, async_function_A);
        auto future_B = std::async(std::launch::async, async_function_B);

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

        auto task_future = std::async(std::launch::async, async_task);

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
            auto thread = std::thread(thread_function);
            
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
            auto task_future = std::async(std::launch::async, async_task);

            std::cout << "catch_exception_from_std_async result = " << task_future.get() << std::endl;
        }
        catch(std::runtime_error const& e)
        {
            std::cerr << "Caught an error: " << e.what() << std::endl;
        }
        
    }
}



int main() {

    std::cout << "main thread" << std::endl;

    ThreadVsAsyncExampleA::use_worker_in_std_thread();
    ThreadVsAsyncExampleA::use_worker_in_std_async();
    ThreadVsAsyncExampleB::split_workers_thread_launcher();
    ThreadVsAsyncExampleB::split_workers_async_launcher();
    ThreadVsAsyncExampleB::async_task_retrieval();
    //ThreadVsAsyncExampleC::catch_exception_from_std_thread();
    ThreadVsAsyncExampleC::catch_exception_from_std_async();

    return 0;
}