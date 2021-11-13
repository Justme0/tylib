// readable string, netorder, hostorder IPv4 convert (6 conversion)
#pragma once

#include <string>

#include <arpa/inet.h>

using IPIntegerType = uint32_t;

// 1, i386(主流)主机序(小端)整数IP转为字符串
inline std::string hostOrderToString(IPIntegerType ipAddress)
{
  char s[INET_ADDRSTRLEN];
  snprintf(s, sizeof s, "%u.%u.%u.%u", (ipAddress & 0xff000000) >> 24,
           (ipAddress & 0x00ff0000) >> 16, (ipAddress & 0x0000ff00) >> 8,
           (ipAddress & 0x000000ff));

  return s;
}

// 2, 网络序(大端)整数IP转为字符串
inline std::string netOrderToString(IPIntegerType ipAddress)
{
  char s[INET_ADDRSTRLEN];
  snprintf(s, sizeof s, "%u.%u.%u.%u", (ipAddress & 0x000000ff),
           (ipAddress & 0x0000ff00) >> 8, (ipAddress & 0x00ff0000) >> 16,
           (ipAddress & 0xff000000) >> 24);

  return s;
}

// 3, return 0 if s is invalid, don't return error :)
inline IPIntegerType stringToNetOrder(const std::string& s)
{
  struct in_addr inaddr = 0;

  inet_pton(AF_INET, s.data(), &inaddr);  // return 1 on success

  return inaddr;
}

// 4
inline IPIntegerType stringToHostOrder(const std::string& s)
{
  return ntohl(stringToNetOrder(s));
}

// 5, htonl(3) convert host order to net
// 6, ntohl(3) convert net order to host

inline void GetIpPort(sockaddr_in addr, std::string& ip, int& port) {
    char str[INET_ADDRSTRLEN]; // only support IPv4
    const char *s = inet_ntop(AF_INET, &(addr.sin_addr), str, INET_ADDRSTRLEN);
    if (nullptr == s) {
      tylog("inet_ntop return null, errno=%d[%s]", errno, strerror(errno));
      ip = "";
    } else {
        ip = str;
    }
    port = ntohs(addr.sin_port);
}
