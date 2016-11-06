#ifndef UNCOPYABLE_HPP
#define UNCOPYABLE_HPP

class Uncopyable
{
protected:
    Uncopyable() {}
    ~Uncopyable() {}
    
private:
    Uncopyable(const Uncopyable &);
    Uncopyable(Uncopyable &&);
    Uncopyable &operator=(const Uncopyable &);
    Uncopyable &operator=(Uncopyable &&);
};

#endif // UNCOPYABLE_HPP
