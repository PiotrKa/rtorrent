#ifndef IPFILTER_H
#define IPFILTER_H

#include <string>
#include <map>
#include <list>

#include "printable.h"
#include "ip_address.h"
#include "ip_range.h"
#include "regex_namespace.h"

namespace core {

typedef std::map<const IpAddress,IpRange::ptr>  range_map;
typedef range_map::iterator                     range_itr;
typedef std::list<std::string>                  str_list;

class IpFilter : public Printable {
        private: // fields
                int m_errors;
                range_map* m_ranges;
                str_list* m_loadedFiles;

        private: // static methods
                static void clear( range_map* map );
                static void clear( str_list* list );

        private: // dynamic methods
                void init_members(void) { // to avoid long constructor lines for every ctor
                        m_ranges = NULL;
                        m_loadedFiles = NULL;
                        m_errors = 0;
                }
                int insert( range_map* rs, IpRange* r );
                int add_from_file( const std::string& fileName, range_map* rs, str_list* files );

        public: // static methods

        public: // dynamic methods
                IpFilter() { init_members(); }
                ~IpFilter() {
                        clear();
                        if( m_ranges ) delete m_ranges;
                        if( m_loadedFiles ) delete m_loadedFiles;
                        m_ranges = NULL;
                        m_loadedFiles = NULL;
                }
                IpFilter( std::string* files, int size ) {
                        init_members();
                        for( int i = 0; i < size; i++, files++ )
                                add_from_file( *files );
                }
                IpFilter( str_list& files ) {
                        init_members();
                        for( str_list::const_iterator i = files.begin(), last = files.end(); i != last; i++ )
                                add_from_file( *i );
                }
                IpFilter( IpFilter& f ) {
                        init_members();
                        m_ranges = new range_map( *f.m_ranges );
                        m_loadedFiles = new str_list( *f.m_loadedFiles );
                }

                int reload();
                int add_from_file( const std::string& fileName );
                int add_from_file( char* fileName ) { std::string s( fileName ); return add_from_file(s); }
                void clear() { clear( m_ranges ); clear( m_loadedFiles ); }

                IpRange* find_range( uint32_t ip ) const;

                bool is_filtered( uint32_t ip ) const { return (find_range( ip ) != NULL); }
                bool is_filtered( std::string ip ) const {
                static std::pair<bool,uint32_t> ipInt = IpAddress::to_int( ip );
                        return (!ipInt.first ? false : is_filtered( ipInt.second ));
                }

                std::string to_string() const;

                int size(void) { return ( m_ranges ? m_ranges->size() : 0 ); }
                int errors(void) { return m_errors; }
                void set_files( str_list& files) { m_loadedFiles = new str_list( files ); }
};

}
#endif
