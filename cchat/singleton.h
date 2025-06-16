#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>
#include <iostream>

// 饿汉式
template <typename T>
class Singleton{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton&) = delete;

   static std::shared_ptr<T> _instance;

public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag, [&](){
            // 为什么不用 std::mark_shared ?
            _instance = std::shared_ptr<T>(new T());
        });

        return _instance;
    }

    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

// 模板类 静态局部变量，头文件 声明+定义
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;


#endif // SINGLETON_H
