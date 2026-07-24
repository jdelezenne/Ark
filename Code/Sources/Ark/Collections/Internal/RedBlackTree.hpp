#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Collections::Internal
{
    template <typename Key, typename Value, typename KeyAccessor, typename Compare = Traits::Less<Key>>
    struct RedBlackTree final
    {
        struct Node
        {
            Value value;
            Node* parent{nullptr};
            Node* left{nullptr};
            Node* right{nullptr};
            bool isRed{true};

            Node(Value const& valueToStore)
                : value(valueToStore)
            {
            }

            Node(Value&& valueToStore)
                : value(Ark::move(valueToStore))
            {
            }

            template <typename... Args>
            explicit Node(Args&&... arguments)
                : value(Ark::forward<Args>(arguments)...)
            {
            }
        };

    private:
        Memory::Allocator& allocator{Memory::getDefaultAllocator()};
        Node* rootNode{nullptr};
        usize entryCount{0};
        Compare comparisonFunction{};
        KeyAccessor keyAccessor{};

    public:
        RedBlackTree(Memory::Allocator& allocatorValue = Memory::getDefaultAllocator())
            : allocator(allocatorValue)
        {
        }

        RedBlackTree(Memory::Allocator& allocatorValue, Compare const& compare)
            : allocator(allocatorValue)
            , comparisonFunction(compare)
        {
        }

        bool isEmpty() const
        {
            return entryCount == 0;
        }

        usize getCount() const
        {
            return entryCount;
        }

        Memory::Allocator* getAllocator() const
        {
            return &allocator;
        }

        Compare getCompare() const
        {
            return comparisonFunction;
        }

        void swapStorage(RedBlackTree& other)
        {
            ARK_ASSERT(&allocator == &other.allocator);
            Ark::swap(rootNode, other.rootNode);
            Ark::swap(entryCount, other.entryCount);
            Ark::swap(comparisonFunction, other.comparisonFunction);
            Ark::swap(keyAccessor, other.keyAccessor);
        }

        Node* getRootNode() const
        {
            return rootNode;
        }

        void removeAll()
        {
            removeSubtree(rootNode);
            rootNode = nullptr;
            entryCount = 0;
        }

        Node* getMinimumNode(Node* node) const
        {
            Node* currentNode = node;
            while ((currentNode != nullptr) && (currentNode->left != nullptr))
            {
                currentNode = currentNode->left;
            }
            return currentNode;
        }

        Node* getMaximumNode(Node* node) const
        {
            Node* currentNode = node;
            while ((currentNode != nullptr) && (currentNode->right != nullptr))
            {
                currentNode = currentNode->right;
            }
            return currentNode;
        }

        Node* getFirstNode() const
        {
            return getMinimumNode(rootNode);
        }

        Node* getLastNode() const
        {
            return getMaximumNode(rootNode);
        }

        Node* getNextNode(Node* node) const
        {
            if (node == nullptr)
            {
                return getFirstNode();
            }

            if (node->right != nullptr)
            {
                return getMinimumNode(node->right);
            }

            Node* currentNode = node;
            Node* parentNode = node->parent;
            while ((parentNode != nullptr) && (currentNode == parentNode->right))
            {
                currentNode = parentNode;
                parentNode = parentNode->parent;
            }
            return parentNode;
        }

        Node* getPreviousNode(Node* node) const
        {
            if (node == nullptr)
            {
                return getLastNode();
            }

            if (node->left != nullptr)
            {
                return getMaximumNode(node->left);
            }

            Node* currentNode = node;
            Node* parentNode = node->parent;
            while ((parentNode != nullptr) && (currentNode == parentNode->left))
            {
                currentNode = parentNode;
                parentNode = parentNode->parent;
            }
            return parentNode;
        }

        Node* findNode(Key const& key) const
        {
            Node* currentNode = rootNode;
            while (currentNode != nullptr)
            {
                Key const& currentKey = keyAccessor(currentNode->value);
                if (comparisonFunction(key, currentKey))
                {
                    currentNode = currentNode->left;
                }
                else if (comparisonFunction(currentKey, key))
                {
                    currentNode = currentNode->right;
                }
                else
                {
                    return currentNode;
                }
            }
            return nullptr;
        }

        Node* findLowerBoundNode(Key const& key) const
        {
            Node* currentNode = rootNode;
            Node* candidateNode = nullptr;
            while (currentNode != nullptr)
            {
                Key const& currentKey = keyAccessor(currentNode->value);
                if (!comparisonFunction(currentKey, key))
                {
                    candidateNode = currentNode;
                    currentNode = currentNode->left;
                }
                else
                {
                    currentNode = currentNode->right;
                }
            }
            return candidateNode;
        }

        Node* findUpperBoundNode(Key const& key) const
        {
            Node* currentNode = rootNode;
            Node* candidateNode = nullptr;
            while (currentNode != nullptr)
            {
                Key const& currentKey = keyAccessor(currentNode->value);
                if (comparisonFunction(key, currentKey))
                {
                    candidateNode = currentNode;
                    currentNode = currentNode->left;
                }
                else
                {
                    currentNode = currentNode->right;
                }
            }
            return candidateNode;
        }

        Pair<Node*, bool> insertUnique(Value const& value)
        {
            return insertUniqueInternal(value);
        }

        Pair<Node*, bool> insertUnique(Value&& value)
        {
            return insertUniqueInternal(Ark::move(value));
        }

        template <typename... Args>
        Pair<Node*, bool> emplaceUnique(Args&&... arguments)
        {
            Value value(Ark::forward<Args>(arguments)...);
            return insertUnique(Ark::move(value));
        }

        void eraseNode(Node* nodeToErase)
        {
            ARK_ASSERT(nodeToErase != nullptr);

            Node* removedNode = nodeToErase;
            bool removedNodeWasRed = removedNode->isRed;
            Node* movedNode = nullptr;
            Node* movedNodeParent = nullptr;

            if (nodeToErase->left == nullptr)
            {
                movedNode = nodeToErase->right;
                movedNodeParent = nodeToErase->parent;
                transplant(nodeToErase, nodeToErase->right);
            }
            else if (nodeToErase->right == nullptr)
            {
                movedNode = nodeToErase->left;
                movedNodeParent = nodeToErase->parent;
                transplant(nodeToErase, nodeToErase->left);
            }
            else
            {
                removedNode = getMinimumNode(nodeToErase->right);
                ARK_ASSERT(removedNode != nullptr);

                removedNodeWasRed = removedNode->isRed;
                movedNode = removedNode->right;
                if (removedNode->parent == nodeToErase)
                {
                    movedNodeParent = removedNode;
                    if (movedNode != nullptr)
                    {
                        movedNode->parent = removedNode;
                    }
                }
                else
                {
                    movedNodeParent = removedNode->parent;
                    transplant(removedNode, removedNode->right);
                    removedNode->right = nodeToErase->right;
                    removedNode->right->parent = removedNode;
                }

                transplant(nodeToErase, removedNode);
                removedNode->left = nodeToErase->left;
                removedNode->left->parent = removedNode;
                removedNode->isRed = nodeToErase->isRed;
            }

            destroyNode(nodeToErase);
            --entryCount;

            if (!removedNodeWasRed)
            {
                fixErase(movedNode, movedNodeParent);
            }
        }

    private:
        static bool isNodeRed(Node* node)
        {
            return (node != nullptr) && node->isRed;
        }

        void removeSubtree(Node* node)
        {
            if (node == nullptr)
            {
                return;
            }

            removeSubtree(node->left);
            removeSubtree(node->right);
            destroyNode(node);
        }

        template <typename V>
        Pair<Node*, bool> insertUniqueInternal(V&& value)
        {
            Key const& key = keyAccessor(value);

            Node* parentNode = nullptr;
            Node* currentNode = rootNode;
            while (currentNode != nullptr)
            {
                parentNode = currentNode;
                Key const& currentKey = keyAccessor(currentNode->value);
                if (comparisonFunction(key, currentKey))
                {
                    currentNode = currentNode->left;
                }
                else if (comparisonFunction(currentKey, key))
                {
                    currentNode = currentNode->right;
                }
                else
                {
                    return {currentNode, false};
                }
            }

            Node* insertedNode = createNode(Ark::forward<V>(value));
            insertedNode->parent = parentNode;
            insertedNode->left = nullptr;
            insertedNode->right = nullptr;
            insertedNode->isRed = true;

            if (parentNode == nullptr)
            {
                rootNode = insertedNode;
            }
            else if (comparisonFunction(key, keyAccessor(parentNode->value)))
            {
                parentNode->left = insertedNode;
            }
            else
            {
                parentNode->right = insertedNode;
            }

            ++entryCount;
            fixInsert(insertedNode);
            return {insertedNode, true};
        }

        template <typename V>
        Node* createNode(V&& value)
        {
            return allocator.make<Node>(Ark::forward<V>(value));
        }

        void destroyNode(Node* node)
        {
            allocator.destroy(node);
        }

        void rotateLeft(Node* pivotNode)
        {
            Node* rightNode = pivotNode->right;
            ARK_ASSERT(rightNode != nullptr);

            pivotNode->right = rightNode->left;
            if (rightNode->left != nullptr)
            {
                rightNode->left->parent = pivotNode;
            }

            rightNode->parent = pivotNode->parent;
            if (pivotNode->parent == nullptr)
            {
                rootNode = rightNode;
            }
            else if (pivotNode == pivotNode->parent->left)
            {
                pivotNode->parent->left = rightNode;
            }
            else
            {
                pivotNode->parent->right = rightNode;
            }

            rightNode->left = pivotNode;
            pivotNode->parent = rightNode;
        }

        void rotateRight(Node* pivotNode)
        {
            Node* leftNode = pivotNode->left;
            ARK_ASSERT(leftNode != nullptr);

            pivotNode->left = leftNode->right;
            if (leftNode->right != nullptr)
            {
                leftNode->right->parent = pivotNode;
            }

            leftNode->parent = pivotNode->parent;
            if (pivotNode->parent == nullptr)
            {
                rootNode = leftNode;
            }
            else if (pivotNode == pivotNode->parent->right)
            {
                pivotNode->parent->right = leftNode;
            }
            else
            {
                pivotNode->parent->left = leftNode;
            }

            leftNode->right = pivotNode;
            pivotNode->parent = leftNode;
        }

        void fixInsert(Node* node)
        {
            Node* currentNode = node;
            while ((currentNode != rootNode) && isNodeRed(currentNode->parent))
            {
                Node* parentNode = currentNode->parent;
                Node* grandparentNode = parentNode->parent;
                ARK_ASSERT(grandparentNode != nullptr);

                if (parentNode == grandparentNode->left)
                {
                    Node* uncleNode = grandparentNode->right;
                    if (isNodeRed(uncleNode))
                    {
                        parentNode->isRed = false;
                        uncleNode->isRed = false;
                        grandparentNode->isRed = true;
                        currentNode = grandparentNode;
                    }
                    else
                    {
                        if (currentNode == parentNode->right)
                        {
                            currentNode = parentNode;
                            rotateLeft(currentNode);
                            parentNode = currentNode->parent;
                            grandparentNode = parentNode->parent;
                        }

                        parentNode->isRed = false;
                        grandparentNode->isRed = true;
                        rotateRight(grandparentNode);
                    }
                }
                else
                {
                    Node* uncleNode = grandparentNode->left;
                    if (isNodeRed(uncleNode))
                    {
                        parentNode->isRed = false;
                        uncleNode->isRed = false;
                        grandparentNode->isRed = true;
                        currentNode = grandparentNode;
                    }
                    else
                    {
                        if (currentNode == parentNode->left)
                        {
                            currentNode = parentNode;
                            rotateRight(currentNode);
                            parentNode = currentNode->parent;
                            grandparentNode = parentNode->parent;
                        }

                        parentNode->isRed = false;
                        grandparentNode->isRed = true;
                        rotateLeft(grandparentNode);
                    }
                }
            }

            if (rootNode != nullptr)
            {
                rootNode->isRed = false;
            }
        }

        void transplant(Node* nodeToReplace, Node* replacementNode)
        {
            if (nodeToReplace->parent == nullptr)
            {
                rootNode = replacementNode;
            }
            else if (nodeToReplace == nodeToReplace->parent->left)
            {
                nodeToReplace->parent->left = replacementNode;
            }
            else
            {
                nodeToReplace->parent->right = replacementNode;
            }

            if (replacementNode != nullptr)
            {
                replacementNode->parent = nodeToReplace->parent;
            }
        }

        void fixErase(Node* node, Node* nodeParent)
        {
            Node* currentNode = node;
            Node* currentParent = nodeParent;

            while ((currentNode != rootNode) && !isNodeRed(currentNode))
            {
                if (currentParent == nullptr)
                {
                    break;
                }

                if (currentNode == currentParent->left)
                {
                    Node* siblingNode = currentParent->right;
                    if (isNodeRed(siblingNode))
                    {
                        siblingNode->isRed = false;
                        currentParent->isRed = true;
                        rotateLeft(currentParent);
                        siblingNode = currentParent->right;
                    }

                    bool siblingLeftIsRed = isNodeRed((siblingNode != nullptr) ? siblingNode->left : nullptr);
                    bool siblingRightIsRed = isNodeRed((siblingNode != nullptr) ? siblingNode->right : nullptr);
                    if (!siblingLeftIsRed && !siblingRightIsRed)
                    {
                        if (siblingNode != nullptr)
                        {
                            siblingNode->isRed = true;
                        }
                        currentNode = currentParent;
                        currentParent = currentParent->parent;
                    }
                    else
                    {
                        if (!siblingRightIsRed)
                        {
                            if ((siblingNode != nullptr) && (siblingNode->left != nullptr))
                            {
                                siblingNode->left->isRed = false;
                            }
                            if (siblingNode != nullptr)
                            {
                                siblingNode->isRed = true;
                                rotateRight(siblingNode);
                            }
                            siblingNode = currentParent->right;
                        }

                        if (siblingNode != nullptr)
                        {
                            siblingNode->isRed = currentParent->isRed;
                        }
                        currentParent->isRed = false;
                        if ((siblingNode != nullptr) && (siblingNode->right != nullptr))
                        {
                            siblingNode->right->isRed = false;
                        }
                        rotateLeft(currentParent);
                        currentNode = rootNode;
                    }
                }
                else
                {
                    Node* siblingNode = currentParent->left;
                    if (isNodeRed(siblingNode))
                    {
                        siblingNode->isRed = false;
                        currentParent->isRed = true;
                        rotateRight(currentParent);
                        siblingNode = currentParent->left;
                    }

                    bool siblingLeftIsRed = isNodeRed((siblingNode != nullptr) ? siblingNode->left : nullptr);
                    bool siblingRightIsRed = isNodeRed((siblingNode != nullptr) ? siblingNode->right : nullptr);
                    if (!siblingLeftIsRed && !siblingRightIsRed)
                    {
                        if (siblingNode != nullptr)
                        {
                            siblingNode->isRed = true;
                        }
                        currentNode = currentParent;
                        currentParent = currentParent->parent;
                    }
                    else
                    {
                        if (!siblingLeftIsRed)
                        {
                            if ((siblingNode != nullptr) && (siblingNode->right != nullptr))
                            {
                                siblingNode->right->isRed = false;
                            }
                            if (siblingNode != nullptr)
                            {
                                siblingNode->isRed = true;
                                rotateLeft(siblingNode);
                            }
                            siblingNode = currentParent->left;
                        }

                        if (siblingNode != nullptr)
                        {
                            siblingNode->isRed = currentParent->isRed;
                        }
                        currentParent->isRed = false;
                        if ((siblingNode != nullptr) && (siblingNode->left != nullptr))
                        {
                            siblingNode->left->isRed = false;
                        }
                        rotateRight(currentParent);
                        currentNode = rootNode;
                    }
                }
            }

            if (currentNode != nullptr)
            {
                currentNode->isRed = false;
            }
        }
    };
}
