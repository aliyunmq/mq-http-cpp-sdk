// Copyright (C) 2019, Alibaba Cloud Computing

#ifndef MQ_CLIENT_H
#define MQ_CLIENT_H

#include "mq_protocol.h"
#include "mq_network_tool.h"
#include <map>
#include <stdint.h>
#include <vector>

#ifdef _WIN32
#include <memory>
#else
#ifdef __APPLE__
#include <memory>
#else
#include <tr1/memory>
#endif
#endif

namespace mq
{
namespace http
{
namespace sdk
{

class MQConsumer;
class MQProducer;
class MQTransProducer;
class MQClient;

#ifdef __APPLE__
typedef std::shared_ptr<MQConsumer> MQConsumerPtr;
typedef std::shared_ptr<MQProducer> MQProducerPtr;
typedef std::shared_ptr<MQTransProducer> MQTransProducerPtr;
#else
typedef std::tr1::shared_ptr<MQConsumer> MQConsumerPtr;
typedef std::tr1::shared_ptr<MQProducer> MQProducerPtr;
typedef std::tr1::shared_ptr<MQTransProducer> MQTransProducerPtr;
#endif

/*
 * use MQClient to access mq service through http
 *
 * CAUTION:
 *     Make sure to catch Exception when calling any function
 *     throws MQServerException when the request fails.
 *     throws MQExceptionBase for client errors
 */
class MQClient
{
public:
    /* init the MQClient for calling mq service
     *
     * @param endpoint:
     *      http://{AccountId}.mq.cn-hangzhou.aliyuncs.com
     *      "cn-hangzhou" is the region place
     * @param accessId:  accessId from aliyun.com
     * @param accessKey: accessKey from aliyun.com
     * @param connPoolSize:
     *      MQClient keeps a pool of connections and reuse them
     */
    MQClient(const std::string& endpoint,
              const std::string& accessId,
              const std::string& accessKey,
              const int32_t connPoolSize = 200,
              const int32_t timeout = 35,
              const int32_t connectTimeout = 35);

    /* init the MQClient for calling mq service
    *
    * @param endpoint:
    *      http://{AccountId}.mq.cn-hangzhou.aliyuncs.com
    *      "cn-hangzhou" is the region place
    * @param accessId:  the sts accessId
    * @param accessKey: the sts accessKey
    * @param stsToken:  the sts token
    * @param connPoolSize:
    *      MQClient keeps a pool of connections and reuse them
    */
    MQClient(const std::string& endpoint,
              const std::string& accessId,
              const std::string& accessKey,
              const std::string& stsToken,
              const int32_t connPoolSize = 200,
              const int32_t timeout = 35,
              const int32_t connectTimeout = 35);

    /* update the AccessId/AccessKey
    *
    * @param accessId: accessId from aliyun.com
    * @param accessKey: accessKey from aliyun.com
    */
    void updateAccessId(const std::string& accessId,
                        const std::string& accessKey);

    /* update the AccessId/AccessKey/StsToken
    *
    * @param accessId:  the sts accessId
    * @param accessKey: the sts accessKey
    * @param stsToken:  the sts token
    */
    void updateAccessId(const std::string& accessId,
                        const std::string& accessKey,
                        const std::string& stsToken);

    virtual ~MQClient() {}

    /* init MQConsumer instance for consume message
     *
     * @param topicName: the topic name
     * @param consumer: the consumer group name
     * @param messageTag: filter messages by this
     *
     * @return: the inited MQConsumer instance
     */
    MQConsumerPtr getConsumerRef(const std::string& topicName, const std::string& consumer, const std::string& messageTag);

     /* init MQConsumer instance for consume message
     *
     * @param instanceId: the instance id
     * @param topicName: the topic name
     * @param consumer: the consumer group name
     * @param messageTag: filter messages by this
     *
     * @return: the inited MQConsumer instance
     */
    MQConsumerPtr getConsumerRef(const std::string& instanceId, const std::string& topicName, const std::string& consumer, const std::string& messageTag);
    
    /* init MQConsumer instance for consume message
     *
     * @param topicName: the topic name
     * @param consumer: the consumer group name
     *
     * @return: the inited MQConsumer instance
     */
    MQConsumerPtr getConsumerRef(const std::string& topicName, const std::string& consumer);

    /* init MQProduer instance for publish message
     *
     * @param topicName: the topic name
     *
     * @return: the inited MQProducer instance
     */
    MQProducerPtr getProducerRef(const std::string& topicName);

    /* 
     * init MQProduer instance for publish message
     *
     * @param instanceId: the instance id
     * @param topicName: the topic name
     *
     * @return: the inited MQProducer instance
     */
    MQProducerPtr getProducerRef(const std::string& instanceId, const std::string& topicName);

    /*
     * int MQTransProducer instance
     *
     * @param topicName: the topic name
     * @param groupId: the group id
     */
    MQTransProducerPtr getTransProducerRef(const std::string& topicName, const std::string& groupId);

    /*
     * int MQTransProducer instance
     *
     * @param instanceId: the instance id
     * @param topicName: the topic name
     * @param groupId: the group id
     */
    MQTransProducerPtr getTransProducerRef(const std::string& instanceId, const std::string& topicName, const std::string& groupId);

    const std::string& getEndpoint() const
    {
        return mEndPoint;
    }
    const std::string& GetAccessId() const
    {
        return mAccessId;
    }
    const std::string& GetAccessKey() const
    {
        return mAccessKey;
    }
    const std::string& GetStsToken() const
    {
        return mStsToken;
    }

public:
    static void sendRequest(Request& req,
                            Response& response,
                            const std::string& endpoint,
                            const std::string& accessId,
                            const std::string& accessKey,
                            const std::string& stsToken,
                            MQConnectionToolPtr mqConnTool);

    static void signRequest(Request& req,
                            const std::string& endpoint,
                            const std::string& accessId,
                            const std::string& accessKey,
                            const std::string& stsToken);

protected:
    std::string mEndPoint;
    std::string mAccessId;
    std::string mAccessKey;
    std::string mStsToken;
    MQConnectionToolPtr mMQConnTool;
};

/*
 *
 * CAUTION:
 *     Make sure to catch Exception when calling any function
 *     throws MQServerException when the request is failed.
 *     throws MQExceptionBase for client errors
 */
class MQConsumer
{
public:
    virtual ~MQConsumer() {}

    /* update the AccessId/AccessKey
    *
    * @param accessId: accessId from aliyun.com
    * @param accessKey: accessKey from aliyun.com
    */
    void updateAccessId(const std::string& accessId,
                        const std::string& accessKey);

    /* update the AccessId/AccessKey/StsToken
    *
    * @param accessId:  the sts accessId
    * @param accessKey: the sts accessKey
    * @param stsToken:  the sts token
    */
    void updateAccessId(const std::string& accessId,
                        const std::string& accessKey,
                        const std::string& stsToken);

    const std::string& getInstanceId()
    {
        return mInstanceId;
    }

    const std::string& getTopicName()
    {
        return mTopicName;
    }

    const std::string& getConsumer()
    {
        return mConsumer;
    }

    const std::string& getMessageTag()
    {
        return mMessageTag;
    }

    /* consume messages,
     *    the message is changed to be reconsumed,if not AckMessage in 5 minute. 
     *
     * @param numOfMessages: the batch size
     * @param messages: received messages
     */
    void consumeMessage(const int32_t numOfMessages,
                             std::vector<Message>& messages);

    /* consume messages,
     *    the message is changed to be reconsumed,if not AckMessage in 5 minute. 
     *
     * @param numOfMessages: the batch size
     * @param waitSeconds:
     *     if no message to consume, the request will block for
     *         "waitSeconds" util timeout or any message coming.
     * @param messages: received messages
     */
    void consumeMessage(const int32_t numOfMessages,
                             const int32_t waitSeconds,
                             std::vector<Message>& messages);

    /* ack messages: disable them to be reconsumed
     *    after consume the message, a ReceiptHandle is returned in Message
     *    this ReceiptHandle is valid in 5 minute.
     *    this ReceiptHandle could be used to ack the message
     *
     * @param receiptHandles: the ReceiptHandles
     */
    void ackMessage(const std::vector<std::string>& receiptHandles,
                            AckMessageResponse& resp);

    friend class MQClient;

protected:
    MQConsumer(const std::string& instanceId,
            const std::string& topicName,
          const std::string& consumer,
          const std::string& messageTag,
          const std::string& endpoint,
          const std::string& accessId,
          const std::string& accessKey,
          const std::string& stsToken,
          MQConnectionToolPtr mqConnTool);

protected:
    std::string mInstanceId;
    std::string mTopicName;
    std::string mConsumer;
    std::string mMessageTag;
    std::string mEndPoint;
    std::string mAccessId;
    std::string mAccessKey;
    std::string mStsToken;
    MQConnectionToolPtr mMQConnTool;
};

/*
 *
 * CAUTION:
 *     Make sure to catch Exception when calling any function
 *     throws MQServerException when the request is failed.
 *     throws MQExceptionBase for client errors
 */
class MQProducer
{
public:
    virtual ~MQProducer() {}

    /* update the AccessId/AccessKey
    *
    * @param accessId: accessId from aliyun.com
    * @param accessKey: accessKey from aliyun.com
    */
    void updateAccessId(const std::string& accessId,
                        const std::string& accessKey);

    /* update the AccessId/AccessKey/StsToken
    *
    * @param accessId:  the sts accessId
    * @param accessKey: the sts accessKey
    * @param stsToken:  the sts token
    */
    void updateAccessId(const std::string& accessId,
                        const std::string& accessKey,
                        const std::string& stsToken);

    const std::string& getTopicName()
    {
        return mTopicName;
    }

    const std::string& getInstanceId()
    {
        return mInstanceId;
    }

    /* publish one message
     *
     * @param messageBody: the message body
     * @param resp: the Response containing MessageId and BodyMD5
     */
    void publishMessage(const std::string& messageBody,
                        PublishMessageResponse& resp);

    /* publish one message with messageTag
     *
     * @param messageBody: the message body
     * @param messageTag: the message Tag
     * @param resp: the Response containing MessageId and BodyMD5
     */
    void publishMessage(const std::string& messageBody,
                        const std::string& messageTag,
                        PublishMessageResponse& resp);

    /* publish message
     *
     * @param topicMessage: the message
     * @param resp: the Response containing MessageId and BodyMD5
     */
    void publishMessage(TopicMessage& topicMessage,
                        PublishMessageResponse& resp);

    friend class MQClient;

protected:
    MQProducer(const std::string& instanceId,
            const std::string& topicName,
          const std::string& endpoint,
          const std::string& accessId,
          const std::string& accessKey,
          const std::string& stsToken,
          MQConnectionToolPtr mqConnTool);

protected:
    std::string mInstanceId;
    std::string mTopicName;
    std::string mEndPoint;
    std::string mAccessId;
    std::string mAccessKey;
    std::string mStsToken;
    MQConnectionToolPtr mMQConnTool;
};

/*
 *
 * CAUTION:
 *     Make sure to catch Exception when calling any function
 *     throws MQServerException when the request is failed.
 *     throws MQExceptionBase for client errors
 */
class MQTransProducer : public MQProducer
{
    public:

        /* 
         * consume half message to check transaction status, three choice: {@link #commit(String)} , {@link #rollback(String)}
         * or do nothing (after 10s will get the message again).
         *
         * @param numOfMessages: the batch size, value: 1~16
         * @param waitSeconds: value: 0~30s, 0 is not long polling.
         *     if no message to consume, the request will block for
         *         "waitSeconds" util timeout or any message coming.
         * @param messages: received messages
         */
        void consumeHalfMessage(const int32_t numOfMessages,
                const int32_t waitSeconds,
                std::vector<Message>& messages);

        /* 
         * commit transaction msg, the consumer will receive the msg.
         *
         * @param receiptHandle: the msg handle
         */
        void commit(const std::string& receiptHandle,
                AckMessageResponse& resp);

        /* 
         * rollback transaction msg, the consumer will not receive the msg.
         *
         * @param receiptHandle: the msg handle
         */
        void rollback(const std::string& receiptHandle,
                AckMessageResponse& resp);


        const std::string& getGroupId()
        {
            return mGroupId;
        }

        friend class MQClient;
    protected:
        MQTransProducer(const std::string& instanceId,
                const std::string& topicName,
                const std::string& groupId,
                const std::string& endpoint,
                const std::string& accessId,
                const std::string& accessKey,
                const std::string& stsToken,
                MQConnectionToolPtr mqConnTool);
    protected:
        std::string mGroupId;
};

}
}
}

#endif
