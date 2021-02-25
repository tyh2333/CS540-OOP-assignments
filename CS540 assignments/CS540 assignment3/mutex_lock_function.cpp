//
// Created by macbook on 2020/11/22.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
/**
 * =====================================================
 *       lock :   lock mutex
 *     unlock : unlock mutex
 * =====================================================
 */
/** output:
id: 0  gum: 0 => 1
id: 1  gum: 1 => 2
id: 0  gum: 2 => 3
id: 1  gum: 3 => 4
id: 0  gum: 4 => 5
id: 1  gum: 5 => 6
*/
int g_num = 0;  // protected by g_num_mutex
std::mutex g_num_mutex;
void slow_increment(int id) {
    for (int i = 0; i < 3; ++i) {
        g_num_mutex.lock(); // lock before modify
        std::cout << "id: "<< id << "  gum: "<< g_num;
        ++g_num; // modify
        std::cout<< " => " << g_num << '\n';
        g_num_mutex.unlock(); // unlock after modify
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void test_lock_and_unlock(){
    std::thread t1(slow_increment, 0);
    std::thread t2(slow_increment, 1);
    /// Function join returns when the thread execution has completed.
    t1.join();
    t2.join();
}
/**
 * =====================================================
 * try_lock: tries to lock the mutex,
 *      returns if the mutex is not available
 * =====================================================
 */
/** output:
job exclusive (1)
job exclusive (2)
job exclusive (3)
job exclusive (4)
job shared (1)
**/
std::chrono::milliseconds interval(100);
std::mutex mutex;
int job_shared = 0; // both threads can modify 'job_shared',
// mutex will protect this variable
int job_exclusive = 0; // only one thread can modify 'job_exclusive'
// no protection needed
// this thread can modify both 'job_shared' and 'job_exclusive'
void job_1()
{
    std::this_thread::sleep_for(interval); // let 'job_2' take a lock
    while (true) {
        // try to lock mutex to modify 'job_shared'
        if (mutex.try_lock()) {
            std::cout << "job shared (" << job_shared << ")\n";
            mutex.unlock();
            return;
        } else {
            // can't get lock to modify 'job_shared'
            // but there is some other work to do
            ++job_exclusive;
            std::cout << "job exclusive (" << job_exclusive << ")\n";
            std::this_thread::sleep_for(interval);
        }
    }
}
// this thread can modify only 'job_shared'
void job_2()
{
    mutex.lock();
    /// After lock , job_2 sleep for 5 interval, so the try_lock in job_1 will fail
    /// during the sleeping time.
    std::this_thread::sleep_for(5 * interval);
    ++job_shared;
    mutex.unlock();
}
void test_try_lock(){
    std::thread thread_1(job_1);
    std::thread thread_2(job_2);
    thread_1.join();
    thread_2.join();
}

/**
 * ======================================================================
 * lock_guard:
 *      When a lock_guard object is created, it attempts to take ownership of the mutex
 * it is given. When control leaves the scope in which the lock_guard object was created,
 * the lock_guard is destructed and the mutex is released. The lock_guard class is
 * non-copyable.
 * ======================================================================
 */
 /** output:
main: 0
id: 1: 1 // thread 1, ++ mutex, when out of scope, release the mutex automatically
id: 2: 2 // thread 2, ++ mutex, when out of scope, release the mutex automatically
main: 2
  */
int g_i = 0;
std::mutex g_i_mutex;  // protects g_i

void safe_increment(int id)
{
    const std::lock_guard<std::mutex> lock(g_i_mutex);
    ++g_i;
    std::cout << "id: "<<id<< ": " << g_i << '\n';

    /// g_i_mutex is automatically released when lock goes out of scope
}
void test_lock_guard(){
    std::cout << "main: " << g_i << '\n';

    std::thread t1(safe_increment,1);
    std::thread t2(safe_increment,2);

    t1.join();
    t2.join();

    std::cout << "main: " << g_i << '\n';
}
/**
 * ======================================================================
 * unique_lock: movable, not copyable
 * std::unique_lock<std::mutex> sbguard1(my_mutex);
 * std::unique_lock<std::mutex> sbguard2(sbguard1); Wrong!!! not copyable
 *
 * Right: unique_lock is movable:
 *    std::unique_lock<std::mutex> sbguard2(std::move(sbguard));
 * ======================================================================
 */
struct Box {
    explicit Box(int num) : num_things{num} {}

    int num_things;
    std::mutex m;
};

void transfer(Box &from, Box &to, int num)
{
    // don't actually take the locks yet
    std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
    std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);

    // lock both unique_locks without deadlock
    std::lock(lock1, lock2);

    from.num_things -= num;
    to.num_things += num;

    // 'from.m' and 'to.m' mutexes unlocked in 'unique_lock' dtors
}
void test_unique_lock(){
    Box acc1(100);
    Box acc2(50);

    std::thread t1(transfer, std::ref(acc1), std::ref(acc2), 10);
    std::thread t2(transfer, std::ref(acc2), std::ref(acc1), 5);

    t1.join();
    t2.join();
}
int main(){
//    test_lock_and_unlock();
//    std::cout<<"***********"<<std::endl;
//    test_try_lock();
//    std::cout<<"***********"<<std::endl;
//    test_lock_guard();
//    std::cout<<"***********"<<std::endl;
    test_unique_lock();
}
