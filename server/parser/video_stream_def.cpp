// 
//   Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// 
// $Id: video_stream_def.cpp,v 1.3 2007/02/08 14:40:20 tgc Exp $

#include "embedVideoDecoderFfmpeg.h"
#include "video_stream_def.h"
#include "video_stream_instance.h"
#include "render.h"

namespace gnash {

video_stream_definition::video_stream_definition(uint16_t char_id)
	:
	m_char_id(char_id)
{
}


video_stream_definition::~video_stream_definition()
{
}


void
video_stream_definition::read(stream* in, SWF::tag_type tag, movie_definition* m)
{

	// Character ID has been read already, and was loaded in the constructor

	assert(tag == SWF::DEFINEVIDEOSTREAM ||	tag == SWF::VIDEOFRAME);

	if (tag == SWF::DEFINEVIDEOSTREAM)
	{
		m_start_frame = m->get_loading_frame();

		m_num_frames = in->read_u16();

		m_width = in->read_u16();
		m_height = in->read_u16();
		m_reserved_flags = in->read_uint(5);
		m_deblocking_flags = in->read_uint(2);
		m_smoothing_flags = in->read_uint(1) ? true : false;

		m_codec_id = in->read_u8();

	}
	else if (tag == SWF::VIDEOFRAME)
	{
		in->skip_bytes(2); //int frameNum = in->read_u16();
		int size = in->get_tag_end_position() - in->get_position();
		uint8_t* data = new uint8_t[size];
		for (int i = 0; i < size; i++)
		{
			data[i] = in->read_u8();
		}
		m_video_frames.push_back(data);
		m_video_frames_size.push_back(size);

	}

}


character*
video_stream_definition::create_character_instance(character* parent, int id)
{
	character* ch = new video_stream_instance(this, parent, id);
	return ch;
}

embedVideoDecoder*
video_stream_definition::get_decoder(){

	if (m_num_frames == 0) return NULL;

	embedVideoDecoder* decoder;

#ifdef USE_FFMPEG
	decoder = new embedVideoDecoderFfmpeg();
//#elif defined(SOUND_GST)
//	decoder = new embedVideoDecoderGst();
#else
	decoder = new embedVideoDecoder();
#endif
	decoder->createDecoder(
				m_width,
				m_height,
				m_deblocking_flags,
				m_smoothing_flags,
				m_codec_id,
				gnash::render::videoFrameFormat());
	return decoder;

}

void 
video_stream_definition::get_frame_data(int frameNum, uint8_t** data, int* size){
	int cur_frame = frameNum - m_start_frame;
	*size = m_video_frames_size[cur_frame];
	*data = m_video_frames[cur_frame];
	if (data == NULL) printf("data is NULL\n");
}

}

