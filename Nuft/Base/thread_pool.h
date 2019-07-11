//
// Created by 王森 on 2019-06-25.
//

#include <thread>
#include <queue>

#ifndef NUFT_THREAD_POOL_H
#define NUFT_THREAD_POOL_H

#endif //NUFT_THREAD_POOL_H


namespace Base{

    struct ThreadExecutor{

        typedef std::function<void()>Task;
        typedef std::pair<std::string,std::function<void ()>>CapTask;
        size_t capacity;
        std::atomic<int> in_use;

        bool check_empty_unguard(){

            return tasks.empty();
        }

        ThreadExecutor(size_t capacity_):capacity(capacity_){
            close_flag.store(true);
            in_use.store(0);

            auto inner_loop = [this](){

                while (close_flag.load()){
                    std::unique_lock<std::mutex>lk(mtx);
                    while (check_empty_unguard()){
                        cond_empty.wait(lk);
                        if(!close_flag.load()){
                            lk.unlock();
                            return;;
                        }
                    }

                    CapTask t = tasks.front();
                    tasks.pop();
                    lk.unlock();
#if !defined(_HIDE_DEBUG_INFO)
                    printf("RUN %s\n",t.first.c_str());
#endif
                    std::atomic_fetch_add(&in_use,1);
                    t.second();
                    std::atomic_fetch_sub(&in_use,1);
#if !defined(_HIDE_DEBUG_INFO)
                    printf("Finish %s\n",t.first.c_str());
#endif
                }
            };
            ths = new std::thread[capacity](inner_loop);
        }
        ~ThreadExecutor(){
            wait();
            delete []ths;
        }

        void add_task(Task&& t){
            std::unique_lock<std::mutex> lk(mtx);
            tasks.push(std::make_pair(std::string(""),t));
#if !defined(_HIDE_DEBUG_INFO)
            printf("Add Task Running %d Queue %d Capacity %d\n", in_use.load(), tasks.size(), capacity);
#endif
            cond_empty.notify_one();
        }

        void add_task(const std::string& name,Task&& t){
            std::unique_lock<std::mutex> lk(mtx);
            tasks.push(std::make_pair(name,t));
#if !defined(_HIDE_DEBUG_INFO)
            printf("Add Task Running %d Queue %d Capacity %d\n", in_use.load(), tasks.size(), capacity);
#endif
            cond_empty.notify_one();
        }

        void stop(){
            close_flag.store(false);
            cond_empty.notify_all();
        }

        void wait(){
            stop();
            for(size_t i = 0;i<capacity;i++){
                if(ths[i].joinable()){
                    ths[i].join();
                }
            }
        }

        void detach(){
            for(size_t i = 0;i<capacity;i++){
                if(ths[i].joinable()){
                    ths[i].detach();
                }
            }
        }

        int workload()const {
            return in_use.load();
        }

        int in_queue()const {
            return tasks.size();
        }

        std::thread* ths;
        std::queue<CapTask> tasks;
        std::mutex mtx;
        std::condition_variable cond_empty;
        std::atomic<bool>close_flag;
    };
}