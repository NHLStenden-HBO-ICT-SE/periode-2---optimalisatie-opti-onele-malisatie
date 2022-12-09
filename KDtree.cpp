#include "precomp.h"
#include "KDtree.h"
#include <vector>

namespace Tmpl8
{
	struct Node {
		Tank* ptank;
		Node* left;
		Node* right;

		Node(Tank &tank)
		{
			ptank = &tank;
			left == nullptr;
			right == nullptr;
		}
	};

	Node* KDtree::insert(Tank* tank, Node* node)
	{
		if (node == nullptr)
		{
			node = new Node(&tank);
			return node;
		}


	}
}