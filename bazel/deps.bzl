load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def deps():
    maybe(
        http_archive,
        name = "com_github_curl",
        build_file = "//third_party:curl.BUILD",
        strip_prefix = "curl-curl-7_79_1",
        sha256 = "f4ce5f374f4e6ca0dd3aed3178ac49f18905988f250eaa77f8cb9dbc0535f254",
        urls = [
            "https://ons-client-sdk.oss-cn-hangzhou.aliyuncs.com/ons-client-cpp/3rd_party/curl-curl-7_79_1.tar.gz",
            "https://github.com/curl/curl/archive/refs/tags/curl-7_79_1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "openssl",
        url = "https://github.com/openssl/openssl/archive/OpenSSL_1_1_0h.tar.gz",
        sha256 = "f56dd7d81ce8d3e395f83285bd700a1098ed5a4cb0a81ce9522e41e6db7e0389",
        strip_prefix = "openssl-OpenSSL_1_1_0h",
        build_file = "@mq-http-cpp-sdk//third_party:openssl.BUILD",
    )

    maybe(
        http_archive,
        name = "rules_pkg",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.5.1/rules_pkg-0.5.1.tar.gz",
            "https://github.com/bazelbuild/rules_pkg/releases/download/0.5.1/rules_pkg-0.5.1.tar.gz",
        ],
        sha256 = "a89e203d3cf264e564fcb96b6e06dd70bc0557356eb48400ce4b5d97c2c3720d",
    )

    maybe(
        http_archive,
        name = "rules_python",
        urls = [
            "https://github.com/bazelbuild/rules_python/releases/download/0.5.0/rules_python-0.5.0.tar.gz",
        ],
        sha256 = "cd6730ed53a002c56ce4e2f396ba3b3be262fd7cb68339f0377a45e8227fe332",
    )