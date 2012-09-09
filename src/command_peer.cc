// rTorrent - BitTorrent client
// Copyright (C) 2005-2011, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#include "config.h"

#include <rak/error_number.h>
#include <rak/path.h>
#include <rak/socket_address.h>
#include <rak/string_manip.h>
#include <torrent/bitfield.h>
#include <torrent/rate.h>
#include <torrent/peer/connection_list.h>
#include <torrent/peer/peer.h>
#include <torrent/peer/peer_info.h>
#include <sstream>
#include <GeoIP.h>
#include <GeoIPCity.h>

#include "core/manager.h"
#include "display/utils.h"

#include "globals.h"
#include "control.h"
#include "command_helpers.h"

#define GEOIP_IP_NOT_FOUND "(IP address not found)"
#define MK_NA(p) (p ? p : "N/A")

torrent::Object
retrieve_p_id(torrent::Peer* peer) {
  const torrent::HashString* hashString = &peer->id();

  return rak::transform_hex(hashString->begin(), hashString->end());
}

torrent::Object
retrieve_p_id_html(torrent::Peer* peer) {
  const torrent::HashString* hashString = &peer->id();

  return rak::copy_escape_html(hashString->begin(), hashString->end());
}

torrent::Object
retrieve_p_address(torrent::Peer* peer) {
  return rak::socket_address::cast_from(peer->peer_info()->socket_address())->address_str();
}

torrent::Object
retrieve_p_port(torrent::Peer* peer) {
  return rak::socket_address::cast_from(peer->peer_info()->socket_address())->port();
}

torrent::Object
retrieve_p_country(torrent::Peer* peer) {
  GeoIP*      gi = control->core()->gi_country();
  int         country_id;
  std::string country_code;
  std::string country_name;
  std::string country_continent;

  if (!gi)
    return "(country db not available)";

  country_id = GeoIP_id_by_ipnum(gi, ntohl(rak::socket_address::cast_from(peer->peer_info()->socket_address())->address().s_addr));
  if (!country_id)
    return GEOIP_IP_NOT_FOUND;
  else {
    country_code = MK_NA(GeoIP_code_by_id(country_id));
    country_name = MK_NA(GeoIP_name_by_id(country_id));
    country_continent = MK_NA(GeoIP_continent_by_id(country_id));
    return country_continent + "; " + country_code + ", " + country_name;
  }
}

torrent::Object
retrieve_p_city(torrent::Peer* peer) {
  GeoIP*             gi = control->core()->gi_city();
  GeoIPRecord*       gir;
  std::ostringstream s;

  if (!gi)
    return "(city db not available)";

  gir = GeoIP_record_by_ipnum(gi, ntohl(rak::socket_address::cast_from(peer->peer_info()->socket_address())->address().s_addr));
  if (!gir)
    return GEOIP_IP_NOT_FOUND;
  else {
    s << MK_NA(gir->region) << ", " << MK_NA(gir->city)
      << ", " << MK_NA(gir->postal_code)
      << ", " << gir->latitude << ", " << gir->longitude
      << ", " << gir->metro_code << ", " << gir->area_code;
    GeoIPRecord_delete(gir);
    return s.str();
  }
}

torrent::Object
retrieve_p_asnum(torrent::Peer* peer) {
  GeoIP* gi = control->core()->gi_asnum();
  const char *asnum_name;

  if (!gi)
    return "(ASNum db not available)";

  asnum_name = GeoIP_name_by_ipnum(gi, ntohl(rak::socket_address::cast_from(peer->peer_info()->socket_address())->address().s_addr));
  return asnum_name ? asnum_name : GEOIP_IP_NOT_FOUND;
}

torrent::Object
retrieve_p_client_version(torrent::Peer* peer) {
  char buf[128];
  display::print_client_version(buf, buf + 128, peer->peer_info()->client_info());

  return std::string(buf);
}

torrent::Object
retrieve_p_options_str(torrent::Peer* peer) {
  return rak::transform_hex(peer->peer_info()->options(), peer->peer_info()->options() + 8);
}

torrent::Object
retrieve_p_completed_percent(torrent::Peer* peer) {
  return (100 * peer->bitfield()->size_set()) / peer->bitfield()->size_bits();
}

void
initialize_command_peer() {
  CMD2_PEER("p.id",                tr1::bind(&retrieve_p_id, tr1::placeholders::_1));
  CMD2_PEER("p.id_html",           tr1::bind(&retrieve_p_id_html, tr1::placeholders::_1));
  CMD2_PEER("p.client_version",    tr1::bind(&retrieve_p_client_version, tr1::placeholders::_1));

  CMD2_PEER("p.options_str",       tr1::bind(&retrieve_p_options_str, tr1::placeholders::_1));

  CMD2_PEER("p.is_encrypted",      tr1::bind(&torrent::Peer::is_encrypted, tr1::placeholders::_1));
  CMD2_PEER("p.is_incoming",       tr1::bind(&torrent::Peer::is_incoming, tr1::placeholders::_1));
  CMD2_PEER("p.is_obfuscated",     tr1::bind(&torrent::Peer::is_obfuscated, tr1::placeholders::_1));
  CMD2_PEER("p.is_snubbed",        tr1::bind(&torrent::Peer::is_snubbed, tr1::placeholders::_1));

  CMD2_PEER("p.is_unwanted",       tr1::bind(&torrent::PeerInfo::is_unwanted,  tr1::bind(&torrent::Peer::peer_info, tr1::placeholders::_1)));
  CMD2_PEER("p.is_preferred",      tr1::bind(&torrent::PeerInfo::is_preferred, tr1::bind(&torrent::Peer::peer_info, tr1::placeholders::_1)));

  CMD2_PEER("p.address",           tr1::bind(&retrieve_p_address, tr1::placeholders::_1));
  CMD2_PEER("p.port",              tr1::bind(&retrieve_p_port, tr1::placeholders::_1));

  CMD2_PEER("p.country",           tr1::bind(&retrieve_p_country, tr1::placeholders::_1));
  CMD2_PEER("p.city",              tr1::bind(&retrieve_p_city, tr1::placeholders::_1));
  CMD2_PEER("p.asnum",             tr1::bind(&retrieve_p_asnum, tr1::placeholders::_1));

  CMD2_PEER("p.completed_percent", tr1::bind(&retrieve_p_completed_percent, tr1::placeholders::_1));

  CMD2_PEER("p.up_rate",           tr1::bind(&torrent::Rate::rate,  tr1::bind(&torrent::Peer::up_rate, tr1::placeholders::_1)));
  CMD2_PEER("p.up_total",          tr1::bind(&torrent::Rate::total, tr1::bind(&torrent::Peer::up_rate, tr1::placeholders::_1)));
  CMD2_PEER("p.down_rate",         tr1::bind(&torrent::Rate::rate,  tr1::bind(&torrent::Peer::down_rate, tr1::placeholders::_1)));
  CMD2_PEER("p.down_total",        tr1::bind(&torrent::Rate::total, tr1::bind(&torrent::Peer::down_rate, tr1::placeholders::_1)));
  CMD2_PEER("p.peer_rate",         tr1::bind(&torrent::Rate::rate,  tr1::bind(&torrent::Peer::peer_rate, tr1::placeholders::_1)));
  CMD2_PEER("p.peer_total",        tr1::bind(&torrent::Rate::total, tr1::bind(&torrent::Peer::peer_rate, tr1::placeholders::_1)));

  CMD2_PEER        ("p.snubbed",     tr1::bind(&torrent::Peer::is_snubbed,  tr1::placeholders::_1));
  CMD2_PEER_VALUE_V("p.snubbed.set", tr1::bind(&torrent::Peer::set_snubbed, tr1::placeholders::_1, tr1::placeholders::_2));
  CMD2_PEER        ("p.banned",      tr1::bind(&torrent::Peer::is_banned,   tr1::placeholders::_1));
  CMD2_PEER_VALUE_V("p.banned.set",  tr1::bind(&torrent::Peer::set_banned,  tr1::placeholders::_1, tr1::placeholders::_2));

  CMD2_PEER_V("p.disconnect",         tr1::bind(&torrent::Peer::disconnect, tr1::placeholders::_1, 0));
  CMD2_PEER_V("p.disconnect_delayed", tr1::bind(&torrent::Peer::disconnect, tr1::placeholders::_1, torrent::ConnectionList::disconnect_delayed));
}
