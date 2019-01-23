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

## Sample

[Publish Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/producer.cpp)

[Consume Message](https://github.com/aliyunmq/mq-http-samples/blob/master/cpp/consumer.cpp)

