# Aliyun Message Queue(MQ) C++ HTTP SDK

Alyun MQ Documents: http://www.aliyun.com/product/ons

Aliyun MQ Console: https://ons.console.aliyun.com  

## Prerequisites:

1. VS2015 for windows, libcurl-devel&openssl-devel for Linux

2. scons for both windows and linux (http://scons.org/pages/download.html)

## HOW TO COMPILE:    
1. change dir to mq-http-cpp-sdk
2. scons or (scons debug=1, to build debug version for windows)
3. now you could find the headers in the "include" dir and library in "lib" dir
4. copy "include" and "lib" to your project


## Note
1. Http consumer only support timer msg (less than 3 days), no matter the msg is produced from http or tcp protocol.
2. Order is only supported at special server cluster.

## Sample (github)

[Publish Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/producer.cpp)

[Consume Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/consumer.cpp)

[Transaction Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/trans_producer.cpp)

[Publish Order Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/order_producer.cpp)

[Consume Order Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/order_consumer.cpp)

### Sample (code.aliyun.com)

[Publish Message](https://code.aliyun.com/aliware_rocketmq/mq-http-samples/blob/master/cpp/producer.cpp)

[Consume Message](https://code.aliyun.com/aliware_rocketmq/mq-http-samples/blob/master/cpp/consumer.cpp)

[Transaction Message](https://code.aliyun.com/aliware_rocketmq/mq-http-samples/blob/master/cpp/trans_producer.cpp)

[Publish Order Message](https://code.aliyun.com/aliware_rocketmq/mq-http-samples/blob/master/cpp/order_producer.cpp)

[Consume Order Message](https://code.aliyun.com/aliware_rocketmq/mq-http-samples/blob/master/cpp/order_consumer.cpp)