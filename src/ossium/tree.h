#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

#include "basics.h"

using namespace std;

namespace Ossium
{

    template<class T>
    class Tree;

    /// A simple container of data and it's child data
    template<class T>
    struct Node
    {
        friend class Tree<T>;

        /// Name given to this node
        string name;
        /// The actual data stored in the node
        T data;
        /// Pointer to parent node; null if this is the root node
        Node<T>* parent;
        /// Children nodes; empty if this is a leaf node
        vector<Node<T>*> children;
        /// Unique identifier
        int id;
    };

    /// A simple hierarchical data structure.
    template<class T>
    class Tree
    {
    public:
        friend struct Node<T>;

        Tree()
        {
            total = 0;
            nextId = 0;
        }

        ~Tree()
        {
            clear();
        }

        /// Adds data to the tree. If parent is null, adds to root node children. Returns pointer to the node
        Node<T>* add(string name, T data, Node<T>* parent = nullptr)
        {
            Node<T>* node = new Node<T>();
            node->name = name;
            node->data = data;
            node->parent = parent;
            add(node);
            return node;
        }

        void add(Node<T>* node)
        {
            if (node == nullptr)
            {
                return;
            }
            node->id = nextId;
            if (node->name.length() < 1)
            {
                node->name = "Node[" + ToString(node->id) + "]";
            }
            if (node->parent != nullptr)
            {
                node->parent->children.push_back(node);
            }
            else
            {
                roots.push_back(node);
            }
            /// No need to add to flatTree array if it's to be recalulated; otherwise add to array
            if (!updateFlattened)
            {
                flatTree.push_back(node);
            }
            total++;
            nextId++;
        }

        /// Removes a specified data node and all nodes below it
        bool remove(Node<T>* node)
        {
            /// Never remove the root node
            if (node == nullptr)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "(!) Attempted to remove node that is already null.");
                return false;
            }
            vector<Node<T>*> all = getAllBelow(node);
            node->children.clear();
            /// Delete all nodes below the source node
            for (auto i = all.begin(); i != all.end(); i++)
            {
                delete *i;
                *i = nullptr;
            }
            if (node->parent != nullptr)
            {
                /// Remove the source node from the tree and then we can safely delete it
                for (auto i = node->parent->children.begin(); i != node->parent->children.end(); i++)
                {
                    if (node->id == (*i)->id)
                    {
                        node->parent->children.erase(i);
                        delete node;
                        node = nullptr;
                        updateFlattened = true;
                        total--;
                        return true;
                    }
                }
            }
            else
            {
                /// Remove the source node from the roots
                for (auto i = roots.begin(); i != roots.end(); i++)
                {
                    if (node->id == (*i)->id)
                    {
                        roots.erase(i);
                        delete node;
                        node = nullptr;
                        updateFlattened = true;
                        total--;
                        return true;
                    }
                }
            }
            /// If we reach this point, there's a problem! Will not delete the node because e.g. the node is in another tree
            SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "(!) Could not locate node[%d] in tree for removal.", node->id);
            return false;
        }

        /// Finds and removes first found data node with the specified name
        bool remove(const string& name)
        {
            /// Short circuit out - no removal of the root node!
            if (name.length() == 0)
            {
                return false;
            }
            Node<T>* node = find(name);
            return remove(node);
        }

        /// Removes all nodes from the tree, except for the root
        void clear()
        {
            roots.clear();
            flatTree.clear();
        }

        /// Recursive search; returns first matching node as a pointer from the source node downwards.
        /// If no instance is found, returns null
        Node<T>* find(const string& name, Node<T>* source)
        {
            if (source == nullptr)
            {
                return nullptr;
            }
            return recursiveFind(name, source);
        }

        /// Uses roots vector as the default source
        Node<T>* find(const string& name)
        {
            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                #ifdef DEBUG
                SDL_assert(*i != nullptr);
                #endif // DEBUG
                Node<T>* node = recursiveFind(name, *i);
                if (node != nullptr)
                {
                    return node;
                }
            }
            return nullptr;
        }

        /// Gets pointers to all nodes in the tree (below the source node) which have the specified name; pretty slow!
        void findAll(const string& name, Node<T>* source, vector<Node<T>*>& output)
        {
            /// While we're iterating over everything we may as well rebuild the flatTree
            flatTree.clear();
            flatTree.reserve(total);
            recursiveFindAll(name, source, output);
            updateFlattened = false;
        }

        /// Defaults to roots as the source
        vector<Node<T>*> findAll(const string& name)
        {
            vector<Node<T>*> nodes;
            /// While we're iterating over everything we may as well rebuild the flatTree
            flatTree.clear();
            flatTree.reserve(total);

            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                recursiveFindAll(*i->name, *i, nodes);
            }
            updateFlattened = false;
            return nodes;
        }

        /// A version of findAll() that matches multiple names; more efficient as it only walks the tree once,
        /// but can match more than a single name
        vector<Node<T>*> findAll(const vector<string>& names)
        {
            vector<Node<T>*> nodes;
            flatTree.clear();
            flatTree.reserve(total);

            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                recursiveFindAll(names, *i, nodes);
            }

            updateFlattened = false;
            return nodes;
        }

        /// Returns all nodes in the tree below a source node; includeSource adds the source node to the output vector
        vector<Node<T>*> getAllBelow(Node<T>* source, bool includeSource = false)
        {
            vector<Node<T>*> all;
            if (source == nullptr)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "(!) Attempted to retrieve all below a null node.");
                return all;
            }
            if (includeSource)
            {
                recursiveGetAll(source, all);
            }
            else
            {
                for (auto i = source->children.begin(); i != source->children.end(); i++)
                {
                    recursiveGetAll(source, all);
                }
            }
            return all;
        }

        /// Gets all nodes in the tree except for the root node
        /// It's preferable to use getFlatTree() instead of this method, as this does not cache
        vector<Node<T>*> getAll()
        {
            vector<Node<T>*> all;
            for (auto i = roots.begin(); i != roots.end(); i++)
            {
                recursiveGetAll(*i, all);
            }
            return all;
        }

        /// Returns a reference to the flatTree version of the tree
        vector<Node<T>*>& getFlatTree()
        {
            if (updateFlattened)
            {
                flatTree.clear();
                /// Preallocate memory for some performance gains. Use total + 1 as we include the root node.
                flatTree.reserve(total);
                for (auto i = roots.begin(); i != roots.end(); i++)
                {
                    recursiveFlatten(*i);
                }
                updateFlattened = false;
            }
            return flatTree;
        }

        /// Not including root node
        unsigned int size()
        {
            return total;
        }

    private:
        /// Total number of nodes, NOT including the root node
        unsigned int total;

        /// Next node id
        int nextId;

        /// Root nodes of the tree
        vector<Node<T>*> roots;

        /// A 'flat' version of the tree with references to all nodes
        vector<Node<T>*> flatTree;

        /// Whether or not the tree should re-calculate the flatTree array next time GetFlattened() is called
        bool updateFlattened;

        /// Recursively iterates through the tree and builds the flatTree tree array
        void recursiveFlatten(Node<T>* parent)
        {
            flatTree.push_back(parent);
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                recursiveFlatten(*i);
            }
        }

        /// Recursive search method
        Node<T>* recursiveFind(const string& name, Node<T>* parent)
        {
            if (parent->name == name)
            {
                return parent;
            }
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                Node<T>* found = recursiveFind(name, *i);
                if (found != nullptr)
                {
                    return found;
                }
            }
            return nullptr;
        }

        void recursiveFindAll(const string& name, Node<T>* parent, vector<Node<T>*>& output)
        {
            /// We can recalculate the flatTree tree while we're here
            if (updateFlattened)
            {
                flatTree.push_back(parent);
            }
            if (parent->name == name)
            {
                output.push_back(parent);
            }
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                recursiveFindAll(name, *i, output);
            }
        }

        void recursiveFindAll(const vector<string>& names, Node<T>* parent, vector<Node<T>*>& output)
        {
            if (updateFlattened)
            {
                flatTree.push_back(parent);
            }
            for (auto i = names.begin(); i != names.end(); i++)
            {
                if (*i == parent->name)
                {
                    output.push_back(parent);
                    break;
                }
            }
            for (auto i = parent->children.begin(); i != parent->children.end(); i++)
            {
                recursiveFindAll(names, *i, output);
            }
        }

        /// Recursively returns pointers to ALL nodes below some source node
        void recursiveGetAll(Node<T>* source, vector<Node<T>*>& output)
        {
            output.push_back(source);
            for (auto i = source->children.begin(); i != source->children.end(); i++)
            {
                recursiveGetAll(*i, output);
            }
        }

    };

}

#endif // TREE_H
