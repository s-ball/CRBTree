#include "gtest/gtest.h"
extern "C" {
#include "rbinternal.h"
}

class TestInsertImpl : public ::testing::Test {
protected:
	RBTree tree;

	TestInsertImpl() {
		RBinit(&tree, compare);
	}

	static int compare(const void* a, const void* b) {
		return (intptr_t)a - (intptr_t)b;
	}
};

TEST_F(TestInsertImpl, SwapRed) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 0},
		{(void*)2, {nodes, nodes + 3}, 0},
		{(void*)3, {nullptr, nullptr}, 1},
		{(void*)4, {nodes + 2, nodes + 4}, 0},
		{(void*)6, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 1;
	tree.black_depth = 2;
	tree.count = 5;
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)5, nullptr));
	EXPECT_EQ(1, nodes[3].red);
	EXPECT_EQ(0, nodes[2].red);
	EXPECT_EQ(0, nodes[4].red);
	ASSERT_NE(nullptr, nodes[4].child[0]);
	EXPECT_EQ((void*)5, nodes[4].child[0]->data);
	EXPECT_EQ(0, RBvalidate(&tree));
}
TEST_F(TestInsertImpl, SimpleRotation) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 0},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nodes + 3}, 0},
		{(void*)4, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 1;
	tree.black_depth = 2;
	tree.count = 4;
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)5, nullptr));
	EXPECT_EQ(nodes[1].child[1], nodes + 3);
	EXPECT_EQ(nodes[3].child[0], nodes + 2);
	ASSERT_NE(nullptr, nodes[3].child[1]);
	EXPECT_EQ((void*)5, nodes[3].child[1]->data);
	EXPECT_EQ(1, nodes[3].child[1]->red);
	EXPECT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestInsertImpl, DoubleRotation) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 0},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nodes + 3}, 0},
		{(void*)5, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 1;
	tree.black_depth = 2;
	tree.count = 4;
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)4, nullptr));
	ASSERT_NE(nullptr, nodes[1].child[1]);
	EXPECT_EQ((void*)4, nodes[1].child[1]->data);
	EXPECT_EQ(0, nodes[1].child[1]->red);
	EXPECT_EQ(1, nodes[3].red);
	EXPECT_EQ(nodes + 2, nodes[1].child[1]->child[0]);
	EXPECT_EQ(nodes + 3, nodes[1].child[1]->child[1]);
	EXPECT_EQ(nullptr, nodes[2].child[0]);
	EXPECT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestInsertImpl, RedRoot) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 1},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nullptr}, 1},
	};
	tree.root = nodes+1;
	tree.black_depth = 1;
	tree.count = 3;
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)4, nullptr));
	EXPECT_EQ(nodes + 1, tree.root);
	EXPECT_EQ(nodes, nodes[1].child[0]);
	EXPECT_EQ(2, tree.black_depth);
	EXPECT_EQ(0, nodes[0].red);
	EXPECT_EQ(0, RBvalidate(&tree));
}

class TestValidate : public ::testing::Test {
protected:
	RBTree tree;

	TestValidate() {
		RBinit(&tree, compare);
	}

	static int compare(const void* a, const void* b) {
		return (intptr_t)a - (intptr_t)b;
	}
};

TEST_F(TestValidate, BadRoot) {
	tree.black_depth = 1;
	EXPECT_EQ(DEPTH_ERROR, RBvalidate(&tree));
}

TEST_F(TestValidate, BadDepth) {
	_RBNode nodes[] = {
		{nullptr,  {nullptr, nullptr}, 0},
	};
	tree.root = nodes;
	tree.black_depth = 0;
	EXPECT_EQ(DEPTH_ERROR, RBvalidate(&tree));
	tree.black_depth = 2;
	EXPECT_EQ(DEPTH_ERROR, RBvalidate(&tree));
}

TEST_F(TestValidate, RedRoot) {
	_RBNode nodes[] = {
		{nullptr,  {nullptr, nullptr}, 1},
	};
	tree.root = nodes;
	tree.black_depth = 1;
	EXPECT_EQ(RED_ROOT, RBvalidate(&tree));
}

TEST_F(TestValidate, RedViolation) {
	_RBNode nodes[] = {
		{(void*)1,  {nullptr, nullptr}, 0},
		{(void*)2,  {nodes, nodes + 3}, 0},
		{(void*)3,  {nullptr, nullptr}, 0},
		{(void*)4,  {nodes + 2, nodes + 4}, 1},
		{(void*)5,  {nullptr, nodes + 5}, 1},
		{(void*)6,  {nullptr, nullptr}, 0},
	};
	tree.root = nodes + 1;
	tree.black_depth = 1;
	tree.count = 6;
	EXPECT_EQ(RED_VIOLATION, RBvalidate(&tree));
}

TEST_F(TestValidate, BlackViolation) {
	_RBNode nodes[] = {
		{(void*)1,  {nullptr, nullptr}, 0},
		{(void*)2,  {nodes, nodes + 3}, 0},
		{(void*)3,  {nullptr, nullptr}, 0},
		{(void*)4,  {nodes + 2, nodes + 4}, 0},
		{(void*)5,  {nullptr, nodes + 5}, 1},
		{(void*)6,  {nullptr, nullptr}, 0},
	};
	tree.root = nodes + 1;
	tree.black_depth = 1;
	tree.count = 6;
	EXPECT_EQ(BLACK_VIOLATION, RBvalidate(&tree));
}

class TestRemoveImpl : public ::testing::Test {
protected:
	RBTree tree;

	TestRemoveImpl() {
		RBinit(&tree, compare);
	}

	static int compare(const void* a, const void* b) {
		return (intptr_t)a - (intptr_t)b;
	}
};

TEST_F(TestRemoveImpl, test_123) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 1},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 1;
	tree.black_depth = 1;
	tree.count = 3;
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)1, RBremove(&tree, (void*)1));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)2, RBremove(&tree, (void*)2));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)3, RBremove(&tree, (void*)3));
	ASSERT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestRemoveImpl, test_321) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 1},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 1;
	tree.black_depth = 1;
	tree.count = 3;
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)3, RBremove(&tree, (void*)3));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)2, RBremove(&tree, (void*)2));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)1, RBremove(&tree, (void*)1));
	ASSERT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestRemoveImpl, test_231) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 1},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 1;
	tree.black_depth = 1;
	tree.count = 3;
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)2, RBremove(&tree, (void*)2));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)3, RBremove(&tree, (void*)3));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)1, RBremove(&tree, (void*)1));
	ASSERT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestRemoveImpl, red_ancestor_simple) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 0},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nullptr}, 0},
		{(void*)4, {nodes + 1, nodes + 5}, 1},
		{(void*)5, {nullptr, nullptr}, 0},
		{(void*)6, {nodes + 4, nodes + 6}, 0},
		{(void*)7, {nullptr, nullptr}, 0},
		{(void*)8, {nodes + 3, nodes + 11}, 0},
		{(void*)9, {nullptr, nullptr}, 0},
		{(void*)10, {nodes + 8, nodes + 10}, 1},
		{(void*)11, {nullptr, nullptr}, 0},
		{(void*)12, {nodes + 9, nodes + 13}, 0},
		{(void*)13, {nullptr, nullptr}, 1},
		{(void*)14, {nodes + 12, nodes + 14}, 0},
		{(void*)15, {nullptr, nullptr}, 1},
	};
	tree.root = nodes + 7;
	tree.black_depth = 3;
	tree.count = 15;
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)5, RBremove(&tree, (void*)5));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ(3, tree.black_depth);
	EXPECT_EQ(nodes + 7, tree.root);
}

TEST_F(TestRemoveImpl, red_ancestor_rotation) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 0},
		{(void*)2, {nodes, nodes + 2}, 0},
		{(void*)3, {nullptr, nullptr}, 0},
		{(void*)4, {nodes + 1, nodes + 7}, 1},
		{(void*)5, {nullptr, nullptr}, 0},
		{(void*)6, {nodes + 4, nodes + 6}, 1},
		{(void*)7, {nullptr, nullptr}, 0},
		{(void*)8, {nodes + 5, nodes + 9}, 0},
		{(void*)9, {nullptr, nullptr}, 1},
		{(void*)10, {nodes + 8, nodes + 10}, 0},
		{(void*)11, {nullptr, nullptr}, 1},
		{(void*)12, {nodes + 3, nodes + 13}, 0},
		{(void*)13, {nullptr, nullptr}, 0},
		{(void*)14, {nodes + 12, nodes + 14}, 0},
		{(void*)15, {nullptr, nullptr}, 0},
	};
	tree.root = nodes + 11;
	tree.black_depth = 3;
	tree.count = 15;
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ((void*)5, RBremove(&tree, (void*)5));
	ASSERT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ(3, tree.black_depth);
	EXPECT_EQ(nodes + 11, tree.root);
	EXPECT_EQ(nodes + 3, nodes[11].child[0]);
}

#ifdef _TEST
// Test the implementation of paint_child_red even if it is internal
extern "C" RBNode * paint_child_red(RBNode*, int);

class TestPaintRed : public ::testing::Test {
protected:
	struct _RBTree tree;

	TestPaintRed() {
		RBinit(&tree, compare);
	}

	static int compare(const void* a, const void* b) {
		return (intptr_t)a - (intptr_t)b;
	}
};

TEST_F(TestPaintRed, simple) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nodes + 1}, 0},
		{(void*)2, {nullptr, nullptr}, 0}
	};
	tree.root = nodes;
	tree.black_depth = 1;
	tree.count = 2;
	ASSERT_EQ(nodes, paint_child_red(nodes, 1));
	EXPECT_EQ(nodes + 1, nodes->child[1]);
	EXPECT_TRUE(nodes[1].red);
	EXPECT_EQ(0, RBvalidate(&tree));
	EXPECT_EQ(1, tree.black_depth);
}

TEST_F(TestPaintRed, two_red) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nodes + 1}, 0},
		{(void*)3, {nodes + 2, nodes + 3}, 0},
		{(void*)2, {nullptr, nullptr}, 1},
		{(void*)4, {nullptr, nullptr}, 1},
	};
	tree.root = paint_child_red(nodes, 1);
	tree.count = 4;
	EXPECT_EQ(nodes, nodes[1].child[0]);
	EXPECT_TRUE(nodes[1].red);
	tree.root->red = 0;
	tree.black_depth = 2;
	EXPECT_EQ(0, RBvalidate(&tree));
}


TEST_F(TestPaintRed, inner_red) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nullptr}, 0},
		{(void*)2, {nodes, nodes + 5}, 0},
		{(void*)3, {nullptr, nullptr}, 0},
		{(void*)4, {nodes + 2, nodes + 4}, 1},
		{(void*)5, {nullptr, nullptr}, 0},
		{(void*)6, {nodes + 3, nodes + 6}, 0},
		{(void*)7, {nullptr, nullptr}, 0},
	};
	tree.count = 7;
	tree.root = paint_child_red(nodes+1, 1);
	EXPECT_EQ(nodes+1, nodes[3].child[0]);
	EXPECT_TRUE(nodes[3].red);
	tree.root->red = 0;
	tree.black_depth = 3;
	EXPECT_EQ(0, RBvalidate(&tree));
}

#endif // _TEST
