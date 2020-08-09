/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>

#include "funcutils.h"
#include "logging.h"

namespace Ossium
{

    template<class T>
    class Tree;

    /// A simple container of data and it's child data.
    template<class T>
    struct Node
    {
        friend class Tree<T>;

        /// The actual data stored in this node.
        T data;

        /// Pointer to parent node; null if this is a root node.
        Node<T>* parent = nullptr;

        /// Children nodes; empty if this is a leaf node
        std::vector<Node<T>*> children;

        /// Unique identifier
        int id = 0;

        /// How deep this node is.
        Uint32 depth = 0;

        /// The index of this node in the parent node's array of children.
        unsigned int childIndex = 0;

    private:
        // Only the Tree class can instantiate nodes
        Node<T>() = default;
        NOCOPY(Node<T>);

    };

    /// A simple hierarchical data structure.
    template<class T>
    class OSSIUM_EDL Tree
    {
    public:
        friend struct Node<T>;

        typedef std::function<bool(Node<T>*)> FindNodePredicate;

        Tree()
        {
            total = 0;
            nextId = 0;
        }

        ~Tree()
        {
            Clear();
        }

        /// Adds data to the tree. If parent is null, adds to root node children. Returns pointer to the node
        Node<T>* Insert(T data, Node<T>* parent = nullptr)
        {
            return Insert(data, parent, parent != nullptr ? parent->children.size() : 0);
        }

        /// Creates a node and inserts at a particular index in the specified parent's children. If parent is null, the node is inserted in the roots array at the given index.
        /// All sibling node child indexes are updated accordingly.
        Node<T>* Insert(T data, Node<T>* parent, unsigned int childIndex)
        {
            Node<T>* node = new Node<T>();
            node->data = data;
            node->parent = parent;
            node->id = nextId;

            if (parent != nullptr)
            {
                DEBUG_ASSERT(childIndex >= 0 && childIndex <= parent->children.size(), "Tree insertion error - child index out of range!");
                node->childIndex = parent->children.size();
                parent->children.push_back(node);
                node->depth = parent->depth + 1;
            }
            else
            {
                node->childIndex = roots.size();
                roots.push_back(node);
                node->depth = 0;
            }
            updateFlattened = true;
            total++;
            nextId++;

            return node;
        }

        /// Removes a specified data node and all nodes below it
        bool Remove(Node<T>* node)
        {
            if (node == nullptr)
            {
                Log.Warning("(!) Attempted to remove node that is already null.");
                return false;
            }
            std::vector<Node<T>*> all = GetAllBelow(node);
            node->children.clear();
            /// Delete all nodes below the source node
            for (auto i = all.begin(); i != all.end(); i++)
            {
                if (*i != nullptr)
                {
                    delete *i;
                    *i = nullptr;
                }
                total--;
            }
            std::vector<Node<T>*>& children = node->parent != nullptr ? node->parent->children : roots;
            //Log.Info("Removing child {0} from {1}", node->childIndex, children);
            auto index = children.begin() + node->childIndex;
            for (auto i = index + 1; i < children.end(); i++)
            {
                (*i)->childIndex--;
                //Log.Info("Reduced child index from {0} to {1}", (*i)->childIndex + 1, (*i)->childIndex);
            }
            children.erase(index);
            updateFlattened = true;
            delete node;
            node = nullptr;
            total--;
            return true;
        }

        /// Finds and removes first found data node that meets the predicate condition.
        bool Remove(FindNodePredicate predicate)
        {
            /// Short circuit evalutation
            Node<T>* node = Find(predicate);
            return Remove(node);
        }

        /// Repositions this node in the tree. Specifying a child index greater than zero will insert this at the index position rather than just appending to the new parent's children.
        void SetParent(Node<T>* node, Node<T>* parent, int index = -1)
        {
            Log.Info("\nInsert index = {0}, childIndex = {1}\n\n", index, node->childIndex);
            if (node->parent != nullptr)
            {
                DEBUG_ASSERT(node->childIndex < node->parent->children.size(), "err");
                // First, update the current parent of the node
                node->parent->children.erase(node->parent->children.begin() + node->childIndex);
                for (auto i = node->parent->children.begin() + node->childIndex, counti = node->parent->children.end(); i < counti; i++)
                {
                    // Update children
                    (*i)->childIndex--;
                }
            }
            else
            {
                roots.erase(roots.begin() + node->childIndex);
                for (auto i = roots.begin() + node->childIndex, counti = roots.end(); i < counti; i++)
                {
                    // Update children
                    (*i)->childIndex--;
                }
            }
            // Now reassign the parent
            node->parent = parent;
            if (node->parent != nullptr)
            {
                if (index >= 0)
                {
                    node->parent->children.insert(parent->children.begin() + index, node);
                    node->childIndex = index;
                    for (auto i = node->parent->children.begin() + node->childIndex + 1, counti = node->parent->children.end(); i < counti; i++)
                    {
                        // Update children
                        (*i)->childIndex++;
                    }
                }
                else
                {
                    node->childIndex = node->parent->children.size();
                    node->parent->children.push_back(node);
                }
                node->depth = node->parent->depth + 1;
            }
            else
            {
                node->depth = 0;
                node->childIndex = 0;
                // TODO: roots!
            }
        }

        /// Removes all nodes from the tree
        void Clear()
        {
            // Delete everything
            for (auto node : GetFlatTree())
            {
                if (node != nullptr)
                {
                    delete node;
                }
            }
            roots.clear();
            flatTree.clear();
        }

        typedef std::function<void(Node<T>*)> NodeOp;

        /// Depth-first tree traversal
        void Walk(NodeOp walkFunc, Node<T>* fromNode = nullptr)
        {
            if (fromNode == nullptr)
            {
                for (auto child : roots)
                {
                    RecursiveWalk(walkFunc, child);
                }
            }
            else
            {
                RecursiveWalk(walkFunc, fromNode);
            }
        }

        /// Overload takes an additional function that is called when walking back up the tree.
        void Walk(NodeOp walkDown, NodeOp walkUp, Node<T>* fromNode = nullptr)
        {
            if (fromNode == nullptr)
            {
                for (auto child : roots)
                {
                    RecursiveWalk(walkDown, walkUp, child);
                }
            }
            else
            {
                RecursiveWalk(walkDown, walkUp, fromNode);
            }
        }

        /// Breadth-first tree traversal
        void WalkBreadth(NodeOp walkFunc)
        {
            std::queue<Node<T>*> nodes;
            for (auto node : roots)
            {
                nodes.push(node);
            }
            while (!nodes.empty())
            {
                walkFunc(nodes.front());
                for (auto child : nodes.front()->children)
                {
                    nodes.push(child);
                }
                nodes.pop();
            }
        }

        /// Searches for a node that meets the provided predicate condition.
        Node<T>* Find(FindNodePredicate predicate, Node<T>* source = nullptr)
        {
            if (source == nullptr)
            {
                /// Search from roots
                for (auto i = roots.begin(); i != roots.end(); i++)
                {
                    DEBUG_ASSERT(*i != nullptr, "Root node is null!");
                    Node<T>* node = RecursiveFind(predicate, *i);
                    if (node != nullptr)
                    {
                        return node;
                    }
                }
            }
            else
            {
                /// Search from parent
                return RecursiveFind(predicate, source);
            }
            return nullptr;
        }

        /// Gets pointers to all nodes in the tree (below the source node) which meet the predicate condition; pretty slow!
        void FindAll(FindNodePredicate predicate, Node<T>* source, std::vector<Node<T>*>& output)
        {
            /// While we're iterating over everything we may as well rebuild the flatTree
            flatTree.clear();
            flatTree.reserve(total);
            RecursiveFindAll(predicate, source, output);
            updateFlattened = false;
        }

        /// Defaults to roots as the source
        std::vector<Node<T>*> FindAll(FindNodePredicate predicate)
        {
            std::vector<Node<T>*> nodes;
            /// While we're iterating over everything we may as well rebuild the flatTree
            flatTree.clear();
            flatTree.reserve(total);

            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                RecursiveFindAll(predicate, *i, nodes);
            }
            updateFlattened = false;
            return nodes;
        }

        /// A version of FindAll() that matches multiple predicates; more efficient as it only walks the tree once,
        /// but can match more than a single predicate at a time.
        std::vector<Node<T>*> FindAll(const std::vector<FindNodePredicate>& predicates)
        {
            std::vector<Node<T>*> nodes;
            flatTree.clear();
            flatTree.reserve(total);

            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                RecursiveFindAll(predicates, *i, nodes);
            }

            updateFlattened = false;
            return nodes;
        }

        /// Returns all nodes in the tree below a source node; includeSource adds the source node to the output vector
        std::vector<Node<T>*> GetAllBelow(Node<T>* source, bool includeSource = false)
        {
            std::vector<Node<T>*> all;
            if (source == nullptr)
            {
                Log.Warning("(!) Attempted to retrieve all below a null node.");
                return all;
            }
            if (includeSource)
            {
                RecursiveGetAll(source, all);
            }
            else
            {
                for (auto i = source->children.begin(); i != source->children.end(); i++)
                {
                    RecursiveGetAll(*i, all);
                }
            }
            return all;
        }

        /// Get every single node in the tree.
        /// It's preferable to use GetFlatTree() instead of this method, as this recurses through the entire tree
        /// instead of returning the cached vector array.
        std::vector<Node<T>*> GetAll()
        {
            std::vector<Node<T>*> all;
            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                RecursiveGetAll(*i, all);
            }
            return all;
        }

        /// Returns all nodes in the tree grouped by depth.
        std::vector<std::vector<Node<T>*>> GetAllDepths()
        {
            std::queue<Node<T>*> nodes;

            std::vector<std::vector<Node<T>*>> groups;
            for (auto node : roots)
            {
                nodes.push(node);
                groups.back().push_back(node);
            }
            while (!nodes.empty())
            {
                if (!nodes.front().children.empty() && nodes.front().children.front().depth >= groups.size())
                {
                    groups.push_back({});
                }
                for (auto child : nodes.front().children)
                {
                    nodes.push(child);
                    groups.back().push_back(child);
                }
                nodes.pop();
            }
        }

        /// Returns a reference to the cached, 'flattened' version of the tree.
        /// Depending on the current build mode, this will either be ordered by depth-first or breadth-first nodes (depth-first by default).
        std::vector<Node<T>*>& GetFlatTree()
        {
            if (updateFlattened)
            {
                flatTree.clear();
                /// Preallocate memory for some performance gains. Use total + 1 as we include the root node.
                flatTree.reserve(total);

                if (buildFlattenedDepthFirst)
                {
                    Walk([&] (Node<T>* node) {
                        flatTree.push_back(node);
                    });
                }
                else
                {
                    WalkBreadth([&] (Node<T>* node) {
                        flatTree.push_back(node);
                    });
                }

                updateFlattened = false;
                //Log.Debug("Flattened tree = {0}", flatTree);
            }
            return flatTree;
        }

        /// Sets the traversal method to use when building the flat tree, which dictates the order that the elements appear in the GetFlatTree() array.
        void SetFlatTreeBuildMode(bool depthFirst)
        {
            if (depthFirst != buildFlattenedDepthFirst)
            {
                updateFlattened = true;
                buildFlattenedDepthFirst = depthFirst;
            }
        }

        /// Returns the total number of nodes in the tree.
        unsigned int Size()
        {
            return total;
        }

        /// Sets the lazy-generated ID. Useful when ensuring unique ids after serialisation.
        void SetGeneration(int generation)
        {
            nextId = generation;
        }

        int GetGeneration()
        {
            return nextId;
        }

        /// Returns a vector array of the root nodes.
        std::vector<Node<T>*> GetRoots()
        {
            return roots;
        }

    private:
        /// Total number of nodes, NOT including the root node
        unsigned int total;

        /// Next node id
        int nextId;

        /// Root nodes of the tree
        std::vector<Node<T>*> roots;

        /// A 'flat' version of the tree with references to all nodes
        std::vector<Node<T>*> flatTree;

        /// Whether or not the tree should re-calculate the flatTree array next time GetFlattened() is called
        bool updateFlattened;

        /// Should the flat tree be built using depth-first or breadth-first traversal?
        bool buildFlattenedDepthFirst = true;

        /// Walks recursively through the tree and operates on all nodes, depth first.
        void RecursiveWalk(NodeOp& nodeOp, Node<T>* node)
        {
            nodeOp(node);
            for (auto child : node->children)
            {
                RecursiveWalk(nodeOp, child);
            }
        }

        /// Walks recursively down and operates on all the nodes, depth first,
        /// but also operates on all the nodes on the walk back up the tree with the second function.
        void RecursiveWalk(NodeOp& walkDown, NodeOp& walkUp, Node<T>* node)
        {
            walkDown(node);
            for (auto child : node->children)
            {
                RecursiveWalk(walkDown, walkUp, child);
            }
            walkUp(node);
        }

        /// Recursive search method
        Node<T>* RecursiveFind(FindNodePredicate& predicate, Node<T>* parent)
        {
            if (predicate(parent))
            {
                return parent;
            }
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                Node<T>* found = RecursiveFind(predicate, *i);
                if (found != nullptr)
                {
                    return found;
                }
            }
            return nullptr;
        }

        void RecursiveFindAll(FindNodePredicate& predicate, Node<T>* parent, std::vector<Node<T>*>& output)
        {
            /// We can recalculate the flatTree tree while we're here
            if (updateFlattened)
            {
                flatTree.push_back(parent);
            }
            if (predicate(parent))
            {
                output.push_back(parent);
            }
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                RecursiveFindAll(predicate, *i, output);
            }
        }

        void RecursiveFindAll(const std::vector<FindNodePredicate>& predicates, Node<T>* parent, std::vector<Node<T>*>& output)
        {
            if (updateFlattened)
            {
                flatTree.push_back(parent);
            }
            for (auto predicate = predicates.begin(); predicate != predicates.end(); predicate++)
            {
                if ((*predicate)(parent))
                {
                    output.push_back(parent);
                    break;
                }
            }
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                RecursiveFindAll(predicates, *i, output);
            }
        }

        /// Recursively returns pointers to ALL nodes below some source node
        void RecursiveGetAll(Node<T>* source, std::vector<Node<T>*>& output)
        {
            output.push_back(source);
            for (auto i = source->children.begin(); i != source->children.end(); i++)
            {
                RecursiveGetAll(*i, output);
            }
        }

    };

}

#endif // TREE_H
