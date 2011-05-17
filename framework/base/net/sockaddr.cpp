#include <ostream>
#include <string.h>
#include <algorithm>
#include <arpa/inet.h>
#include <cassert>
#include <base/net/sockaddr.h>
#include <stdio.h>

#define ADDR_PREFIX_INET "inet://"
#define ADDR_PREFIX_LOCAL "local://"
#define ADDR_PREFIX_FILE "file://"
#define ADDR_LOCALHOST "127.0.0.1"

namespace base
{

SockAddress::SockAddress()
   :   _string(), _len(0), _stAddr()
{
   memset(&_stAddr, 0, sizeof(_stAddr));
}

SockAddress::SockAddress(const sockaddr * pSockaddr)
   :   _string(), _len(0), _stAddr()
{
   Setsockaddr(pSockaddr);
}

void SockAddress::Setsockaddr(const sockaddr * pSockaddr)
{
    if (AF_INET == pSockaddr->sa_family)
    {
        _stAddr.in.sin_family = AF_INET;
        const sockaddr_in* sockaddin = (const sockaddr_in*)(pSockaddr);
        _stAddr.in.sin_port = sockaddin->sin_port;
        _stAddr.in.sin_addr = sockaddin->sin_addr;
        _len = sizeof(_stAddr.in);
        char ip[16] = {0};
        if(inet_ntop(AF_INET, &(sockaddin->sin_addr), ip, sizeof(ip))==NULL)
        {
            _string.assign("0.0.0.0");
        }
        else
        {
            _string.assign(ip);
        }
        
        int port = ntohs(sockaddin->sin_port);
        snprintf(ip, sizeof(ip)-1, "%d", port);
        _string.append(":");
        _string.append(ip);
    }
    else if (AF_LOCAL == pSockaddr->sa_family)
    {
        _stAddr.un.sun_family = AF_LOCAL;
        struct sockaddr_un* sockaddrun = (struct sockaddr_un*)(pSockaddr);
        strncpy(_stAddr.un.sun_path, sockaddrun->sun_path, sizeof(_stAddr.un.sun_path) - 1);
        _len = sizeof(_stAddr.un);
        _string.assign(sockaddrun->sun_path);
    }
    else
    {
        assert(false);
    }
}

int SockAddress::SetAddr(const std::string& ip, int port)
{
   sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));

   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);   
   Setsockaddr((sockaddr*)&addr);
   return 0;
}

int SockAddress::SetAddr(const char* addr)
{
   if (strncmp(addr, ADDR_PREFIX_INET, strlen(ADDR_PREFIX_INET)) == 0) 
   {
      addr += strlen(ADDR_PREFIX_INET);

      const char* host = addr;
      unsigned short port = 0;

      const char* pos = NULL;
      if ((pos = (const char*)strstr(addr, ":")) != NULL) 
      {
         port = atoi(pos + 1);
         return SetAddr(std::string(host, pos).c_str(), port);
      }
      
   }
   sockaddr_un stAddr;
   memset(&stAddr, 0, sizeof(stAddr));   

   stAddr.sun_family = AF_LOCAL;
   const char* path;
   
   if (strncmp(addr, ADDR_PREFIX_LOCAL, strlen(ADDR_PREFIX_LOCAL)) == 0) 
   {
      path = addr + strlen(ADDR_PREFIX_LOCAL);
   }
   else if (strncmp(addr, ADDR_PREFIX_FILE, strlen(ADDR_PREFIX_FILE)) == 0)
   {
      path = addr + strlen(ADDR_PREFIX_FILE);
   }
   else 
   {
      path = addr;
   }
   strncpy(stAddr.sun_path, path, sizeof(stAddr.sun_path) - 1);
   Setsockaddr((sockaddr*)&stAddr);
        return 0; 
}

std::string SockAddress::GetHost()
{
    if (_stAddr.generic.sa_family == AF_INET)
    {
        return _string.substr(0, _string.find(":"));
    }
    else
    {
        return _string;
    }
    
}

unsigned short SockAddress::GetPort()
{
    if (_stAddr.generic.sa_family == AF_INET)
    {
        return ntohs(_stAddr.in.sin_port);
    }
    else
    {
        return 0;
    }
}

}
