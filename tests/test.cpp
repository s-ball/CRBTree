#include "gtest/gtest.h"
#include "rbtree.h"

namespace {
	// A stub comparison function (always returns equality)
	int stub_comp(const void* a, const void* b) {
		return 0;
	}
}


// Just test that RBinit is accessible and does the right thing
TEST(General, rbinit) {
	RBTree tree;
	RBinit(&tree, stub_comp);
	EXPECT_EQ(nullptr, tree.root);
	EXPECT_EQ(0, tree.black_depth);
	EXPECT_EQ(&stub_comp, tree.comp);
	EXPECT_EQ(0, RBvalidate(&tree));
}