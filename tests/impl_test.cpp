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
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)5, nullptr));
	EXPECT_EQ(nodes[1].child[1], nodes + 3);
	EXPECT_EQ(nodes[3].child[0], nodes + 2);
	ASSERT_NE(nullptr, nodes[3].child[1]);
	EXPECT_EQ((void*)5, nodes[3].child[1]->data);
	EXPECT_EQ(0, nodes[3].child[1]->red);
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
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)4, nullptr));
	ASSERT_NE(nullptr, nodes[1].child[1]);
	EXPECT_EQ((void*)4, nodes[1].child[1]->data);
	EXPECT_EQ(1, nodes[1].child[1]->red);
	EXPECT_EQ(0, nodes[3].red);
	EXPECT_EQ(nodes + 2, nodes[1].child[1]->child[0]);
	EXPECT_EQ(nodes + 3, nodes[1].child[1]->child[1]);
	EXPECT_EQ(nullptr, nodes[2].child[0]);
	EXPECT_EQ(0, RBvalidate(&tree));
}

TEST_F(TestInsertImpl, RedRoot) {
	_RBNode nodes[] = {
		{(void*)1, {nullptr, nodes + 1}, 0},
		{(void*)2, {nullptr, nullptr}, 1},
	};
	tree.root = nodes;
	tree.black_depth = 1;
	EXPECT_EQ(nullptr, RBinsert(&tree, (void*)3, nullptr));
	EXPECT_EQ(nodes + 1, tree.root);
	EXPECT_EQ(nodes, nodes[1].child[0]);
	EXPECT_EQ(2, tree.black_depth);
	EXPECT_EQ(0, nodes[1].red);
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
	EXPECT_EQ(BLACK_VIOLATION, RBvalidate(&tree));
}

