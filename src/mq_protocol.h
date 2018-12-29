// Copyright (C) 2015, Alibaba Cloud Computing

#ifndef MQ_PROTOCOL_H
#define MQ_PROTOCOL_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <stdint.h>
#include "pugixml.hpp"
#include "mq_utils.h"

namespace mq
{
namespace http
{
namespace sdk
{

struct AckMessageFailedItem
{
    AckMessageFailedItem()
        : errorCode("")
        , errorMessage("")
        , receiptHandle("")
    {
    }
    std::string errorCode;
    std::string errorMessage;
    std::string receiptHandle;
};

class ConsumeMessageResponse;

class Message
{
public:
    Message()
        : mMessageId("")
        , mReceiptHandle("")
        , mMessageBody("")
        , mMessageBodyMD5("")
        , mMessageTag("")
        , mPublishTime(-1)
        , mNextConsumeTime(-1)
        , mFirstConsumeTime(-1)
        , mConsumedTimes(-1)
    {
    }

    const std::string& getMessageId()
    {
        return mMessageId;
    }

    const std::string& getReceiptHandle()
    {
        return mReceiptHandle;
    }

    const std::string& getMessageBody()
    {
        return mMessageBody;
    }

    const std::string& getMessageBodyMD5()
    {
        return mMessageBodyMD5;
    }

    const std::string& getMessageTag()
    {
        return mMessageTag;
    }

    int64_t getPublishTime()
    {
        return mPublishTime;
    }

    int64_t getFirstConsumeTime()
    {
        return mFirstConsumeTime;
    }

    int64_t getNextConsumeTime()
    {
        return mNextConsumeTime;
    }

    int32_t getConsumedTimes()
    {
        return mConsumedTimes;
    }


    friend class ConsumeMessageResponse;

protected:
    void initFromXml(const pugi::xml_node& messageNode);

protected:
    std::string mMessageId;
    std::string mReceiptHandle;
    std::string mMessageBody;
    std::string mMessageBodyMD5;
    std::string mMessageTag;
    int64_t mPublishTime;
    int64_t mNextConsumeTime;
    int64_t mFirstConsumeTime;
    int32_t mConsumedTimes;
};

class Request
{
public:
    Request(const std::string& method)
        : mMethod(method), mCanonicalizedResource(""), mRequestBody("")
    {}
    virtual ~Request() {};

    const std::string& getMethod() const
    {
        return mMethod;
    }

    const std::string& getCanonicalizedResource()
    {
        if (mCanonicalizedResource == "")
        {
            return generateCanonicalizedResource();
        }
        return mCanonicalizedResource;
    }

    const std::string& generateCanonicalizedResource()
    {
        mCanonicalizedResource = getResourcePath();
        if (getQueryString() != "")
        {
            mCanonicalizedResource += "?" + getQueryString();
        }
        return mCanonicalizedResource;
    }

    const std::string& getRequestBody()
    {
        if (mRequestBody == "")
        {
            return generateRequestBody();
        }
        return mRequestBody;
    }

    virtual std::string getQueryString() = 0;

    virtual std::string getResourcePath() = 0;

    virtual const std::string& generateRequestBody() = 0;

    const std::map<std::string, std::string>& getHeaders()
    {
        return mHeaders;
    }

    std::string getHeader(const std::string& key)
    {
		std::map<std::string, std::string>::iterator iter = mHeaders.find(key);
        if (iter != mHeaders.end())
        {
            return iter->second;
        }
        return "";
    }

    void setHeader(std::string key, std::string value)
    {
        mHeaders[key] = value;
    }

protected:
    std::string mMethod;
    std::string mCanonicalizedResource;
    std::string mRequestBody;
    std::map<std::string, std::string> mHeaders;
};

class Response
{
public:
    Response();
    virtual ~Response() {};

    int getStatus()
    {
        return mStatus;
    }

    void setStatus(int32_t status)
    {
        mStatus = status;
    }

    const std::map<std::string, std::string>& getHeaders()
    {
        return mHeaders;
    }

    void setHeader(std::string key, std::string value)
    {
        mHeaders[key] = value;
    }

    std::string getHeader(std::string key)
    {
		std::map<std::string, std::string>::iterator iter = mHeaders.find(key);
        if (iter != mHeaders.end())
        {
            return iter->second;
        }
        return NULL;
    }

    std::string* getRawDataPtr()
    {
        return &mRawData;
    }

    void clearRawData()
    {
        mRawData.clear();
    }

    virtual bool isSuccess() = 0;
    virtual void parseResponse() = 0;

protected:
    pugi::xml_node toXML();
    void parseCommonError(const pugi::xml_node& rootNode);
    bool isCommonError(const pugi::xml_node& rootNode);

protected:
    pugi::xml_document mDoc;
    std::map<std::string, std::string> mHeaders;
    std::string mRawData;
    int32_t mStatus;
};

class ConsumeMessageRequest : public Request
{
public:
    ConsumeMessageRequest(
        const std::string& instanceId,
        const std::string& topicName,
        const std::string& consumer,
        const int32_t numOfMessages,
        const std::string& messageTag = "",
        const int32_t waitSeconds = -1);

    virtual ~ConsumeMessageRequest() {}

    std::string getQueryString();
    const std::string& generateRequestBody();
    std::string getResourcePath()
    {
        if (mTopicName == NULL)
            return "";
        return "/topics/" + *mTopicName + "/messages";
    }

protected:
    const std::string* mInstanceId;
    const std::string* mTopicName;
    const std::string* mMessageTag;
    const std::string* mConsumer;
    const int32_t mNumOfMessages;
    const int32_t mWaitSeconds;
};

class ConsumeMessageResponse : public Response
{
public:
    ConsumeMessageResponse(std::vector<Message>& messages);
    virtual ~ConsumeMessageResponse() {}

    void parseResponse();
    bool isSuccess();

protected:
    std::vector<Message>* mMessages;
};

class AckMessageRequest : public Request
{
public:
    AckMessageRequest(const std::string& instanceId, 
            const std::string& topicName, 
            const std::string& consumer,
            const std::vector<std::string>& receiptHandles);
    virtual ~AckMessageRequest() {}

    std::string getQueryString();
    const std::string& generateRequestBody();


    std::string getResourcePath()
    {
        if (mTopicName == NULL)
            return "";
        return "/topics/" + *mTopicName + "/messages";
    }

protected:
    const std::string* mInstanceId;
    const std::string* mTopicName;
    const std::string* mConsumer;
    const std::vector<std::string>* mReceiptHandles;
};

class AckMessageResponse : public Response
{
public:
    void parseResponse();
    bool isSuccess();

    const std::vector<AckMessageFailedItem>& getAckMessageFailedItem()
    {
        return mAckMessageFailedItems;
    }

protected:
    std::vector<AckMessageFailedItem> mAckMessageFailedItems;
};

class PublishMessageRequest : public Request
{
public:
    PublishMessageRequest(const std::string& instanceId,
                          const std::string& topicName,
                          const std::string& messageBody);

    PublishMessageRequest(const std::string& instanceId,
                          const std::string& topicName,
                          const std::string& messageBody,
                          const std::string& messageTag);

    virtual ~PublishMessageRequest() {}

    std::string getQueryString();
    const std::string& generateRequestBody();


    std::string getResourcePath()
    {
        if (mTopicName == NULL)
            return "";
        return "/topics/" + *mTopicName + "/messages";
    }

protected:
    const std::string* mInstanceId;
    const std::string* mTopicName;
    const std::string* mMessageBody;
    const std::string* mMessageTag;
};

class PublishMessageResponse : public Response
{
public:
    PublishMessageResponse();
    virtual ~PublishMessageResponse() {}

    void parseResponse();
    bool isSuccess();

    std::string getMessageId()
    {
        return mMessageId;
    }

    std::string getMessageBodyMD5()
    {
        return mMessageBodyMD5;
    }

protected:
    std::string mMessageId;
    std::string mMessageBodyMD5;
};

}
}
}

#endif
