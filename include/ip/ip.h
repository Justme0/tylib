#ifndef TY_IP_H_
#define TY_IP_H_

#include <string>

// 主机序(小端)整数IP转为字符串
inline std::string hostOrderToString(uint32_t ipAddress)
{
  char sIpAddr[16];
  snprintf(sIpAddr, sizeof sIpAddr, "%u.%u.%u.%u",
           (ipAddress & 0xff000000) >> 24, (ipAddress & 0x00ff0000) >> 16,
           (ipAddress & 0x0000ff00) >> 8, (ipAddress & 0x000000ff));

  return sIpAddr;
}

// 网络序(大端)整数IP转为字符串
inline std::string netOrderToString(uint32_t ipAddress)
{
  char sIpAddr[16];
  snprintf(sIpAddr, sizeof sIpAddr, "%u.%u.%u.%u", (ipAddress & 0x000000ff),
           (ipAddress & 0x0000ff00) >> 8, (ipAddress & 0x00ff0000) >> 16,
           (ipAddress & 0xff000000) >> 24);

  return sIpAddr;
}

#endif  // TY_IP_H_
