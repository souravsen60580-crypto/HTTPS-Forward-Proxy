cmake_minimum_required(VERSION 3.16)

project(cpp_proxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Main Proxy Executable
# ============================================================

add_executable(
    cpp-proxy

    src/main.cpp

    # Server
    src/server/tcp_server.cpp
    src/server/client_handler.cpp

    # HTTP
    src/http/request.cpp
    src/http/response.cpp
    src/http/parser.cpp

    # Proxy
    src/proxy/http_proxy.cpp
    src/proxy/https_proxy.cpp
    src/proxy/tunnel.cpp

    # Network
    src/network/socket.cpp
    src/network/dns.cpp
    src/network/connection.cpp

    # Security
    src/security/policy_engine.cpp

    # Cache
    src/cache/cache.cpp

    # Logging
    src/logging/logger.cpp
)


# ============================================================
# Include Directory
# ============================================================

target_include_directories(
    cpp-proxy
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)


# ============================================================
# Testing
# ============================================================

enable_testing()

add_executable(
    test_parser

    tests/test_parser.cpp

    src/http/request.cpp
    src/http/parser.cpp
)


target_include_directories(
    test_parser
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)


add_test(
    NAME parser
    COMMAND test_parser
)