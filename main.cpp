#include<iostream>
#include<vector>
#include<numeric>
#include<algorithm>
#include<functional>
#include<thread>
#include<future>
#include<cassert>
#include<stdexcept>

#include"ThreadVsAsyncExampleA.h"
#include"ThreadVsAsyncExampleB.h"
#include"ThreadVsAsyncExampleC.h"
#include"SyncVsAsyncExample.h"

int main() {

    std::cout << "main thread" << std::endl;

    ThreadVsAsyncExampleA::use_worker_in_std_thread();
    ThreadVsAsyncExampleA::use_worker_in_std_async();

    ThreadVsAsyncExampleB::split_workers_thread_launcher();
    ThreadVsAsyncExampleB::split_workers_async_launcher();
    ThreadVsAsyncExampleB::async_task_retrieval();

    //ThreadVsAsyncExampleC::catch_exception_from_std_thread();
    ThreadVsAsyncExampleC::catch_exception_from_std_async();

    SyncVSAsync::sync_copy_file("textfile.txt", "textfile_copy.txt");
    SyncVSAsync::future_promise_copy_file("textfile.txt", "textfile_copy.txt");
    SyncVSAsync::packaged_task_copy_file("textfile.txt", "textfile_copy.txt");
    SyncVSAsync::async_copy_file("textfile.txt", "textfile_copy.txt");
    

    return 0;
}