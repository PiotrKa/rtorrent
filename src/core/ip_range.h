#ifndef IPRANGE_H
#define IPRANGE_H

#include <string>
#include <boost/xpressive/xpressive.hpp>
#include <boost/shared_ptr.hpp>

#include "printable.h"
#include "ip_address.h"
#include "regex_namespace.h"

namespace core {

class IpRange : public Printable {
        private: // constants
                static const std::string        PATTERN_RANGE_EXPRESSION;
                static const regex::sregex      PATTERN_RANGE;

                static const int                GRP_DESCRIPTION;
                static const int                GRP_FIRST_IP;
                static const int                GRP_SECOND_IP;

        private: // fields
                std::string             m_description;
                const IpAddress*        m_from;
                const IpAddress*        m_to;

        private: // dynamic methods
                IpRange() : m_description(), m_from(NULL), m_to(NULL) {}

        public: // static methods
                typedef IpRange* ptr;
                static IpRange* parse( const std::string& s );

        public: // dynamic methods
                IpRange( IpRange& rng ) { copy(rng); }
                IpRange& operator= ( IpRange& rng ) { copy(rng); return *this; }

                void copy( IpRange& rng ) {
                        m_description = rng.m_description;
                        m_from = (!rng.m_from) ? NULL : new IpAddress( *rng.m_from );
                        m_to = (!rng.m_to) ? NULL : new IpAddress( *rng.m_to );
                }

                const std::string&      get_description ( void ) const  { return m_description; }
                const IpAddress*        get_from        ( void ) const  { return m_from; }
                const IpAddress*        get_to          ( void ) const  { return m_to; }

                void    set_description ( const std::string&    description )   { m_description = description; }
                void    set_from        ( const IpAddress*      from )          { if( m_from ) delete m_from; m_from = new IpAddress( *from ); }
                void    set_to          ( const IpAddress*      to )            { if( m_to ) delete m_to; m_to = new IpAddress( *to ); }

                bool    includes( const IpAddress& ip ) const { return includes((uint32_t)ip); }
                bool    includes( uint32_t ip ) const { return (*m_from <= ip) && (*m_to >= ip); }

                ~IpRange() {
                        delete m_from;
                        m_from = NULL;
                        delete m_to;
                        m_to = NULL;
                }

                std::string to_string() const;
};

}
#endif
