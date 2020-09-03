// Copyright (C) 2019, Alibaba Cloud Computing

#ifndef MQ_CONSTANTS_H
#define MQ_CONSTANTS_H

namespace mq
{
namespace http
{
namespace sdk
{

const char* const GET = "GET";
const char* const PUT = "PUT";
const char* const POST = "POST";
const char* const DELETE_METHOD = "DELETE";

const char* const HOST = "Host";
const char* const CURRENT_VERSION = "2015-06-06";
const char* const CONTENT_TYPE = "Content-Type";
const char* const CONTENT_LENGTH = "Content-Length";
const char* const SECURITY_TOKEN = "security-token";
const char* const DATE = "Date";
const char* const MQ_VERSION = "x-mq-version";
const char* const AUTHORIZATION = "Authorization";
const char* const CONTENT_MD5 = "Content-MD5";
const char* const CANONICALIZED_MQ_HEADER_PREFIX = "x-mq-";
const char* const MQ_XML_NAMESPACE_V1 = "http://mq.aliyuncs.com/doc/v1";
const char* const DEFAULT_CONTENT_TYPE = "text/xml;charset=UTF-8";

//Error code
const char* const TOPIC_NOT_EXIST = "TopicNotExist";
const char* const SUBSCRIPTION_NOT_EXIST = "SubscriptionNotExist";
const char* const STATE_CONFLICT = "StateConflict";
const char* const REQUEST_TIMEOUT = "RequestTimeout";

const char* const ERROR_TAG = "Error";
const char* const MESSAGE = "Message";
const char* const REQUEST_ID = "RequestId";
const char* const HOST_ID = "HostId";
const char* const MESSAGE_ID = "MessageId";
const char* const MESSAGE_BODY = "MessageBody";
const char* const MESSAGE_BODY_MD5 = "MessageBodyMD5";
const char* const MESSAGE_TAG = "MessageTag";
const char* const RECEIPT_HANDLE = "ReceiptHandle";
const char* const PUBLISH_TIME = "PublishTime";
const char* const FIRST_CONSUME_TIME = "FirstConsumeTime";
const char* const NEXT_CONSUME_TIME = "NextConsumeTime";
const char* const CONSUMED_TIMES = "ConsumedTimes";
const char* const MESSAGE_PROPERTIES = "Properties";
const char* const CODE = "Code";
const char* const ERROR_MESSAGE = "ErrorMessage";
const char* const ERROR_CODE = "ErrorCode";

const char* const MESSAGE_PROP_KEY = "KEYS";
const char* const MESSAGE_PROP_TRANS_CHECK = "__TransCheckT";
const char* const MESSAGE_PROP_TIMER = "__STARTDELIVERTIME";
const char* const MESSAGE_PROP_SHARDING = "__SHARDINGKEY";

const char* const AGENT = "mq-cpp-sdk/1.0.3(curl)";

const std::string EMPTY = "";

}
}
}

#endif
