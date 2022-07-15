#include "gtest/gtest.h"
#include "rbtree.h"
#include <random>
#include <algorithm>
#include <iostream>


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

class TestGlobal : public ::testing::Test {
protected:

	static constexpr size_t SIZE = 1000;
	static std::mt19937 rg;
	static std::vector<int> arr;
	static constexpr size_t NB = 128;

	RBTree tree;

	static void SetUpTestCase() {
		for (size_t i = 1; i <= SIZE; i++) {
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
		return (intptr_t)a - (intptr_t)b;
	}

};

std::vector<int> TestGlobal::arr{TestGlobal::SIZE};
std::mt19937 TestGlobal::rg(0);

TEST_F(TestGlobal, nb_run) {
	for (unsigned i = 0; i < NB; i++) {
		for (int i : arr) {
			int error;
			if (i == 784) {
				std::cout << " " << i;
			}
			else {
				std::cout << " " << i;
			}
			RBinsert(&tree, (void*) i, &error);
			ASSERT_EQ(0, error);
			ASSERT_EQ(0, RBvalidate(&tree));
		}
		std::shuffle(arr.begin(), arr.end(), rg);
		for (int i : arr) {
			ASSERT_EQ((void*)i, RBremove(&tree, (void*)i));
			ASSERT_EQ(0, RBvalidate(&tree));
		}
		ASSERT_EQ(0, tree.black_depth);
	}
}