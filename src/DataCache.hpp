#ifndef DATACACHE_HPP
#define DATACACHE_HPP

#include <unordered_map>

template<class K, class T>
class DataCache
{
public:
    explicit DataCache(int cap = 20)
        : map(), head(nullptr), tail(nullptr), cap(cap) {}
    DataCache(const DataCache &other) = delete;
    DataCache(DataCache &&other) = delete;
    DataCache &operator=(const DataCache &other) = delete;
    DataCache &operator=(DataCache &&other) = delete;
    ~DataCache()
    {
        for (DC_Con *p = head; ; )
        {
            if (p == nullptr) break;
            DC_Con *n = p->next;
            delete p;
            p = n;
        }
    }

    void setCapacity(int capacity)
    {
        if (capacity < 0) return;
        bool c = (cap != capacity);
        cap = capacity;
        if (c) while (count() > cap) clean();
    }
    int getCapacity() const { return cap; }

    T *get(const K &key)
    {
        if (map.find(key) == map.end())
        {
            return nullptr;
        }

        DC_Con *con = map[key];
        if (con->prev) con->prev->next = con->next;
        if (con->next) con->next->prev = con->prev;
        if (con == tail) tail = con->prev;
        if (con == head) head = con->next;

        if (head) head->prev = con;
        con->next = head;
        con->prev = nullptr;
        head = con;
        return con->data;
    }
    bool add(const K &key, T *data)
    {
        if (cap == 0) return false;

        DC_Con *con = new DC_Con(key, data);
        map.insert(make_pair(key, con));
        if (head)
        {
            con->next = head;
            con->prev = nullptr;
            head->prev = con;
            head = con;
        }
        else
        {
            con->next = con->prev = nullptr;
            head = tail = con;
        }

        while (cap < count()) clean();
        return true;
    }
    bool contain(const K &key) const { return map.find(key) != map.end(); }
    int count() const { return map.size(); }

private:
    class DC_Con
    {
    public:
        explicit DC_Con(const K &key, T *d)
            : next(nullptr), prev(nullptr), key(key), data(d) {}
        DC_Con(const DC_Con &) = delete;
        DC_Con(DC_Con &&) = delete;
        DC_Con &operator=(const DC_Con &) = delete;
        DC_Con &operator=(DC_Con &&) = delete;
        ~DC_Con() { delete data; }

        DC_Con *next, *prev;
        K key;
        T *data;
    };

    std::unordered_map<K, DC_Con*> map;
    DC_Con *head, *tail;
    int cap;

    void clean()
    {
        if (!tail) return;
        DC_Con *con = tail;
        map.erase(con->key);
        tail = con->prev;
        if (tail) tail->next = nullptr;
        if (head == con) head = nullptr;
        delete con;
    }
};

#endif // DATACACHE_HPP
