cat > README.md <<'EOF'
# C++ HTTP/HTTPS Forward Proxy

A modular HTTP/HTTPS forward proxy server built from scratch in C++17.

The project demonstrates TCP socket programming, HTTP request parsing, HTTPS CONNECT tunneling, DNS resolution, caching, domain-based security policies, multithreading, and thread-safe logging.

## Features

- HTTP forward proxy
- HTTPS proxy using CONNECT tunneling
- TCP socket programming
- HTTP request parsing
- DNS resolution using `getaddrinfo`
- Domain-based blocking policy
- Thread-per-client architecture
- Bidirectional HTTPS tunneling
- Thread-safe response cache with TTL
- Thread-safe logging
- CMake build system
- Parser unit test

## Architecture

```text
                    Internet
                       |
                       |
                +--------------+
                | Target Server|
                +--------------+
                       ^
                       |
                  TCP / HTTPS
                       |
+--------+       +-------------+
| Client | ----> | C++ Proxy   |
+--------+       +-------------+
                       |
              +--------+--------+
              |        |        |
           Parser    Cache    Policy
              |        |        |
              +--------+--------+
                       |
                 Network Layer# HTTPS-Forward-Proxy
