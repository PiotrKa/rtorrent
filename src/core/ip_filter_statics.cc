#include "ip_address.h"
#include "ip_range.h"

namespace core {

const std::string       IpAddress::PATTERN_IP_EXPRESSION        = "((?:\\d{1,3}\\.){3}\\d{1,3})";
const std::string       IpAddress::PATTERN_IP_BYTES_EXPRESSION  = "(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})";
const regex::sregex     IpAddress::PATTERN_IP_BYTES             = regex::sregex::compile( PATTERN_IP_BYTES_EXPRESSION );

const int               IpAddress::GRP_IP_FIRST_BYTE    = 1;
const int               IpAddress::GRP_IP_BYTES_COUNT   = 4;

const std::string       IpRange::PATTERN_RANGE_EXPRESSION       = "\\s*(.*)\\s*:\\s*" + IpAddress::PATTERN_IP_EXPRESSION + "\\s*-\\s*" + IpAddress::PATTERN_IP_EXPRESSION + "\\s*";
const regex::sregex     IpRange::PATTERN_RANGE                  = regex::sregex::compile( PATTERN_RANGE_EXPRESSION );

const int               IpRange::GRP_DESCRIPTION        = 1;
const int               IpRange::GRP_FIRST_IP           = 2;
const int               IpRange::GRP_SECOND_IP          = 3;

}
