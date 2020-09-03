// Copyright (C) 2019, Alibaba Cloud Computing

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
#include "constants.h"

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

class TopicMessage
{
    public:
        TopicMessage(const std::string& messageBody)
            : mMessageBody(messageBody), mMessageTag("")
        {
        }

        TopicMessage(const std::string& messageBody,
                const std::string& messageTag)
            : mMessageBody(messageBody), mMessageTag(messageTag)
        {
        }

        void putProperty(const std::string key, const std::string value)
        {
            if (key.length() == 0 || value.length() == 0)
            {
                return;
            }
            mProperties[key] = value;
        }

        /**
         * Timer message, msg will be consumed after this time.
         *
         * @param time
         */
        void setStartDeliverTime(int64_t time)
        {
            mProperties[MESSAGE_PROP_TIMER] = std::to_string(time);
        }

        /**
         * Transaction message, the first checked time.
         *
         * @param seconds
         */
        void setTransCheckImmunityTime(int32_t seconds)
        {
            mProperties[MESSAGE_PROP_TRANS_CHECK] = std::to_string(seconds);
        }

        void setMessageKey(std::string k)
        {
            if (k.length() == 0)
            {
                return;
            }
            mProperties[MESSAGE_PROP_KEY] = k;
        }

        /**
         * Order message, select the shard according to key
         *
         * @param key: sharding key
         */
        void setShardingKey(std::string key)
        {
            if (key.length() == 0)
            {
                return;
            }
            mProperties[MESSAGE_PROP_SHARDING] = key;
        }

        friend class MQProducer;
    protected:
        const std::string mMessageBody;
        const std::string mMessageTag;
        std::map<std::string, std::string> mProperties;
};

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

    /**
     * it's meaningless for orderly consume
     */
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

    const std::map<std::string, std::string>& getProperties()
    {
        return mProperties;
    }

    const std::string& getProperty(const std::string& key)
    {
		std::map<std::string, std::string>::iterator iter = mProperties.find(key);
        if (iter != mProperties.end())
        {
            return iter->second;
        }
        return EMPTY;
    }

    const std::string getPropertiesAsString()
    {
        std::string o = "";
        for(std::map<std::string, std::string>::const_iterator it = mProperties.begin();
                    it != mProperties.end(); ++it)
        {
                o += it->first;
                o += ":";
                o += it->second;
                o += ";";
        }
        return o;
    }

    const std::string& getMessageKey()
    {
        return getProperty(MESSAGE_PROP_KEY);
    }

    const int32_t getTransCheckImmunityTime()
    {
        std::string value = getProperty(MESSAGE_PROP_TRANS_CHECK);
        if (value.length() == 0) 
        {
            return 0;
        }
        return atoi(value.c_str());
    }

    int64_t getStartDeliverTime()
    {
        std::string value = getProperty(MESSAGE_PROP_TIMER);
        if (value.length() == 0)
        {
            return 0;
        }
        return atoll(value.c_str());
    }

    const std::string& getShardingKey()
    {
        return getProperty(MESSAGE_PROP_SHARDING);
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
    std::map<std::string, std::string> mProperties;
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

    friend class MQTransProducer;
    friend class MQConsumer;

protected:
    void setTransConsume()
    {
        this->mTrans = "pop";
    }

    void setOrderConsume()
    {
        this->mTrans = "order";
    }

protected:
    const std::string* mInstanceId;
    const std::string* mTopicName;
    const std::string* mMessageTag;
    const std::string* mConsumer;
    const int32_t mNumOfMessages;
    const int32_t mWaitSeconds;
    std::string mTrans;
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

    friend class MQTransProducer;

protected:
    void setTransCommit()
    {
        this->mTrans = "commit";
    }

    void setTransRollback()
    {
        this->mTrans = "rollback";
    }

protected:
    const std::string* mInstanceId;
    const std::string* mTopicName;
    const std::string* mConsumer;
    const std::vector<std::string>* mReceiptHandles;
    std::string mTrans;
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

    friend class MQProducer;

protected:
    const std::string* mInstanceId;
    const std::string* mTopicName;
    const std::string* mMessageBody;
    const std::string* mMessageTag;
    std::string mProperties;
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

    /**
     * only transaction msg.
     */
    std::string getReceiptHandle()
    {
        return mReceiptHandle;
    }

protected:
    std::string mMessageId;
    std::string mMessageBodyMD5;
    std::string mReceiptHandle;
};

}
}
}

#endif
