#include <iostream>
#include <thread>

int thread_routine()
{
    for (int i = 0; i < 10; i++)
        std::cout << "Hello World " << i << std::endl;
    return 0;
}

int main()
{
    std::thread t(thread_routine);
    t.join();
    return 0;
}