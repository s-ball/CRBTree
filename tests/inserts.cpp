#include "gtest/gtest.h"
#include "rbtree.h"

class TestInsert: public ::testing::Test {
protected:
	RBTree tree;

	TestInsert() {
		RBinit(&tree, compare);
	}

	static int compare(const void* a, const void* b) {
		return (intptr_t)a - (intptr_t)b;
	}
};

TEST_F(TestInsert, test_1) {
	EXPECT_EQ(nullptr, RBinsert(&tree, (void *) 1, nullptr));
	EXPECT_EQ((void *) 1, RBinsert(&tree, (void*)1, nullptr));
	EXPECT_EQ(1, tree.black_depth);
	EXPECT_EQ(0, RBvalidate(&tree));
}