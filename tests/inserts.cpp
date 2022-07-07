#include "gtest/gtest.h"
#include "rbtree.h"
#include <list>

using std::list;

class TestInsert: public ::testing::Test {
protected:
	RBTree tree;
	thread_local static list<int> expect_dele;

	TestInsert() {
		RBinit(&tree, compare);
	}

	void TearDown() override {
		RBdestroy(&tree, expect_dele.empty() ? nullptr : dele);
		EXPECT_TRUE(expect_dele.empty());
	}

	static int compare(const void* a, const void* b) {
		return (intptr_t)a - (intptr_t)b;
	}

	static void dele(const void* data) {
		EXPECT_EQ((void*) expect_dele.front(), data);
		expect_dele.pop_front();
	}
};

thread_local list<int> TestInsert::expect_dele;

TEST_F(TestInsert, test_1) {
	EXPECT_EQ(nullptr, RBinsert(&tree, (void *) 1, nullptr));
	EXPECT_EQ((void *) 1, RBinsert(&tree, (void*)1, nullptr));
	EXPECT_EQ(1, tree.black_depth);
	EXPECT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestInsert, test_bunch_red_levels) {
	int arr[] = { 8, 6, 10, 4, 2, 14, 12, 1, 5, 9, 13 };
	for (int i : arr) {
		EXPECT_EQ(nullptr, RBinsert(&tree, (void*)i, nullptr));
		ASSERT_EQ(0, RBvalidate(&tree));
	}
	EXPECT_EQ(2, tree.black_depth);
	expect_dele.clear();
	expect_dele.splice(expect_dele.end(), list<int>{1,2,5,6,4,9,10,13,14,12,8});
}
