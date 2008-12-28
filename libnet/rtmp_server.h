// 
//   Copyright (C) 2005, 2006, 2007, 2008 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef _RTMP_SERVER_H_
#define _RTMP_SERVER_H_ 1

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

#include "rtmp.h"
#include "amf.h"
#include "handler.h"
#include "network.h"
#include "buffer.h"
#include "diskstream.h"

namespace gnash
{

class DSOEXPORT RTMPServer : public RTMP
{
public:
    RTMPServer();
    ~RTMPServer();
//    bool processClientHandShake(int fd, amf::Buffer &buf);
    bool handShakeResponse(int fd, amf::Buffer &buf);
    boost::shared_ptr<amf::Buffer> serverFinish(int fd, amf::Buffer &handshake1, amf::Buffer &handshake2);
    bool packetSend(amf::Buffer &buf);
    bool packetRead(amf::Buffer &buf);
    
    // These are handlers for the various types
    boost::shared_ptr<amf::Buffer> encodeResult(RTMPMsg::rtmp_status_e status);
    boost::shared_ptr<amf::Buffer> encodePing(rtmp_ping_e type, boost::uint32_t milliseconds);
    boost::shared_ptr<amf::Buffer> encodePing(rtmp_ping_e type);
    
    void dump();
  private:
    typedef boost::char_separator<char> Sep;
    typedef boost::tokenizer<Sep> Tok;
    DiskStream::filetype_e  _filetype;
    std::string		_filespec;
    boost::uint32_t     _filesize;
};

// This is the thread for all incoming RTMP connections
bool rtmp_handler(Network::thread_params_t *args);

} // end of gnash namespace
// end of _RTMP_SERVER_H_
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:

