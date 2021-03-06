#include "jobSystem.h"
#include <random>


JobSystem::JobSystem(std::size_t worker_threads_count , std::size_t jobs_per_thread)
{
    workers.reserve(worker_threads_count);
    master_thread_id = std::this_thread::get_id();
    std::size_t jobs_per_queue = jobs_per_thread;
    workers.emplace_back(this,jobs_per_queue,JobWorker::Mode::Master);
    master_worker = &workers[0];
    master_worker->set_state(JobWorker::State::Running);
    for (std::size_t i=1;i< worker_threads_count;i++)
    {
        workers.emplace_back(this,jobs_per_queue,JobWorker::Mode::Background);
    }

   for (std::size_t i=1;i< worker_threads_count;i++)
    {
        workers[i].run();
    } 

}

JobSystem::~JobSystem(){

    std::size_t worker_threads_count = workers.size();
   for (std::size_t i=1;i< worker_threads_count;i++)
    {
        workers[i].set_state(JobWorker::State::Stopping);
    } 
   for (std::size_t i=1;i< worker_threads_count;i++)
    {
        workers[i].join();
    } 
}

 JobWorker* JobSystem::get_random_worker(){
   
   static std::random_device rd;
   static std::mt19937 gen=std::mt19937{rd()};
   static std::uniform_int_distribution<std::size_t> dist= std::uniform_int_distribution<std::size_t>{0, workers.size()-1};
   int index =dist(gen);
   
    JobWorker* worker = &workers[index];
    
    if(worker->is_running())
    {
        return worker;
    }
    else
    {
        return nullptr;
    }
}
