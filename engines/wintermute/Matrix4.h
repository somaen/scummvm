/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#ifndef WINTERMUTE_MATRIX4_H
#define WINTERMUTE_MATRIX4_H

namespace WinterMute {

class Vector2;

class Matrix4 {
public:
	Matrix4();
	~Matrix4();

	void Identity();
	void RotationZ(float angle);
	void TransformVector2(Vector2 &vec);

	/*  union {
	        struct {
	            float _11, _12, _13, _14;
	            float _21, _22, _23, _24;
	            float _31, _32, _33, _34;
	            float _41, _42, _43, _44;
	        };*/
	float m[4][4];
	//};

};

} // end of namespace WinterMute

#endif // WINTERMUTE_MATRIX4_H