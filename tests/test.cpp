#include "gtest/gtest.h"
#include "rbtree.h"
#include <random>
#include <algorithm>
#include <iostream>
#include<sstream>


namespace {
	// A stub comparison function (always returns equality)
	int stub_comp(const void* a, const void* b) {
		return 0;
	}

	int int_comp(const void* a, const void* b) {
		return (int)(intptr_t)a - (int)(intptr_t)b;
	}

	int int_comp_err(const void* a, const void* b, int* err) {
		if (0 == a || 0 == b) {
			*err = 1;
			return 0;
		}
		return int_comp(a, b);
	}
}


// Just test that RBinit is accessible and does the right thing
TEST(General, rbinit) {
	RBTree tree;
	RBinit(&tree, stub_comp);
	EXPECT_EQ(nullptr, tree.root);
	EXPECT_EQ(0, tree.black_depth);
	EXPECT_EQ((int (*)())(& stub_comp), tree.comp);
	EXPECT_EQ(0, RBvalidate(&tree));
}

class TestGlobal : public ::testing::Test {
protected:

	static constexpr size_t SIZE = 1000;
	static std::mt19937 rg;
	static std::vector<int> arr;
	static constexpr size_t NB = 128;

	RBTree tree;

	static void SetUpTestCase() {
		arr.reserve(SIZE);
		for (int i = 1; i <= SIZE; i++) {
			arr.push_back(i);
		}

	}
	TestGlobal() {
		RBinit(&tree, compare);
		std::shuffle(arr.begin(), arr.end(), rg);
	}

	~TestGlobal() {
		RBdestroy(&tree, nullptr);
	}

	static int compare(const void* a, const void* b) {
		return (int)(intptr_t)a - (int)(intptr_t)b;
	}

	static void dump(void* data, char* buff) {
		std::stringstream ss;
		int val = (int)(intptr_t)data;
		snprintf(buff, 4, "%d", val);
	}
};

std::vector<int> TestGlobal::arr;
std::mt19937 TestGlobal::rg(0);

TEST_F(TestGlobal, nb_run) {
	for (unsigned j = 0; j < NB; j++) {
		for (int i : arr) {
			int error;
			RBinsert(&tree, (void*) (intptr_t) i, &error);
			ASSERT_EQ(0, error);
			ASSERT_EQ(0, RBvalidate(&tree));
		}
		std::shuffle(arr.begin(), arr.end(), rg);
		for (int i : arr) {
			ASSERT_EQ((void*)(intptr_t)i, RBremove(&tree, (void*)(intptr_t)i));
			ASSERT_EQ(0, RBvalidate(&tree));
		}
		ASSERT_EQ(0, tree.black_depth);
	}
}

TEST_F(TestGlobal, To32) {
	for (int i = 1; i < 32; i++) {
		int error;
		ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)i, &error));
		EXPECT_EQ(0, error);
		ASSERT_EQ(0, RBvalidate(&tree));
	}
	RBIter* iter = RBfirst(&tree);
	for (int i = 1; i < 32; i++) {
		ASSERT_EQ((void*)(intptr_t)i, RBnext(iter));
	}
	RBiter_release(iter);
	RBdestroy(&tree, nullptr);
}

TEST_F(TestGlobal, Dump32) {
	for (int i = 1; i < 32; i++) {
		int error;
		ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)i, &error));
		EXPECT_EQ(0, error);
		ASSERT_EQ(0, RBvalidate(&tree));
	}
	RBdump(&tree, 4, dump);
	RBdestroy(&tree, nullptr);
}

class TestIntTree : public ::testing::Test {
protected:
	RBTree tree;

	TestIntTree() {
		RBinit(&tree, int_comp);
	}

	~TestIntTree() {
		RBdestroy(&tree, nullptr);
	}
};

TEST_F(TestIntTree, EmptyTree) {
	RBIter* iter = RBfirst(&tree);
	ASSERT_NE(nullptr, iter);
	EXPECT_EQ(nullptr, RBnext(iter));
	RBiter_release(iter);
}

TEST_F(TestIntTree, KeyError) {
	ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)1, nullptr));
	EXPECT_EQ(nullptr, RBremove(&tree, (void*)(intptr_t)2));
	EXPECT_EQ(1, tree.count);
}

TEST_F(TestIntTree, CountError) {
	ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)1, nullptr));
	EXPECT_EQ(1, tree.count);
	tree.count = 2;
	EXPECT_EQ(COUNT_ERROR, RBvalidate(&tree));
}

TEST_F(TestIntTree, Find) {
	ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)1, nullptr));
	ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)2, nullptr));
	ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)3, nullptr));
	EXPECT_EQ((void*)(intptr_t)3, RBfind(&tree, (void*)(intptr_t)3));
	EXPECT_EQ(nullptr, RBfind(&tree, (void*)(intptr_t)4));
}

class TestSearch : public TestIntTree {
protected:
	void SetUp() {
		int vals[] = {8, 4, 12, 2, 6, 10, 14};
		for (int i : vals) {
			ASSERT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t)i, nullptr));
		}
	}
};

TEST_F(TestSearch, AfterLast) {
	RBIter *iter = RBsearch(&tree, (void*)(intptr_t)15);
	ASSERT_NE(nullptr, iter);
	ASSERT_EQ(nullptr, RBnext(iter));
	RBiter_release(iter);
}

TEST_F(TestSearch, Found) {
	RBIter* iter = RBsearch(&tree, (void*)(intptr_t)10);
	ASSERT_NE(nullptr, iter);
	EXPECT_EQ((void*)(intptr_t)10, RBnext(iter));
	EXPECT_EQ((void*)(intptr_t)12, RBnext(iter));
	EXPECT_EQ((void*)(intptr_t)14, RBnext(iter));
	ASSERT_EQ(nullptr, RBnext(iter));
	RBiter_release(iter);
}
TEST_F(TestSearch, NotFound) {
	RBIter* iter = RBsearch(&tree, (void*)(intptr_t)9);
	ASSERT_NE(nullptr, iter);
	EXPECT_EQ((void*)(intptr_t)10, RBnext(iter));
	EXPECT_EQ((void*)(intptr_t)12, RBnext(iter));
	EXPECT_EQ((void*)(intptr_t)14, RBnext(iter));
	ASSERT_EQ(nullptr, RBnext(iter));
	RBiter_release(iter);
}

TEST(TestVersion, NotNull) {
	const unsigned char* version = RBversion();
	int null = 1;
	for (int i = 0; i < 4; i++) {
		if (0 != version[i]) {
			null = 0;
			break;
		}
	}
	ASSERT_EQ(0, null);
}

TEST(TestComp3, rbinit2) {
	RBTree tree;
	RBinit2(&tree, int_comp_err);
	ASSERT_EQ((int (*)()) &int_comp_err, tree.comp);
}

TEST(TestComp3, insert) {
	RBTree tree;
	RBinit2(&tree, int_comp_err);
	int arr[] = { 1, 2, 3 };
	for (int i : arr) {
		int err = 0;
		EXPECT_EQ(nullptr, RBinsert(&tree, (void*)(intptr_t) i, &err));
		EXPECT_EQ(0, err);
		EXPECT_EQ(nullptr, RBinsert(&tree, nullptr, &err));
		EXPECT_NE(0, err);
	}
	EXPECT_EQ(3, tree.count);
	RBdestroy(&tree, nullptr);
}

class TestClone : public TestIntTree {
protected:
	static int count;

	TestClone() : TestIntTree() {
		count = 0;
	}

	static void* process(void* const data) {
		count += 1;
		return data;
	}
};

int TestClone::count;

TEST_F(TestClone, clone15) {
	for (int i = 1; i <= 15; i++) {
		RBinsert(&tree, (void*)(intptr_t)i, nullptr);
	}
	RBTree* copy = RBclone(&tree, process);
	EXPECT_EQ(15, count);
	EXPECT_EQ(15, copy->count);
	EXPECT_NE(tree.root, copy->root);
	EXPECT_EQ(0, RBvalidate(copy));
	RBdestroy(copy, nullptr);
	free(copy);
}