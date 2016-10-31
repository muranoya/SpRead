#ifndef PREFETCHER_HPP
#define PREFETCHER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include "DataCache.hpp"
#include "ImageFile.hpp"
#include "BasicImage.hpp"

class Prefetcher
{
public:
    explicit Prefetcher();
    ~Prefetcher();

    void putRequest(const std::vector<ImageFile> &args);
    BasicImage *get(const std::string &key);
    void setCacheSize(int n);
    int getCacheSize() const;

private:
    std::thread *master;
    std::thread *worker[8];
    DataCache<std::string, BasicImage> cache;
    std::vector<ImageFile> files;
    std::vector<ImageFile> reqfiles;

    std::mutex mtx;
    std::mutex mtx_req;
    std::condition_variable cond_get;
    std::condition_variable cond_put;
    std::condition_variable cond_req;
    std::condition_variable cond_order;
    int task_no;
    int task_filled;
    int task_putid;

    ImageFile *getTask(int &id);
    void setResult(const std::string &key, int id,
            BasicImage *data);

    void master_thread();
    void worker_thread();
};

#endif // PREFETCHER_HPP
