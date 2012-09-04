#include <cstdlib>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <boost/xpressive/xpressive.hpp>

#include "ip_address.h"

namespace core {

std::pair<bool,uint32_t> IpAddress::to_int( const std::string& address ) {
  uint32_t a;
  int r = inet_pton( AF_INET, address.c_str(), &a);
  if( r )
    a = ntohl( a );
  return std::pair<bool,uint32_t>( (r!=0), a );
}

std::string IpAddress::to_string() const {
  char buf[128] = "";
  uint32_t a = htonl( m_address );
  inet_ntop( AF_INET, &a, buf, sizeof(buf) );
  return std::string( buf );
}

}
