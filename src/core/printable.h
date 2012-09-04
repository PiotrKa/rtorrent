#ifndef PRINTABLE_H
#define PRINTABLE_H

#include <iostream>

class Printable {
        public:
                virtual std::string to_string() const = 0;
};

template<typename _CharT,class _Traits> inline std::basic_ostream<_CharT,_Traits>&
        operator<<( std::basic_ostream<_CharT,_Traits>& out, const Printable& val) {
        return out << val.to_string();
}

#endif
