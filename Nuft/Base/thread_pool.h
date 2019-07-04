//
// Created by 王森 on 2019-06-25.
//

#include <thread>
#include <queue>

#ifndef NUFT_THREAD_POOL_H
#define NUFT_THREAD_POOL_H

#endif //NUFT_THREAD_POOL_H


namespace Base{

    struct ThreadPool{

        typedef std::function<void()>Task;
        typedef std::pair<std::string,std::function<void ()>>CapTask;

        bool check_empty_unguard(){

            return tasks.size()==0;
        }

        ThreadPool(size_t capacity_):capacity(capacity_) {

            close_flag.store(true);
            in_use.store(0);

            auto inner_loop = [this]() {

                while (close_flag.load()) {
                    std::unique_lock<std::mutex> M_lock(Mutex);

                    while (check_empty_unguard()) {
                        cond_val.wait(M_lock);

                        if (!close_flag.load()) {

                            M_lock.unlock();
                            return;
                        }
                    }
                    CapTask t = tasks.front();
                    tasks.pop();
                    M_lock.unlock();

                    printf("Run %s \n", t.first.c_str());
                    std::atomic_fetch_add(&in_use, 1);
                    t.second();
                    std::atomic_fetch_sub(&in_use, 1);
                    printf("Finish %s \n", t.first.c_str());
                }
            };

            Thread = new std::thread[capacity](inner_loop); /* 暂时先这样 */
        }

        ~ThreadPool(){
            wait();
            delete []Thread;
        }

        void add_task(Task &&t){
            std::unique_lock<std::mutex> lk(Mutex);
            tasks.push(std::make_pair(std::string(""),t));

            printf("Add task Running %d Queue, %d Capacity %d \n",in_use.load(),tasks.size(),capacity);

            cond_val.notify_one();
        }

        void stop(){
            close_flag.store(false);
            cond_val.notify_all();
        }

        void wait(){
            stop();

            for(size_t i = 0;i<capacity;i++){

                if(Thread[i].joinable()){
                    Thread[i].join();
                }
            }
        }

        void detach(){
            for(size_t i = 0;i<capacity;i++){

                if(Thread[i].joinable()){
                    Thread[i].join();
                }
            }
        }

        int workload()const {
            return in_use.load();
        }

        int in_queue()const {
            return tasks.size();
        }

        size_t capacity;
        std::atomic<int> in_use;
        std::thread *Thread;
        std::queue<CapTask> tasks;
        std::mutex Mutex;
        std::condition_variable cond_val;
        std::atomic<bool> close_flag;
    };
}