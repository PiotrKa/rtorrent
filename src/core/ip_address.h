#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "printable.h"
#include "regex_namespace.h"

namespace core {

class IpAddress : public Printable {
        friend class IpRange;

        private: // constants
                static const std::string        PATTERN_IP_EXPRESSION;
                static const std::string        PATTERN_IP_BYTES_EXPRESSION;
                static const regex::sregex      PATTERN_IP_BYTES;

                static const int                        GRP_IP_FIRST_BYTE;
                static const int                        GRP_IP_BYTES_COUNT;

        private: // fields
                uint32_t                     m_address;

        private: // static methods

        private: // dynamic methods
                IpAddress() : m_address(0) {}

                void copy( const IpAddress& addr ) {
                        m_address = addr.m_address;
                }

        public: // static methods
                static std::pair<bool,uint32_t> to_int( const std::string& strAddress );
                static IpAddress* parse( const std::string& strAddress ) {
                  std::pair<bool,uint32_t> result = to_int( strAddress );
                  return ( !result.first ) ? NULL : new IpAddress( result.second );
                }

        public: // dynamic methods
                IpAddress( uint32_t address ) : m_address(address) {}
                IpAddress( const IpAddress& addr ) { copy( addr ); }
                IpAddress& operator= ( const IpAddress& addr ) { copy( addr ); return *this; }

                operator uint32_t() const { return m_address; }

                bool operator>= ( const IpAddress& ip ) const { return (m_address >=    ip.m_address); }
                bool operator<= ( const IpAddress& ip ) const { return (m_address <=    ip.m_address); }
                bool operator<  ( const IpAddress& ip ) const { return (m_address <     ip.m_address); }
                bool operator>  ( const IpAddress& ip ) const { return (m_address >     ip.m_address); }
                bool operator== ( const IpAddress& ip ) const { return (m_address ==    ip.m_address); }
                bool operator!= ( const IpAddress& ip ) const { return (m_address !=    ip.m_address); }

                bool operator>= ( uint32_t ip ) const { return (m_address >= ip); }
                bool operator<= ( uint32_t ip ) const { return (m_address <= ip); }
                bool operator<  ( uint32_t ip ) const { return (m_address <  ip); }
                bool operator>  ( uint32_t ip ) const { return (m_address >  ip); }
                bool operator== ( uint32_t ip ) const { return (m_address == ip); }
                bool operator!= ( uint32_t ip ) const { return (m_address != ip); }

                std::string to_string() const;
};

}
#endif
