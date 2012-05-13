/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _RPACKET_H
#define _RPACKET_H

#include "buffer.h"

typedef struct rpacket
{
	unsigned long cmd;
	unsigned long len;     //����
	unsigned long rpos;    //���±�
	unsigned long data_remain;
	unsigned long binbufpos;
	unsigned long begin_pos;
	buffer_t binbuf;       //���ڴ�ſ�Խbuffer_t�߽����ݵ�buffer_t
	buffer_t buf;          //��Ŵ����ݰ����ݵ�buffer_t����
	buffer_t readbuf;      //��ǰrpos���ڵ�buffer_t
}*rpacket_t;

struct wpacket;

rpacket_t rpacket_create(buffer_t,unsigned long pos);
rpacket_t rpacket_create_by_wpacket(struct wpacket*);//ͨ��wpacket����
void      rpacket_destroy(rpacket_t*);

//���ݶ�ȡ�ӿ�
unsigned long  rpacket_len(rpacket_t);
unsigned long  rpacket_read_cmd(rpacket_t);
unsigned long  rpacket_data_remain(rpacket_t);
unsigned char  rpacket_read_char(rpacket_t);
unsigned short rpacket_read_short(rpacket_t);
unsigned long  rpacket_read_long(rpacket_t);
double         rpacket_read_double(rpacket_t);
const char*    rpacket_read_string(rpacket_t);
const void*    rpacket_read_binary(rpacket_t,unsigned long *len);

#endif