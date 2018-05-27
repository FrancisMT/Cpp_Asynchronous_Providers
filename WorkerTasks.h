#pragma once

#include<vector>
#include<algorithm>
#include<stdexcept>
#include<thread>

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