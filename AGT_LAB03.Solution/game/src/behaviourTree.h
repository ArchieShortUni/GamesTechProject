#pragma once
#include <engine.h>
#include <iostream>
#include <list>
#include <vector>
#include <stack>
#include <initializer_list>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include "game_enums.h"

class BehaviourTree {  // Note:  A proper copy constructor and assignment operator should be defined, since the implicit ones use shallow copies only.
public:
	class Node {  // This class represents each node in the behaviour tree.
	public:
		virtual bool run() = 0;
	};

	class CompositeNode : public Node {  //  This type of Node follows the Composite Pattern, containing a list of other Nodes.
	private:
		std::vector<Node*> children;
		
	public:
		const std::vector<Node*>& getChildren() const { return children; }
		void addChild(Node* child) { children.emplace_back(child); }
		void addChildren(std::initializer_list<Node*>&& newChildren) { for (Node* child : newChildren) addChild(child); }
		template <typename CONTAINER>
		void addChildren(const CONTAINER& newChildren) { for (Node* child : newChildren) addChild(child); }
	protected:
		std::vector<Node*> childrenShuffled() const {
			std::random_device rd;
			std::mt19937 generator(rd()); std::vector<Node*> temp = children;  std::shuffle(temp.begin(), temp.end(), generator);  return temp; }
	};

	class Selector : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // The generic Selector implementation
				if (child->run())  // If one child succeeds, the entire operation run() succeeds.  Failure only results if all children fail.
					return true;
			}
			return false;  // All children failed so the entire run() operation fails.
		}
	};

	class RandomSelector : public CompositeNode {  // RandomSelector operates as a Selector, but in a random order instead of from first child to last child.
	public:
		virtual bool run() override {
			for (Node* child : childrenShuffled()) {  // The order is shuffled
				if (child->run())
					return true;
			}
			return false;
		}
	};

	class Sequence : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // The generic Sequence implementation.
				if (!child->run())  // If one child fails, then enter operation run() fails.  Success only results if all children succeed.
					return false;
			}
			return true;  // All children suceeded, so the entire run() operation succeeds.
		}
	};

	class Root : public Node {
	private:
		Node* child;
		friend class BehaviourTree;
		void setChild(Node* newChild) { child = newChild; }
		virtual bool run() override { return child->run(); }
	};
private:
	Root* root;
public:
	BehaviourTree() : root(new Root) {}
	void setRootChild(Node* rootChild) const { root->setChild(rootChild); }
	bool run() const { return root->run(); }
};

//All code above this point is from the online blog referenced in my report

class Action : public BehaviourTree::Node {
private:
	game_enums::state state;
	bool& outcome;
	game_enums::state& last_state;
public:
	Action(const game_enums::state newstate, bool& prob, game_enums::state& l_state) : state(newstate), outcome(prob), last_state(l_state) {}
private:
	virtual bool run() override {
		last_state = state;
		return outcome;
	}
};

