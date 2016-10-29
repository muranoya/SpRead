#include "Prefetcher.hpp"

using namespace std;

Prefetcher::Prefetcher()
    : cache(20)
    , task_no(0)
    , task_filled(0)
    , task_putid(0)
{
    master = new thread(&Prefetcher::master_thread, this);
    
    int len = sizeof(worker)/sizeof(worker[0]);
    for (int i = 0; i < len; ++i)
    {
        worker[i] = new thread(&Prefetcher::worker_thread, this);
    }
}

Prefetcher::~Prefetcher()
{
    /*
    int len = sizeof(worker)/sizeof(worker[0]);
    for (int i = 0; i < len; ++i)
    {
        delete worker[i];
    }
    */
}

void
Prefetcher::putRequest(const vector<ImageFile> &tasks)
{
    mtx_req.lock();
    reqfiles.clear();
    reqfiles = tasks;
    cond_req.notify_one();
    mtx_req.unlock();
}

vector<unsigned char> *
Prefetcher::get(const string &key)
{
    vector<unsigned char> *data = nullptr;
    mtx.lock();
    if (cache.contain(key))
    {
        data = new vector<unsigned char>(*cache.get(key));
        data->shrink_to_fit();
    }
    mtx.unlock();
    return data;
}

void
Prefetcher::setCacheSize(int n)
{
    mtx.lock();
    cache.setCapacity(n);
    mtx.unlock();
}

int
Prefetcher::getCacheSize() const
{
    return cache.getCapacity();
}

ImageFile *
Prefetcher::getTask(int &id)
{
    ImageFile *f;
    unique_lock<mutex> um(mtx);
    for (;;)
    {
        while (task_no >= files.size())
        {
            cond_get.wait(um);
        }
        f = &files[task_no];
        id = task_no;
        task_no++;

        string k = f->createKey();
        if (cache.contain(k))
        {
            cache.get(k); // update
            task_putid++;
            task_filled++;
            if (task_filled >= files.size())
            {
                cond_put.notify_one();
            }
        }
        else
        {
            break;
        }
    }
    um.unlock();
    return f;
}

void
Prefetcher::setResult(const string &key, int id,
        vector<unsigned char> *data)
{
    unique_lock<mutex> um(mtx);
    while (task_putid < id)
    {
        cond_order.wait(um);
    }
    if (data) cache.add(key, data);
    cond_order.notify_all();
    task_filled++;
    task_putid++;
    if (task_filled >= files.size())
    {
        cond_put.notify_one();
        files.clear();
    }
    um.unlock();
}

void
Prefetcher::master_thread()
{
    for (;;)
    {
        unique_lock<mutex> um_req(mtx_req);
        for (;;)
        {
            while (reqfiles.empty() || getCacheSize() == 0)
            {
                cond_req.wait(um_req);
            }

            unique_lock<mutex> um(mtx);
            if (task_filled >= files.size())
            {
                task_no = 0;
                task_filled = 0;
                task_putid = 0;
                files.clear();
                files = reqfiles;
                reqfiles.clear();
                cond_get.notify_all();
                um.unlock();
                break;
            }
            um.unlock();
        }
        um_req.unlock();

        unique_lock<mutex> um(mtx);
        while (task_filled < files.size())
        {
            cond_put.wait(um);
        }
        um.unlock();
    }
}

void
Prefetcher::worker_thread()
{
    int id;
    for (;;)
    {
        ImageFile *f = getTask(id);
        vector<unsigned char> *data = f->readData();
        setResult(f->createKey(), id, data);
    }
}

