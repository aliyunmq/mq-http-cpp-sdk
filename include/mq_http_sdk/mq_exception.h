#ifndef MQ_SDK_EXCEPTION
#define MQ_SDK_EXCEPTION

#include <exception>
#include <string>

namespace mq
{
namespace http
{
namespace sdk
{

#ifndef FUNCTION_NAME
	#ifdef __PRETTY_FUNCTION__
	#define FUNCTION_NAME __PRETTY_FUNCTION__
	#elif defined(__FUNCTION__)
	#define FUNCTION_NAME __FUNCTION__
	#elif defined(__func__)
	#define FUNCTION_NAME __func__
	#else
	#define FUNCTION_NAME ""
	#endif
#endif

#define MQ_THROW(ExClass, msg)                         \
    do                                                      \
    {                                                       \
        ExClass e(msg);                                    \
        e.Init(__FILE__, FUNCTION_NAME, __LINE__);    \
        throw e;                                            \
    }                                                       \
    while(false)

#define MQ_DEFINE_EXCEPTION(ExClass, Base)                 \
    ExClass(const std::string& msg = "")throw()             \
        : Base(msg)                                         \
    {}                                                      \
    ~ExClass() throw() {}                                   \
    /*override*/ std::string GetClassName() const           \
    {                                                       \
        return #ExClass;                                    \
    }

struct ErrorInfo
{
    std::string errorMessage;
    std::string code;
    std::string requestId;
    std::string hostId;
    int httpStatus;
};

class MQExceptionBase : public std::exception
{
public:
    MQExceptionBase(const std::string& msg = "") throw();

    virtual ~MQExceptionBase() throw();

    void Init(const char* file, const char* func, int line);

#ifndef _WIN32
    virtual std::string GetClassName() const;

    virtual std::string GetMessage() const;
#endif

	virtual std::string GetExceptionClassName() const;

	virtual std::string GetExceptionMessage() const;

    const char* what() const throw();

    const std::string& ToString() const;

    std::string GetStackTrace() const;

protected:
    std::string mMsg;
    const char* mFile;
    const char* mFunc;
    int mLine;

private:
    enum { MAX_STACK_TRACE_SIZE = 50 };
    void* mStackTrace[MAX_STACK_TRACE_SIZE];
    size_t mStackTraceSize;
    mutable std::string mWhat;
};

class MQServerException : public MQExceptionBase
{
public:
    MQServerException(const ErrorInfo& errorInfo) throw()
        : MQExceptionBase(errorInfo.errorMessage)
    {
        mErrorInfo.errorMessage = errorInfo.errorMessage;
        mErrorInfo.code = errorInfo.code;
        mErrorInfo.requestId = errorInfo.requestId;
        mErrorInfo.hostId = errorInfo.hostId;
        mErrorInfo.httpStatus = errorInfo.httpStatus;
    }
    ~MQServerException() throw(){}
    std::string GetClassName() const
    {
        return "MQServerException";
    }

    const std::string& GetRequestId() const
    {
        return mErrorInfo.requestId;
    }
    const std::string& GetErrorCode() const
    {
        return mErrorInfo.code;
    }
    const std::string& GetHostId() const
    {
        return mErrorInfo.hostId;
    }
    const int GetHttpStatus() const
    {
        return mErrorInfo.httpStatus;
    }
    const std::string& GetErrorMessage() const
    {
        return mErrorInfo.errorMessage;
    }

    ErrorInfo mErrorInfo;
};

}
}
}

#endif
