#include <iostream>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <deque>
#include <condition_variable>

std::deque <std::packaged_task <int()>> deq_t;
std::mutex mu_deq_t, mu_cout;
std::condition_variable cond_deq_t;

template <typename T>
void safe_cout (const T &x) {
   {
      std::lock_guard <std::mutex> lg_cout(mu_cout);
      std::cout << x << std::endl;
   }
}

int factorial(int n)
{
    int fact = 1;
    for(; n > 0; n--){
        fact = fact * n;
    }
    return fact;
}

void worker_thread()
{
    std::packaged_task<int()> t;
    {
        std::unique_lock<std::mutex> ul_deq_t(mu_deq_t);
        cond_deq_t.wait(ul_deq_t, [](){ return !deq_t.empty();});
        t = std::move(deq_t.front());
        deq_t.pop_front();
    }
    t();
}
int main()
{
    std::thread t1(worker_thread);
    std::packaged_task <int()> t(std::bind(factorial, 5));
    // get a future object associated to the packaged_task
    std::future <int> f = t.get_future();

    { // locks the mutex for the deque
        std::unique_lock<std::mutex> lg_deq_t(mu_deq_t);
        deq_t.push_back(std::move(t));
    }
    cond_deq_t.notify_one(); // notify on the condition variable
    safe_cout(f.get()); // get the value using future
    t1.join();
    return EXIT_SUCCESS;
}