/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef STARK_SKELETON_H
#define STARK_SKELETON_H

#include "common/array.h"
#include "common/str.h"

#include "math/quat.h"
#include "math/vector3d.h"

namespace Stark {

class ArchiveReadStream;
class SkeletonAnim;

class BoneNode {
public:
	BoneNode() : _parent(-1) { }
	~BoneNode() { }
	Common::String _name;
	float _u1;
	Common::Array<uint32> _children;
	int _parent;
	int _idx;

	Math::Vector3d _animPos;
	Math::Quaternion _animRot;
	Math::Matrix4 _animTransform;
};

/**
 * Skeleton manager to load and store skeletal information about an actor
 */
class Skeleton {
public:
	Skeleton();
	~Skeleton();

	/**
	 * Increment the skeleton timestamp, and apply bone animations if required
	 */
	void animate(uint32 time);

	/**
	 * Start reading animation data from the specified stream
	 */
	void setAnim(SkeletonAnim *anim);

	/**
	 * Create skeleton object from the specified stream
	 */
	void readFromStream(ArchiveReadStream *stream);

	/**
	 * Transform the vertex so that it is attached to the requested bone
	 */
	void applyBoneTransform(uint32 boneIdx, Math::Vector3d &vertex);

private:
	void setNode(uint32 time, BoneNode *bone, const BoneNode *parent);

	Common::Array<BoneNode *> _bones;
	SkeletonAnim *_anim;

	uint32 _lastTime;
};

} // End of namespace Stark

#endif // STARK_SKELETON_H
