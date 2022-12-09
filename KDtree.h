#pragma once
namespace Tmpl8
{

	class KDtree{
	public:
		KDtree();
		Node* insert(Tank* tank, Node* node);

		Node* root;
	private:
	};
}