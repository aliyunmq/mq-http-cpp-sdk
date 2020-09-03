#include "mq_client.h"
#include "mq_utils.h"
#include "mq_network_tool.h"
#include "mq_common_tool.h"
#include "constants.h"

#ifdef _WIN32
#include "curl-win/curl.h"
#else
#include <curl/curl.h>
#endif

#include <sstream>
#include <ctime>
#include <iostream>

using namespace std;
using namespace mq::http::sdk;

class MQGlobalFlagInit
{
public:
    MQGlobalFlagInit()
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    ~MQGlobalFlagInit()
    {
        curl_global_cleanup();
    }
};
static MQGlobalFlagInit sMQInit;

MQClient::MQClient(const string& endpoint,
                     const string& accessId,
                     const string& accessKey,
                     const int32_t connPoolSize,
                     const int32_t timeout,
                     const int32_t connectTimeout)
    : mAccessId(accessId)
    , mAccessKey(accessKey)
    , mStsToken("")
{
    mMQConnTool.reset(new MQConnectionTool(connPoolSize, connectTimeout, timeout));

    mEndPoint = StringTool::RightTrimString(endpoint);
    mEndPoint = StringTool::RightTrimString(mEndPoint, '/');
}

MQClient::MQClient(const std::string& endpoint,
          const std::string& accessId,
          const std::string& accessKey,
          const std::string& stsToken,
          const int32_t connPoolSize,
          const int32_t timeout,
          const int32_t connectTimeout)
    : mAccessId(accessId)
    , mAccessKey(accessKey)
    , mStsToken(stsToken)
{
    mMQConnTool.reset(new MQConnectionTool(connPoolSize, connectTimeout, timeout));

    mEndPoint = StringTool::RightTrimString(endpoint);
    mEndPoint = StringTool::RightTrimString(mEndPoint, '/');
}

void MQClient::updateAccessId(const std::string& accessId,
                               const std::string& accessKey)
{
    mAccessId = accessId;
    mAccessKey = accessKey;
    mStsToken = "";
}

void MQClient::updateAccessId(const std::string& accessId,
                               const std::string& accessKey,
                               const std::string& stsToken)
{
    mAccessId = accessId;
    mAccessKey = accessKey;
    mStsToken = stsToken;
}

void MQClient::sendRequest(Request& request,
                            Response& response,
                            const std::string& endpoint,
                            const std::string& accessId,
                            const std::string& accessKey,
                            const std::string& stsToken,
                            MQConnectionToolPtr mqConnTool)
{
    MQClient::signRequest(request, endpoint, accessId, accessKey, stsToken);
    MQNetworkTool::SendRequest(endpoint,
                                request,
                                response,
                                mqConnTool);
}

void MQClient::signRequest(Request& req,
                            const std::string& endpoint,
                            const std::string& accessId,
                            const std::string& accessKey,
                            const std::string& stsToken)
{
    if (stsToken != "")
    {
        req.setHeader(SECURITY_TOKEN, stsToken);
    }
    const std::string& canonicalizedResource = req.generateCanonicalizedResource();
    const std::string& requestBody = req.generateRequestBody();
    size_t contentLength = strlen(requestBody.c_str());

    size_t pos = endpoint.find_first_of("//");
    req.setHeader(HOST, endpoint.substr(pos + 2));
    req.setHeader(CONTENT_TYPE, DEFAULT_CONTENT_TYPE);
    req.setHeader(DATE, TimeTool::GetDateTime());
    req.setHeader(MQ_VERSION, CURRENT_VERSION);
    if (contentLength > 0)
        req.setHeader(CONTENT_LENGTH, StringTool::ToString(contentLength));
    req.setHeader(AUTHORIZATION,
        MQNetworkTool::Signature(req.getMethod(), canonicalizedResource,
            accessId, accessKey, req.getHeaders()));

}

MQConsumerPtr MQClient::getConsumerRef(const std::string& instanceId, const std::string& topicName, const std::string& consumer, const std::string& messageTag)
{
    std::string encodeTag;
    MQUtils::urlEncode(messageTag, encodeTag);

    return MQConsumerPtr(new MQConsumer(instanceId, topicName, consumer, encodeTag, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool));
}

MQConsumerPtr MQClient::getConsumerRef(const std::string& topicName, const std::string& consumer, const std::string& messageTag)
{
    std::string encodeTag;
    MQUtils::urlEncode(messageTag, encodeTag);

    return MQConsumerPtr(new MQConsumer(EMPTY, topicName, consumer, encodeTag, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool));
}

MQConsumerPtr MQClient::getConsumerRef(const std::string& topicName, const std::string& consumer)
{
    return MQConsumerPtr(new MQConsumer(EMPTY, topicName, consumer, EMPTY, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool));
}

MQProducerPtr MQClient::getProducerRef(const std::string& instanceId, const std::string& topicName)
{
    return MQProducerPtr(new MQProducer(instanceId, topicName, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool));
}

MQProducerPtr MQClient::getProducerRef(const std::string& topicName)
{
    return MQProducerPtr(new MQProducer(EMPTY, topicName, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool));
}

MQTransProducerPtr MQClient::getTransProducerRef(const std::string& topicName, const std::string& groupId)
{
    return MQTransProducerPtr(new MQTransProducer(EMPTY, topicName, groupId, mEndPoint, mAccessId, 
                mAccessKey, mStsToken, mMQConnTool));
}


MQTransProducerPtr MQClient::getTransProducerRef(const std::string& instanceId, const std::string& topicName, const std::string& groupId)
{
    return MQTransProducerPtr(new MQTransProducer(instanceId, topicName, groupId, mEndPoint, mAccessId, 
                mAccessKey, mStsToken, mMQConnTool));
}

MQConsumer::MQConsumer(const std::string& instanceId,
      const std::string& topicName,
      const std::string& consumer,
      const std::string& messageTag,
      const std::string& endpoint,
      const std::string& accessId,
      const std::string& accessKey,
      const std::string& stsToken,
      MQConnectionToolPtr mqConnTool)
    : mInstanceId(instanceId)
    , mTopicName(topicName)
    , mConsumer(consumer)
    , mMessageTag(messageTag)
    , mEndPoint(endpoint)
    , mAccessId(accessId)
    , mAccessKey(accessKey)
    , mStsToken(stsToken)
    , mMQConnTool(mqConnTool)
{
}

void MQConsumer::consumeMessage(const int32_t numOfMessages,
                                std::vector<Message>& messages)
{
    ConsumeMessageRequest req(mInstanceId, mTopicName, mConsumer, numOfMessages, mMessageTag, -1);
    ConsumeMessageResponse resp(messages);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQConsumer::consumeMessageOrderly(const int32_t numOfMessages,
                                std::vector<Message>& messages)
{
    ConsumeMessageRequest req(mInstanceId, mTopicName, mConsumer, numOfMessages, mMessageTag, -1);
    req.setOrderConsume();

    ConsumeMessageResponse resp(messages);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQConsumer::consumeMessage(const int32_t numOfMessages,
                                const int32_t waitSeconds,
                                std::vector<Message>& messages)
{
    ConsumeMessageRequest req(mInstanceId, mTopicName, mConsumer, numOfMessages, mMessageTag, waitSeconds);
    ConsumeMessageResponse resp(messages);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQConsumer::consumeMessageOrderly(const int32_t numOfMessages,
                                const int32_t waitSeconds,
                                std::vector<Message>& messages)
{
    ConsumeMessageRequest req(mInstanceId, mTopicName, mConsumer, numOfMessages, mMessageTag, waitSeconds);
    req.setOrderConsume();

    ConsumeMessageResponse resp(messages);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQConsumer::ackMessage(const std::vector<std::string>& receiptHandles,
                              AckMessageResponse& resp)
{
    AckMessageRequest req(mInstanceId, mTopicName, mConsumer, receiptHandles);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

MQProducer::MQProducer(const std::string& instanceId,
             const std::string& topicName,
             const std::string& endpoint,
             const std::string& accessId,
             const std::string& accessKey,
             const std::string& stsToken,
             MQConnectionToolPtr mqConnTool)
    : mInstanceId(instanceId)
    , mTopicName(topicName)
    , mEndPoint(endpoint)
    , mAccessId(accessId)
    , mAccessKey(accessKey)
    , mStsToken(stsToken)
    , mMQConnTool(mqConnTool)
{
}

void MQProducer::updateAccessId(const std::string& accessId,
                           const std::string& accessKey)
{
    mAccessId = accessId;
    mAccessKey = accessKey;
    mStsToken = "";
}

void MQProducer::updateAccessId(const std::string& accessId,
                           const std::string& accessKey,
                           const std::string& stsToken)
{
    mAccessId = accessId;
    mAccessKey = accessKey;
    mStsToken = stsToken;
}

void MQProducer::publishMessage(const std::string& messageBody,
                           PublishMessageResponse& resp)
{
    PublishMessageRequest req(mInstanceId, mTopicName, messageBody, EMPTY);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQProducer::publishMessage(const std::string& messageBody,
                           const std::string& messageTag,
                           PublishMessageResponse& resp)
{
    PublishMessageRequest req(mInstanceId, mTopicName, messageBody, messageTag);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQProducer::publishMessage(TopicMessage& topicMessage, PublishMessageResponse& resp)
{
    PublishMessageRequest req(mInstanceId, mTopicName, topicMessage.mMessageBody, topicMessage.mMessageTag);
    MQUtils::mapToString(topicMessage.mProperties, req.mProperties);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

MQTransProducer::MQTransProducer(const std::string& instanceId,
             const std::string& topicName,
             const std::string& groupId,
             const std::string& endpoint,
             const std::string& accessId,
             const std::string& accessKey,
             const std::string& stsToken,
             MQConnectionToolPtr mqConnTool)
    : MQProducer(instanceId, topicName, endpoint, accessId, accessKey, stsToken, mqConnTool) 
    , mGroupId(groupId)
{
}

void MQTransProducer::consumeHalfMessage(const int32_t numOfMessages,
        const int32_t waitSeconds,
        std::vector<Message>& messages)
{
    ConsumeMessageRequest req(mInstanceId, mTopicName, mGroupId, numOfMessages, EMPTY, waitSeconds);
    req.setTransConsume();

    ConsumeMessageResponse resp(messages);
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
            mAccessKey, mStsToken, mMQConnTool);
}

void MQTransProducer::commit(const std::string& receiptHandle,
        AckMessageResponse& resp)
{
    std::vector<std::string> receiptHandles;
    receiptHandles.push_back(receiptHandle);

    AckMessageRequest req(mInstanceId, mTopicName, mGroupId, receiptHandles);
    req.setTransCommit();
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);
}

void MQTransProducer::rollback(const std::string& receiptHandle,
                AckMessageResponse& resp)
{
    std::vector<std::string> receiptHandles;
    receiptHandles.push_back(receiptHandle);

    AckMessageRequest req(mInstanceId, mTopicName, mGroupId, receiptHandles);
    req.setTransRollback();
    MQClient::sendRequest(req, resp, mEndPoint, mAccessId,
        mAccessKey, mStsToken, mMQConnTool);

}
