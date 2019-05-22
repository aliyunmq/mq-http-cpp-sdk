#include "mq_protocol.h"
#include "mq_utils.h"
#include "mq_exception.h"
#include "mq_network_tool.h"
#include "mq_common_tool.h"
#include "mq_exception.h"
#include "constants.h"
#include "mq_utils.h"

#include <iostream>

using namespace std;
using namespace pugi;
using namespace mq::http::sdk;

void Message::initFromXml(const xml_node& messageNode)
{
    xml_node iterNode = messageNode.first_child();
    while (!iterNode.empty())
    {
        if (iterNode.type() != node_element)
        {
            continue;
        }
        const pugi::char_t* name = iterNode.name();
        if (0 == strcmp(MESSAGE_ID, name))
        {
            mMessageId = iterNode.text().get();
        }
        else if (0 == strcmp(RECEIPT_HANDLE, name))
        {
            mReceiptHandle = iterNode.text().get();
        }
        else if (0 == strcmp(MESSAGE_BODY, name))
        {
            mMessageBody = iterNode.text().get();
        }
        else if (0 == strcmp(MESSAGE_BODY_MD5, name))
        {
            mMessageBodyMD5 = iterNode.text().get();
        }
        else if (0 == strcmp(MESSAGE_TAG, name))
        {
            mMessageTag = iterNode.text().get();
        }
        else if (0 == strcmp(PUBLISH_TIME, name))
        {
            mPublishTime = atol(iterNode.text().get());
        }
        else if (0 == strcmp(FIRST_CONSUME_TIME, name))
        {
            mFirstConsumeTime = atol(iterNode.text().get());
        }
        else if (0 == strcmp(NEXT_CONSUME_TIME, name))
        {
            mNextConsumeTime = atol(iterNode.text().get());
        }
        else if (0 == strcmp(CONSUMED_TIMES, name))
        {
            mConsumedTimes = atoi(iterNode.text().get());
        }
        else if (0 == strcmp(MESSAGE_PROPERTIES, name))
        {
            MQUtils::stringToMap(iterNode.text().get(), mProperties);
        }

        iterNode = iterNode.next_sibling();
    }
}

Response::Response()
    : mRawData(""), mStatus(0)
{
}

bool Response::isCommonError(const pugi::xml_node& rootNode)
{
    const pugi::char_t* name = rootNode.name();
    return 0 == strcmp(ERROR_TAG, name);
}

pugi::xml_node Response::toXML()
{
    mDoc.reset();
    pugi::xml_parse_result result = mDoc.load_string(mRawData.c_str());
    if (!result)
    {
        MQ_THROW(MQExceptionBase, mRawData);
    }
    xml_node iterNode = mDoc.first_child();
    while (!iterNode.empty())
    {
        if (iterNode.type() == node_element) {
            return iterNode;
        }

        iterNode = iterNode.next_sibling();
    }
    MQ_THROW(MQExceptionBase, mRawData);
}

void Response::parseCommonError(const pugi::xml_node& rootNode)
{
    if (!isCommonError(rootNode))
    {
        MQ_THROW(MQExceptionBase, "Unknown CommonError XML: " + mRawData);
    }

    ErrorInfo errorInfo;
    errorInfo.httpStatus = mStatus;

    xml_node iterNode = rootNode.first_child();
    while (!iterNode.empty())
    {
        if (iterNode.type() != node_element)
        {
            continue;
        }
        const pugi::char_t* name = iterNode.name();
        if (0 == strcmp(CODE, name))
            errorInfo.code = iterNode.text().get();
        else if (0 == strcmp(MESSAGE, name))
            errorInfo.errorMessage = iterNode.text().get();
        else if (0 == strcmp(REQUEST_ID, name))
            errorInfo.requestId = iterNode.text().get();
        else if (0 == strcmp(HOST_ID, name))
            errorInfo.hostId = iterNode.text().get();

        iterNode = iterNode.next_sibling();
    }
    MQ_THROW(MQServerException, errorInfo);
}

ConsumeMessageResponse::ConsumeMessageResponse(
    std::vector<Message>& messages)
    : Response(), mMessages(&messages)
{
}

ConsumeMessageRequest::ConsumeMessageRequest(
    const std::string& instanceId,
    const std::string& topicName,
    const std::string& consumer,
    const int32_t numOfMessages,
    const std::string& messageTag,
    const int32_t waitSeconds)
    : Request("GET"), mInstanceId(&instanceId),
        mTopicName(&topicName), 
        mConsumer(&consumer),
        mNumOfMessages(numOfMessages),
        mMessageTag(&messageTag),
        mWaitSeconds(waitSeconds)
{
}

std::string ConsumeMessageRequest::getQueryString()
{
    string queryStr = "consumer=" + *mConsumer;
    queryStr += "&numOfMessages=" + StringTool::ToString(mNumOfMessages);
    if (mInstanceId != NULL && *mInstanceId != "")
    {
        queryStr += "&ns=" + *mInstanceId;
    }
    if (mWaitSeconds > 0)
    {
        queryStr += "&waitseconds=" + StringTool::ToString(mWaitSeconds);
    }
    if (mMessageTag != NULL && *mMessageTag != "")
    {
        queryStr += "&tag=" + *mMessageTag;
    }
    if (mTrans != "")
    {
        queryStr += "&trans=" + mTrans;
    }
    return queryStr;
}

const std::string& ConsumeMessageRequest::generateRequestBody()
{
    mRequestBody = "";
    return mRequestBody;
}

void ConsumeMessageResponse::parseResponse()
{
    pugi::xml_node rootNode = toXML();
    if (!isSuccess())
    {
        parseCommonError(rootNode);
        return;
    }

    int32_t index = 0;
    xml_node iterNode = rootNode.first_child();
    while (!iterNode.empty())
    {
        if (iterNode.type() != node_element)
        {
            continue;
        }
        const pugi::char_t* name = iterNode.name();
        if (0 == strcmp(MESSAGE, name))
        {
            Message message;
            mMessages->push_back(message);
            mMessages->at(index).initFromXml(iterNode);
            index += 1;
        }

        iterNode = iterNode.next_sibling();
    }
}

bool ConsumeMessageResponse::isSuccess()
{
    return mStatus == 200;
}

AckMessageRequest::AckMessageRequest(const std::string& instanceId,
    const std::string& topicName,
    const std::string& consumer,
    const std::vector<std::string>& receiptHandles)
    : Request("DELETE"), mInstanceId(&instanceId), mTopicName(&topicName), mConsumer(&consumer),
        mReceiptHandles(&receiptHandles)
{
}

std::string AckMessageRequest::getQueryString()
{
    string queryStr = "consumer=" + *mConsumer;
    if (mInstanceId != NULL && *mInstanceId != "")
    {
        queryStr += "&ns=" + *mInstanceId;
    }
    if (mTrans != "")
    {
        queryStr += "&trans=" + mTrans;
    }
    return queryStr;
}

const std::string& AckMessageRequest::generateRequestBody()
{
    pugi::xml_document doc;
    doc.load("<?xml version=\"1.0\" encoding=\"UTF-8\"?>", pugi::parse_declaration);
    pugi::xml_node node = doc.append_child("ReceiptHandles");
    node.append_attribute("xmlns") = MQ_XML_NAMESPACE_V1;

    for (std::vector<std::string>::const_iterator iter = mReceiptHandles->begin();
        iter != mReceiptHandles->end(); ++iter)
    {
        node.append_child("ReceiptHandle").append_child(
            pugi::node_pcdata).set_value(iter->c_str());
    }

    ostringstream os;
    doc.save(os);
    mRequestBody = os.str();
    return mRequestBody;
}

void AckMessageResponse::parseResponse()
{
    if (isSuccess())
    {
        return;
    }

    pugi::xml_node rootNode = toXML();

    if (isCommonError(rootNode))
    {
        parseCommonError(rootNode);
        return;
    }

    xml_node iterNode = rootNode.first_child();
    while (!iterNode.empty())
    {
        if (iterNode.type() != node_element)
        {
            continue;
        }
        const pugi::char_t* name = iterNode.name();
        if (0 == strcmp(ERROR_TAG, name))
        {
            AckMessageFailedItem failedItem;

            xml_node iterErrorNode = iterNode.first_child();
            while (!iterErrorNode.empty())
            {
                if (iterErrorNode.type() != node_element)
                {
                    continue;
                }
                const pugi::char_t* itemName = iterErrorNode.name();
                if (0 == strcmp(ERROR_MESSAGE, itemName))
                {
                    failedItem.errorMessage = iterErrorNode.text().get();
                }
                else if (0 == strcmp(ERROR_CODE, itemName))
                {
                    failedItem.errorCode = iterErrorNode.text().get();
                }
                else if (0 == strcmp(RECEIPT_HANDLE, itemName))
                {
                    failedItem.receiptHandle = iterErrorNode.text().get();
                }

                iterErrorNode = iterErrorNode.next_sibling();
            }
            mAckMessageFailedItems.push_back(failedItem);
        }

        iterNode = iterNode.next_sibling();
    }
}

bool AckMessageResponse::isSuccess()
{
    return mStatus == 204;
}

PublishMessageRequest::PublishMessageRequest(const std::string& instanceId,
                                             const std::string& topicName,
                                             const std::string& messageBody,
                                             const std::string& messageTag)
    : Request("POST")
    , mInstanceId(&instanceId)
    , mTopicName(&topicName)
    , mMessageBody(&messageBody)
    , mMessageTag(&messageTag)
{
}

string PublishMessageRequest::getQueryString()
{
    if (mInstanceId != NULL && *mInstanceId != "")
    {
        return "ns=" + *mInstanceId;
    }
    return "";
}

const string& PublishMessageRequest::generateRequestBody()
{
    pugi::xml_document doc;
    doc.load("<?xml version=\"1.0\" encoding=\"UTF-8\"?>", pugi::parse_declaration);
    pugi::xml_node node = doc.append_child("Message");
    node.append_attribute("xmlns") = MQ_XML_NAMESPACE_V1;

    node.append_child(MESSAGE_BODY).append_child(
        pugi::node_pcdata).set_value(mMessageBody->c_str());

    if (mMessageTag != NULL && *mMessageTag != "")
    {
        node.append_child(MESSAGE_TAG).append_child(
            pugi::node_pcdata).set_value(mMessageTag->c_str());
    }
    if (mProperties != "")
    {
        node.append_child(MESSAGE_PROPERTIES).append_child(
            pugi::node_pcdata).set_value(mProperties.c_str());
    }

    ostringstream os;
    doc.save(os);
    mRequestBody = os.str();
    return mRequestBody;
}

PublishMessageResponse::PublishMessageResponse()
    : mMessageId(""), mMessageBodyMD5("")
{
}

bool PublishMessageResponse::isSuccess()
{
    return mStatus == 201;
}

void PublishMessageResponse::parseResponse()
{
    pugi::xml_node rootNode = toXML();
    if (!isSuccess())
    {
        parseCommonError(rootNode);
        return;
    }

    xml_node iterNode = rootNode.first_child();
    while (!iterNode.empty())
    {
        if (iterNode.type() != node_element)
        {
            continue;
        }
        const pugi::char_t* name = iterNode.name();
        if (0 == strcmp(MESSAGE_ID, name))
        {
            mMessageId = iterNode.text().get();
        }
        else if (0 == strcmp(MESSAGE_BODY_MD5, name))
        {
            mMessageBodyMD5 = iterNode.text().get();
        }
        else if (0 == strcmp(RECEIPT_HANDLE, name))
        {
            mReceiptHandle = iterNode.text().get();
        }

        iterNode = iterNode.next_sibling();
    }
}
