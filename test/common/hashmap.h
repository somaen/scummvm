#include <cxxtest/TestSuite.h>

#include "common/hash-str.h"
#include "common/hashmap.h"
#include "common/ordered_hashmap.h"

class HashMapTestSuite : public CxxTest::TestSuite
{
	public:
	void test_empty_clear() {
		Common::HashMap<int, int> container;
		TS_ASSERT(container.empty());
		container[0] = 17;
		container[1] = 33;
		TS_ASSERT(!container.empty());
		container.clear();
		TS_ASSERT(container.empty());

		Common::StringMap container2;
		TS_ASSERT(container2.empty());
		container2["foo"] = "bar";
		container2["quux"] = "blub";
		TS_ASSERT(!container2.empty());
		container2.clear();
		TS_ASSERT(container2.empty());
	}

	void test_contains() {
		Common::HashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		TS_ASSERT(container.contains(0));
		TS_ASSERT(container.contains(1));
		TS_ASSERT(!container.contains(17));
		TS_ASSERT(!container.contains(-1));

		Common::StringMap container2;
		container2["foo"] = "bar";
		container2["quux"] = "blub";
		TS_ASSERT(container2.contains("foo"));
		TS_ASSERT(container2.contains("quux"));
		TS_ASSERT(!container2.contains("bar"));
		TS_ASSERT(!container2.contains("asdf"));
	}

	void test_add_remove() {
		Common::HashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;
		TS_ASSERT(container.contains(1));
		container.erase(1);
		TS_ASSERT(!container.contains(1));
		container[1] = 42;
		TS_ASSERT(container.contains(1));
		container.erase(0);
		TS_ASSERT(!container.empty());
		container.erase(1);
		TS_ASSERT(!container.empty());
		container.erase(2);
		TS_ASSERT(!container.empty());
		container.erase(3);
		TS_ASSERT(!container.empty());
		container.erase(4);
		TS_ASSERT(container.empty());
		container[1] = 33;
		TS_ASSERT(container.contains(1));
		TS_ASSERT(!container.empty());
		container.erase(1);
		TS_ASSERT(container.empty());
	}

	void test_add_remove_iterator() {
		Common::HashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;
		TS_ASSERT(container.contains(1));
		container.erase(container.find(1));
		TS_ASSERT(!container.contains(1));
		container[1] = 42;
		TS_ASSERT(container.contains(1));
		container.erase(container.find(0));
		TS_ASSERT(!container.empty());
		container.erase(container.find(1));
		TS_ASSERT(!container.empty());
		container.erase(container.find(2));
		TS_ASSERT(!container.empty());
		container.erase(container.find(3));
		TS_ASSERT(!container.empty());
		container.erase(container.find(4));
		TS_ASSERT(container.empty());
		container[1] = 33;
		TS_ASSERT(container.contains(1));
		TS_ASSERT(!container.empty());
		container.erase(container.find(1));
		TS_ASSERT(container.empty());
	}

	void test_lookup() {
		Common::HashMap<int, int> container;
		container[0] = 17;
		container[1] = -1;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;

		TS_ASSERT_EQUALS(container[0], 17);
		TS_ASSERT_EQUALS(container[1], -1);
		TS_ASSERT_EQUALS(container[2], 45);
		TS_ASSERT_EQUALS(container[3], 12);
		TS_ASSERT_EQUALS(container[4], 96);
	}

	void test_lookup_with_default() {
		Common::HashMap<int, int> container;
		container[0] = 17;
		container[1] = -1;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;

		// We take a const ref now to ensure that the map
		// is not modified by getValOrDefault.
		const Common::HashMap<int, int> &containerRef = container;

		TS_ASSERT_EQUALS(containerRef.getValOrDefault(0), 17);
		TS_ASSERT_EQUALS(containerRef.getValOrDefault(17), 0);
		TS_ASSERT_EQUALS(containerRef.getValOrDefault(0, -10), 17);
		TS_ASSERT_EQUALS(containerRef.getValOrDefault(17, -10), -10);
	}

	void test_iterator_begin_end() {
		Common::HashMap<int, int> container;

		// The container is initially empty ...
		TS_ASSERT_EQUALS(container.begin(), container.end());

		// ... then non-empty ...
		container[324] = 33;
		TS_ASSERT_DIFFERS(container.begin(), container.end());

		// ... and again empty.
		container.clear();
		TS_ASSERT_EQUALS(container.begin(), container.end());
	}

	void test_hash_map_copy() {
		Common::HashMap<int, int> map1, container2;
		map1[323] = 32;
		container2 = map1;
		TS_ASSERT_EQUALS(container2[323], 32);
	}

	void test_collision() {
		// NB: The usefulness of this example depends strongly on the
		// specific hashmap implementation.
		// It is constructed to insert multiple colliding elements.
		Common::HashMap<int, int> h;
		h[5] = 1;
		h[32+5] = 1;
		h[64+5] = 1;
		h[128+5] = 1;
		TS_ASSERT(h.contains(5));
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(32+5);
		TS_ASSERT(h.contains(5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(5);
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h[32+5] = 1;
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h[5] = 1;
		TS_ASSERT(h.contains(5));
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(5);
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(64+5);
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(128+5);
		TS_ASSERT(h.contains(32+5));
		h.erase(32+5);
		TS_ASSERT(h.empty());
	}

	void test_iterator() {
		Common::HashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;
		container.erase(1);
		container[1] = 42;
		container.erase(0);
		container.erase(1);

		int found = 0;
		Common::HashMap<int, int>::iterator i;
		for (i = container.begin(); i != container.end(); ++i) {
			int key = i->_key;
			TS_ASSERT(key >= 0 && key <= 4);
			TS_ASSERT(!(found & (1 << key)));
			found |= 1 << key;
		}
		TS_ASSERT(found == 16+8+4);

		found = 0;
		Common::HashMap<int, int>::const_iterator j;
		for (j = container.begin(); j != container.end(); ++j) {
			int key = j->_key;
			TS_ASSERT(key >= 0 && key <= 4);
			TS_ASSERT(!(found & (1 << key)));
			found |= 1 << key;
		}
		TS_ASSERT(found == 16+8+4);
}

	// TODO: Add test cases for iterators, find, ...
};

class OrderedHashMapTestSuite : public CxxTest::TestSuite {
	struct pair {
		int key, value;
	};
public:
	void test_empty_clear() {
		Common::OrderedHashMap<int, int> container;
		TS_ASSERT(container.empty());
		container[0] = 17;
		container[1] = 33;
		TS_ASSERT(!container.empty());
		container.clear();
		TS_ASSERT(container.empty());

		Common::StringMap container2;
		TS_ASSERT(container2.empty());
		container2["foo"] = "bar";
		container2["quux"] = "blub";
		TS_ASSERT(!container2.empty());
		container2.clear();
		TS_ASSERT(container2.empty());
	}

	void test_contains() {
		Common::OrderedHashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		TS_ASSERT(container.contains(0));
		TS_ASSERT(container.contains(1));
		TS_ASSERT(!container.contains(17));
		TS_ASSERT(!container.contains(-1));

		Common::StringMap container2;
		container2["foo"] = "bar";
		container2["quux"] = "blub";
		TS_ASSERT(container2.contains("foo"));
		TS_ASSERT(container2.contains("quux"));
		TS_ASSERT(!container2.contains("bar"));
		TS_ASSERT(!container2.contains("asdf"));
	}

	void test_add_remove() {
		Common::OrderedHashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;
		TS_ASSERT(container.contains(1));
		container.erase(1);
		TS_ASSERT(!container.contains(1));
		container[1] = 42;
		TS_ASSERT(container.contains(1));
		container.erase(0);
		TS_ASSERT(!container.empty());
		container.erase(1);
		TS_ASSERT(!container.empty());
		container.erase(2);
		TS_ASSERT(!container.empty());
		container.erase(3);
		TS_ASSERT(!container.empty());
		container.erase(4);
		TS_ASSERT(container.empty());
		container[1] = 33;
		TS_ASSERT(container.contains(1));
		TS_ASSERT(!container.empty());
		container.erase(1);
		TS_ASSERT(container.empty());
	}

	void test_add_remove_iterator() {
		Common::OrderedHashMap<int, int> container;
		// Intentionally shuffled order.
		container[0] = 17;
		container[4] = 96;
		container[2] = 45;
		container[1] = 33;
		container[3] = 12;
		TS_ASSERT(container.contains(1));
		container.erase(container.find(1));
		TS_ASSERT(!container.contains(1));
		container[1] = 42;
		TS_ASSERT(container.contains(1));
		container.erase(container.find(0));
		TS_ASSERT(!container.empty());
		container.erase(container.find(1));
		TS_ASSERT(!container.empty());
		container.erase(container.find(2));
		TS_ASSERT(!container.empty());
		// Check that we still have preserved order:
		{
			int expectedOrder[] = {
				4, 3
			};

			int index = 0;
			for (auto &element : container) {
				TS_ASSERT(element._key == expectedOrder[index]);
				index++;
			}
		}
		container.erase(container.find(3));
		TS_ASSERT(!container.empty());
		container.erase(container.find(4));
		TS_ASSERT(container.empty());
		container[1] = 33;
		TS_ASSERT(container.contains(1));
		TS_ASSERT(!container.empty());
		container.erase(container.find(1));
		TS_ASSERT(container.empty());
	}

	void test_lookup() {
		Common::OrderedHashMap<int, int> container;
		container[0] = 17;
		container[1] = -1;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;

		TS_ASSERT_EQUALS(container[0], 17);
		TS_ASSERT_EQUALS(container[1], -1);
		TS_ASSERT_EQUALS(container[2], 45);
		TS_ASSERT_EQUALS(container[3], 12);
		TS_ASSERT_EQUALS(container[4], 96);
	}

	void test_lookup_with_default() {
		Common::OrderedHashMap<int, int> container;
		container[0] = 17;
		container[1] = -1;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;

		// We take a const ref now to ensure that the map
		// is not modified by getValOrDefault.
		const Common::OrderedHashMap<int, int> &containerRef = container;

		TS_ASSERT_EQUALS(containerRef.getValOrDefault(0), 17);
		TS_ASSERT_EQUALS(containerRef.getValOrDefault(17), 0);
		TS_ASSERT_EQUALS(containerRef.getValOrDefault(0, -10), 17);
		TS_ASSERT_EQUALS(containerRef.getValOrDefault(17, -10), -10);
	}

	void test_iterator_begin_end() {
		Common::OrderedHashMap<int, int> container;

		// The container is initially empty ...
		TS_ASSERT_EQUALS(container.begin(), container.end());

		// ... then non-empty ...
		container[324] = 33;
		TS_ASSERT_DIFFERS(container.begin(), container.end());

		// ... and again empty.
		container.clear();
		TS_ASSERT_EQUALS(container.begin(), container.end());
	}

	void test_hash_map_copy() {
		Common::HashMap<int, int> map1, container2;
		map1[323] = 32;
		container2 = map1;
		TS_ASSERT_EQUALS(container2[323], 32);
	}

	void test_collision() {
		// NB: The usefulness of this example depends strongly on the
		// specific hashmap implementation.
		// It is constructed to insert multiple colliding elements.
		Common::OrderedHashMap<int, int> h;
		h[5] = 1;
		h[32+5] = 1;
		h[64+5] = 1;
		h[128+5] = 1;
		TS_ASSERT(h.contains(5));
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(32+5);
		TS_ASSERT(h.contains(5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(5);
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h[32+5] = 1;
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h[5] = 1;
		TS_ASSERT(h.contains(5));
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(5);
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(64+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(64+5);
		TS_ASSERT(h.contains(32+5));
		TS_ASSERT(h.contains(128+5));
		h.erase(128+5);
		TS_ASSERT(h.contains(32+5));
		h.erase(32+5);
		TS_ASSERT(h.empty());
	}

	void test_iterator() {
		Common::OrderedHashMap<int, int> container;
		container[0] = 17;
		container[1] = 33;
		container[2] = 45;
		container[3] = 12;
		container[4] = 96;
		container.erase(1);
		container[1] = 42;
		container.erase(0);
		container.erase(1);

		int found = 0;
		for (auto i = container.begin(); i != container.end(); ++i) {
			int key = i->_key;
			TS_ASSERT(key >= 0 && key <= 4);
			TS_ASSERT(!(found & (1 << key)));
			found |= 1 << key;
		}
		TS_ASSERT(found == 16+8+4);

		found = 0;
		for (auto j = container.begin(); j != container.end(); ++j) {
			int key = j->_key;
			TS_ASSERT(key >= 0 && key <= 4);
			TS_ASSERT(!(found & (1 << key)));
			found |= 1 << key;
		}
		TS_ASSERT(found == 16+8+4);
	}

	// Tests that depend upon the iteration order
	void test_iteration_order_indexed_insertion() {
		pair testData[] = {
			{4, 33},
			{3, 17},
			{9, 2},
			{1, 129},
			{-1, -1}
		};
		Common::OrderedHashMap<int, int> container;
		int index = 0;
		while (testData[index].key != -1) {
			container[testData[index].key] = testData[index].value;
			index++;
		}
		index = 0;
		for (auto &element : container) {
			TS_ASSERT(element._key == testData[index].key);
			TS_ASSERT(element._value == testData[index].value);
			index++;
		}
	}

	void test_iteration_order_push_after_removal() {
		pair testData[] = {
			{4, 33},
			{3, 17},
			{9, 2},
			{1, 129},
			{-1, -1}
		};
		Common::OrderedHashMap<int, int> container;
		int index = 0;
		while (testData[index].key != -1) {
			container[testData[index].key] = testData[index].value;
			index++;
		}
		int deletionIndicies[] = {
			2, 1, 3, 0
		};
		int arraySize = sizeof(deletionIndicies) / sizeof(int);
		int expectedLength = arraySize;
		TS_ASSERT(container.size() == expectedLength);
		for (int i = 0; i < arraySize; i++) {
			int indexToDelete = deletionIndicies[i];
			container.erase(testData[indexToDelete].key);
			testData[indexToDelete].key = -1;
			expectedLength--;
			TS_ASSERT(container.size() == expectedLength);
			index = 0;
			for (auto &element : container) {
				if (testData[index].key == -1)
					continue;
				TS_ASSERT(element._key == testData[index].key);
				TS_ASSERT(element._value == testData[index].value);
				index++;
			}
		}
		TS_ASSERT(container.empty());
	}

	void test_iteration_order_after_removing_and_reinserting() {
		pair testData[] = {
			{4, 33},
			{3, 17},
			{9, 2},
			{1, 129},
			{-1, -1},
		};
		Common::OrderedHashMap<int, int> container;
		int index = 0;
		while (testData[index].key != -1) {
			container[testData[index].key] = testData[index].value;
			index++;
		}

		container.erase(testData[2].key);
		container[testData[2].key] = testData[2].value;
		int expectedOrder[] = {
			0, 1, 3, 2
		};

		index = 0;
		for (auto &element : container) {
			TS_ASSERT(element._key == testData[expectedOrder[index]].key);
			TS_ASSERT(element._value == testData[expectedOrder[index]].value);
			index++;
		}
	}

	void test_iteration_order_after_replacing_element() {
		// Element replacement will _not_ change the iteration order,
		// unless an explicit erase/insert is performed.
		pair testData[] = {
			{4, 33},
			{3, 17},
			{9, 2},
			{1, 129},
			{-1, -1},
		};
		Common::OrderedHashMap<int, int> container;
		int index = 0;
		while (testData[index].key != -1) {
			container[testData[index].key] = testData[index].value;
			index++;
		}

		container[testData[2].key] = testData[2].value;
		int expectedOrder[] = {
			0, 1, 2, 3
		};

		index = 0;
		for (auto &element : container) {
			TS_ASSERT(element._key == testData[expectedOrder[index]].key);
			TS_ASSERT(element._value == testData[expectedOrder[index]].value);
			index++;
		}
	}

	void test_assignment_operator() {
		Common::OrderedHashMap<int, int> srcContainer;
		Common::OrderedHashMap<int, int> dstContainer;
		srcContainer[0] = 1;
		dstContainer[2] = 3;
		dstContainer = srcContainer;
		TS_ASSERT(dstContainer.size() == 1);
		TS_ASSERT(dstContainer[0] == 1);
		TS_ASSERT(dstContainer.contains(2) == false);
		srcContainer[2] = 3; // Check that modifying the src doesn't affect the dst.
		TS_ASSERT(dstContainer.size() == 1);
		TS_ASSERT(dstContainer[0] == 1);
		TS_ASSERT(dstContainer.contains(2) == false);
	}

	void test_copy_constructor() {
		Common::OrderedHashMap<int, int> srcContainer;
		srcContainer[0] = 1;
		Common::OrderedHashMap<int, int> dstContainer(srcContainer);
		TS_ASSERT(dstContainer.size() == 1);
		TS_ASSERT(dstContainer[0] == 1);
		TS_ASSERT(dstContainer.contains(2) == false);
		srcContainer[2] = 3; // Check that modifying the src doesn't affect the dst.
		TS_ASSERT(dstContainer.size() == 1);
		TS_ASSERT(dstContainer[0] == 1);
		TS_ASSERT(dstContainer.contains(2) == false);
	}
};
