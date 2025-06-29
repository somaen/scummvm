/* ScummVM - Graphic Adventure Engine
*
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef COMMON_ORDERED_HASHMAP_H
#define COMMON_ORDERED_HASHMAP_H

#include "common/hashmap.h"
#include "common/list.h"

namespace Common {
/**
 * HashMap<Key,Val> maps objects of type Key to objects of type Val.
 * This keeps the same exact functionality as HashMap, except that
 * iteration order follows insertion-order.
 */
template<class Key, class Val, class HashFunc = Hash<Key>, class EqualFunc = EqualTo<Key> >
class OrderedHashMap {
	typedef HashMap<Key, Val, HashFunc, EqualFunc> HM_t;
	typedef OrderedHashMap<Key, Val, HashFunc, EqualFunc> OHM_t;
	HM_t _map;
	List<Key> _insertionOrder;

	template<class T> friend class IteratorImpl;

	/**
	 * Simple OrderedHashMap iterator implementation.
	 */
	template<class HashMapType, class ListIteratorType>
	class IteratorImpl {
		friend class OrderedHashMap;
#if defined(__INTEL_COMPILER)
		template<class T> friend class Common::IteratorImpl;
#else
		template<class T, class U> friend class IteratorImpl;
#endif
	protected:
		typedef typename HM_t::Node NodeType;

		ListIteratorType _insertionOrderIterator;
		HashMapType *_hashmap;

	protected:
		IteratorImpl(ListIteratorType orderIt, HashMapType *hashmap) : _insertionOrderIterator(orderIt), _hashmap(hashmap) {        }

		NodeType *deref() const {
			assert(_hashmap != nullptr);
			auto key = *_insertionOrderIterator;
			typename HM_t::iterator iterator = _hashmap->find(key);
			NodeType *node = &(*iterator);
			assert(node != nullptr);
			return node;
		}

	public:
		IteratorImpl() : _insertionOrderIterator(0), _hashmap(nullptr) {}
		template<class T, class L>
		IteratorImpl(const IteratorImpl<T, L> &c) : _insertionOrderIterator(c._insertionOrderIterator), _hashmap(c._hashmap) {}

		NodeType &operator*() const { return *deref(); }
		NodeType *operator->() const { return deref(); }

		bool operator==(const IteratorImpl &iter) const { return _insertionOrderIterator == iter._insertionOrderIterator && _hashmap == iter._hashmap; }
		bool operator!=(const IteratorImpl &iter) const { return !(*this == iter); }

		IteratorImpl &operator++() {
			assert(_hashmap);
			_insertionOrderIterator++;
			return *this;
		}

		IteratorImpl operator++(int) {
			IteratorImpl old = *this;
			operator ++();
			return old;
		}
	};

public:
	typedef uint size_type;

	typedef IteratorImpl<HM_t, typename List<Key>::iterator> iterator;
	typedef IteratorImpl<const HM_t, typename List<Key>::const_iterator> const_iterator;

	OrderedHashMap() = default;
	OrderedHashMap(const OHM_t &map) : _map(map._map), _insertionOrder(map._insertionOrder) {};

	OrderedHashMap &operator=(const OHM_t &map) {
		_map = map._map;
		_insertionOrder = map._insertionOrder;
		return *this;
	}

	bool contains(const Key &key) const {
		return _map.contains(key);
	}

	Val &operator[](const Key &key) {
		return getOrCreateVal(key);
	}
	const Val &operator[](const Key &key) const {
		return getVal(key);
	}

	Val &getOrCreateVal(const Key &key) {
		// This is cheaper than checking whether we have the element already
		int currentSize = _map.size();
		auto &val = _map.getOrCreateVal(key);
		if (_map.size() > currentSize) {
			_insertionOrder.push_back(key);
		}
		return val;
	}
	Val &getVal(const Key &key) {
		return _map.getVal(key);
	}
	const Val &getVal(const Key &key) const {
		return _map.getVal(key);
	}
	const Val &getValOrDefault(const Key &key) const {
		return _map.getValOrDefault(key);
	}
	const Val &getValOrDefault(const Key &key, const Val &defaultVal) const {
		return _map.getValOrDefault(key, defaultVal);
	}
	bool tryGetVal(const Key &key, Val &out) const {
		return _map.tryGetVal(key, out);
	}
	void setVal(const Key &key, const Val &val) {
		int currentSize = _map.size();
		_map.setVal(key, val);
		if (_map.size() > currentSize) {
			_insertionOrder.push_back(key);
		}
	}

	void clear(bool shrinkArray = 0) {
		_map.clear(shrinkArray);
		_insertionOrder.clear();
	}

	void erase(iterator entry);
	void erase(const Key &key);

	size_type size() const { return _map.size(); }

	iterator	begin() {
		return iterator(_insertionOrder.begin(), &_map);
	}
	iterator	end() {
		return iterator(_insertionOrder.end(), &_map);
	}
	const_iterator	begin() const {
		return const_iterator(_insertionOrder.begin(), &_map);
	}
	const_iterator	end() const {
		return const_iterator(_insertionOrder.end(), &_map);
	}

	// Ironically we get O(N) lookup as we have to find the
	// right iterator to preserve insertion order.
	iterator	find(const Key &key) {
		for (auto it = begin(); it != end(); ++it) {
			if (it->_key == key) {
				return it;
			}
		}
		return end();
	}

	const_iterator	find(const Key &key) const {
		for (const auto it = begin(); it != end(); ++it) {
			if (it->_key == key) {
				return it;
			}
		}
		return end();
	}

	bool empty() const {
		return _map.empty();
	}
};

template<class Key, class Val, class HashFunc, class EqualFunc>
void OrderedHashMap<Key, Val, HashFunc, EqualFunc>::erase(iterator entry) {
	// Check whether we have a valid iterator
	assert(entry._hashmap == &this->_map);
	_map.erase(*entry._insertionOrderIterator);
	_insertionOrder.erase(entry._insertionOrderIterator);
}

/**
 * Erase an element specified by a key.
 */

template<class Key, class Val, class HashFunc, class EqualFunc>
void OrderedHashMap<Key, Val, HashFunc, EqualFunc>::erase(const Key &key) {
	_map.erase(key);
	for (auto it = _insertionOrder.begin(); it != _insertionOrder.end(); it++) {
		if (*it == key) {
			_insertionOrder.erase(it);
			break;
		}
	}
}

} // End of namespace Common

#endif //ORDERED_HASHMAP_H
