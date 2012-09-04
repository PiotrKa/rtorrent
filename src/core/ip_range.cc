#include <string>
#include <boost/xpressive/xpressive.hpp>

#include "ip_range.h"

namespace core {

IpRange* IpRange::parse( const std::string& s ) {
        regex::smatch what;

        if( !regex::regex_match( s, what, PATTERN_RANGE ) )
                return NULL;

        std::string     description     = what[ GRP_DESCRIPTION ].str();
        std::string     ip1             = what[ GRP_FIRST_IP ].str();
        std::string     ip2             = what[ GRP_SECOND_IP ].str();
        IpAddress*      from            = IpAddress::parse( ip1 );
        IpAddress*      to              = IpAddress::parse( ip2 );

        if( !from || !to || (*to < *from) )
                return NULL;

        IpRange* r = new IpRange();

        r->m_description        = description;
        r->m_from               = from;
        r->m_to                 = to;

        return r;
}

std::string IpRange::to_string() const {
        std::stringstream result;
        result << m_description << ": [" << m_from->to_string() << " - " << m_to->to_string() << ']';
        return result.str();
}

}
