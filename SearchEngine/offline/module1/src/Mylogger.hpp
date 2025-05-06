#ifndef __WD_MYLOGGER_H__
#define __WD_MYLOGGER_H__
#include <iostream>
#include <string>
#include <log4cpp/Category.hh>

class Mylogger
{
    // 自动释放类，用于在程序结束时释放 Mylogger 实例
    class AutoRelease
    {
    public:
        AutoRelease() {}
        ~AutoRelease() {
            Mylogger::destroy();
        }
    };
public:
    // 线程安全的单例模式实现
    static Mylogger* getInstance()
    {
        static Mylogger instance;
        return &instance;
    }

    // 销毁 Mylogger 实例
    static void destroy()
    {
        static bool destroyed = false;
        if (!destroyed) {
            // 这里可以添加更多的资源释放逻辑
            destroyed = true;
        }
    }

    // 记录错误日志
    void error(const char* msg);

    // 支持可变参数的错误日志记录
    template <class... Args>
    void error(const char* msg, Args... args)
    {
        _cat.error(msg, args...);
    }

    // 记录警告日志
    void warn(const char* msg);

    // 支持可变参数的警告日志记录
    template <class... Args>
    void warn(const char* msg, Args... args)
    {
        _cat.warn(msg, args...);
    }

    // 记录调试日志
    void debug(const char* msg);

    // 支持可变参数的调试日志记录
    template <class... Args>
    void debug(const char* msg, Args... args)
    {
        _cat.debug(msg, args...);
    }

    // 记录信息日志
    void info(const char* msg);

    // 支持可变参数的信息日志记录
    template <class... Args>
    void info(const char* msg, Args... args)
    {
        _cat.info(msg, args...);
    }

private:
    // 构造函数，初始化日志类别
    Mylogger();
    // 析构函数，释放资源
    ~Mylogger();

private:
    static Mylogger* _pInstance;
    log4cpp::Category& _cat;
    static AutoRelease _ar;
};

// 为日志消息添加前缀，包含文件、函数和行号信息
#define addprefix(msg)  std::string("[")\
    .append(__FILE__).append(":")\
    .append(__func__).append(":")\
    .append(std::to_string(__LINE__)).append("] ")\
    .append(msg).c_str()

// 记录信息日志的宏
#define LogInfo(msg, ...) Mylogger::getInstance()->info(addprefix(msg), ##__VA_ARGS__)
// 记录错误日志的宏
#define LogError(msg, ...) Mylogger::getInstance()->error(addprefix(msg), ##__VA_ARGS__)
// 记录警告日志的宏
#define LogWarn(msg, ...) Mylogger::getInstance()->warn(addprefix(msg), ##__VA_ARGS__)
// 记录调试日志的宏
#define LogDebug(msg, ...) Mylogger::getInstance()->debug(addprefix(msg), ##__VA_ARGS__)

#endif