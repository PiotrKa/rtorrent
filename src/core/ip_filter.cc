#include "config.h"

#include <string>
#include <map>
#include <list>
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include <torrent/exceptions.h>

#include "globals.h"
#include "control.h"
#include "core/manager.h"
#include "ip_filter.h"

namespace core {

int IpFilter::insert( range_map* rs, IpRange* r ) {
        if( !r || !r->get_from() )
                return 0;

        std::pair<const IpAddress,IpRange::ptr> p( *r->get_from(), IpRange::ptr(r) );
        std::pair<range_itr,bool> duo = rs->insert( p );

        // return 1 if insert failed
        return duo.second == false ? 1 : 0;
}

int IpFilter::add_from_file( const std::string& fileName, range_map* rs, str_list* files ) {
        std::ifstream in( fileName.c_str() );
        std::string line;
        int errors = 0;

        if( in.fail() || !in.is_open() )
                return -1;

        while( in.good() ) {
                std::getline( in, line );
                boost::trim( line );

                if( (line[0] == '#') || (line.length() == 0) )
                        continue;

                IpRange* ir = IpRange::parse( line );
                if( !ir )
                        ++errors;
                else
                        errors += insert( rs, ir );
        }
        files->push_back( std::string(fileName) );
        in.close();

        m_errors += errors;
        return errors;
}

int IpFilter::add_from_file( const std::string& fileName ) {
        if( !m_ranges )
                m_ranges = new range_map();
        if( !m_loadedFiles )
                m_loadedFiles = new std::list<std::string>();

        return add_from_file( fileName, m_ranges, m_loadedFiles );
}

int IpFilter::reload() {
        if( !m_loadedFiles || m_loadedFiles->empty() )
                return 0;

        range_map* rs = new range_map();
        str_list* files = new str_list();
        int errors = 0;
        int ret;
        for( str_list::const_iterator it = m_loadedFiles->begin(), end = m_loadedFiles->end(); it != end; it++ )
                if ((ret = add_from_file( *it, rs, files )) < 0) {
                    control->core()->push_log(("IP filter failed to open file '" + *it + "'.").c_str());
                    throw torrent::input_error("IP filter failed to open file '" + *it + "'.");
                }
                else
                    errors += ret;

        range_map* rsOld = m_ranges;
        m_ranges = rs;
        if( rsOld ) {
                clear( rsOld );
                delete rsOld;
        }

        str_list* filesOld = m_loadedFiles;
        m_loadedFiles = files;
        if( filesOld ) {
                clear( filesOld );
                delete filesOld;
        }

        m_errors = errors;
        return errors;
}

IpRange* IpFilter::find_range( uint32_t ip ) const {
        if( (ip >= 0) && m_ranges && !m_ranges->empty() ) {
                range_itr idx = m_ranges->upper_bound( ip );
                if( idx != m_ranges->begin() )
                        --idx;
                IpRange* curr = idx->second;
                if( curr->includes( ip ) )
                        return curr;
        }
        return NULL;
}

std::string IpFilter::to_string() const {
        std::stringstream result;
        if( !m_ranges )
                result << "NULL" << std::endl;
        else {
                for( range_map::const_iterator it = m_ranges->begin() ; it != m_ranges->end(); it++ ) {
                        const IpAddress a = it->first;
                        IpRange* ir = it->second;
                        result << a << ": " << *ir << std::endl;
                }
        }
        return result.str();
}

void IpFilter::clear( range_map* map ) {
        if( map ) {
                for( range_itr i = map->begin(), j = map->end(); i != j; i++ )
                        delete i->second;
                map->clear();
        }
}

void IpFilter::clear( str_list* list ) {
        if( list )
                list->clear();
}

}
