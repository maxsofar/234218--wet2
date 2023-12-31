//
// Created by Max on 05/05/2023.
//

#ifndef WET1_TREE_H
#define WET1_TREE_H

#include "Node.h"
#include <memory>
#include <functional>

template <typename Key, typename Value> class HashTable;

using std::unique_ptr;

template <class Key, class Value>
class Tree {
public:
    /*
     * Constructors
     */
    Tree();
    ~Tree();
    Tree(const Tree& tree) = delete;
    Tree& operator=(const Tree& tree) = delete;
    /*
     * Methods
     */
    bool insert(const Key& key, const Value& value);
    bool remove(const Key& key);
    Node<Key, Value>* getRoot() const;
    void deleteTree(Node<Key, Value>* current);
    Node<Key, Value>* find(const Key& key, Node<Key, Value>* current) const;
    Node<Key, Value>* findMin(Node<Key, Value>* current) const;
    /*
     * RecordCompany adapted methods
     */
    void addPrizeAux(Node<Key, Value> *current, const int &id1, const int &id2, const double &amount);
    void addPrize(const int &id1, const int &id2, const double &amount);
    void updateExtraLeft(Node<Key, Value> *current, const int &id, const double &amount, int prevTurn);
    void updateExtraRight(Node<Key, Value> *current, const int &id, const double &amount, int prevTurn);
    void inOrder(Node<Key, Value>* current, Tree<Key, Value>* newTable,
                 std::function<size_t(const Key&)> hash_function);
    void inOrderNullify(Node<Key, Value>* current);
    bool sumUpExtra(const Key& id, double* sum);
    void resetExpenses(Node<Key, Value>* current);

private:
    Node<Key, Value>* m_root;
    unique_ptr<Node<Key, Value>> m_minNode;
    int m_size;
    /*
     * Private Methods
     */
    Node<Key, Value>* rotateLeft(Node<Key, Value>* current);
    Node<Key, Value>* rotateRight(Node<Key, Value>* current);
    void updateExtraOnLeftRotation(Node<Key, Value>* current);
    void updateExtraOnRightRotation(Node<Key, Value>* current);
    Node<Key, Value>* balance(Node<Key, Value>* current);
    Node<Key, Value>* insert(Node<Key, Value>* nodeToInsert, Node<Key, Value>* current, bool* doesExist);
    Node<Key, Value>* remove(const Key& key, Node<Key, Value>* current, bool* doesExist);
    static int max(int a, int b);
};

template<class Key, class Value>
void Tree<Key, Value>::inOrderNullify(Node<Key, Value> *current)
{
    if (current == nullptr)
        return;

    inOrderNullify(current->getLeft());
    current->newMonthNullify();
    inOrderNullify(current->getRight());
}

template<class Key, class Value>
void Tree<Key, Value>::resetExpenses(Node<Key, Value> *current)
{
    if (current == nullptr)
        return;

    resetExpenses(current->getLeft());
    current->getValue()->resetExpenses();
    current->newMonthNullify();
    resetExpenses(current->getRight());
}

template<class Key, class Value>
bool Tree<Key, Value>::sumUpExtra(const Key &id, double* sum)
{
    Node<Key, Value>* current = this->getRoot();
    while (current != nullptr)
    {
        *sum += current->getExtra();
        if (current->getKey() == id) {
            *sum = current->getValue()->getExpenses() - *sum;
            return true;
        }
        else if (current->getKey() > id)
            current = current->getLeft();
        else
            current = current->getRight();
    }
    return false;
}

template<class Key, class Value>
void Tree<Key, Value>::inOrder(Node<Key, Value> *current, Tree<Key, Value>* newTable,
                               std::function<size_t(const Key&)> hash_function)
{
    if (current == nullptr)
        return;

    inOrder(current->getLeft(), newTable, hash_function);
    size_t index = hash_function(current->getKey());
    newTable[index].insert(current->getKey(), current->getValue());
    inOrder(current->getRight(), newTable, hash_function);
}

template <class Key, class Value>
Tree<Key, Value>::Tree() : m_root(nullptr), m_minNode(nullptr), m_size(0) {}

template<class Key, class Value>
void Tree<Key, Value>::deleteTree(Node<Key, Value>* current)
{
    if (current == nullptr)
        return;

    deleteTree(current->getLeft());
    deleteTree(current->getRight());
    delete current;
}

template <class Key, class Value>
Tree<Key, Value>::~Tree()
{
    deleteTree(this->m_root);
}

template<class Key, class Value>
int Tree<Key, Value>::max(int a, int b)
{
    return (a > b) ? a : b;
}

template<class Key, class Value>
Node<Key, Value> *Tree<Key, Value>::findMin(Node<Key, Value> *current) const
{
    if (current == nullptr)
    {
        return nullptr;
    }
    if (current->getLeft() == nullptr)
    {
        return current;
    }
    return findMin(current->getLeft());
}

template<class Key, class Value>
void Tree<Key, Value>::updateExtraLeft(Node<Key, Value> *current, const int &id, const double &amount, int prevTurn)
{
    if (current == nullptr)
        return;

    if (id < current->getKey()) {
        if (prevTurn == -1) {
            if (current->getLeft() == nullptr)
                return;
            updateExtraLeft(current->getLeft(), id, amount, -1);
        }
        else {
            if (current->getLeft() == nullptr) {
                current->setExtra(amount);
                return;
            }
            current->setExtra(amount);
            updateExtraLeft(current->getLeft(), id, amount, -1);
        }
    } else if (current->getKey() < id) {
        if (prevTurn == 1) {
            if (current->getRight() == nullptr)
                return;
            updateExtraLeft(current->getRight(), id, amount, 1);
        }
        else {
            if (current->getRight() == nullptr) {
                current->setExtra(-amount);
                return;
            }
            current->setExtra(-amount);
            updateExtraLeft(current->getRight(), id, amount, 1);
        }
    } else {
        if (prevTurn == -1) {
            if (current->getLeft() != nullptr)
                current->getLeft()->setExtra(-amount);
        } else {
            current->setExtra(amount);
            if (current->getLeft() != nullptr)
                current->getLeft()->setExtra(-amount);
        }
    }
}

template<class Key, class Value>
void Tree<Key, Value>::updateExtraRight(Node<Key, Value> *current, const int &id, const double &amount, int prevTurn)
{
    if (current == nullptr)
        return;

    if (current->getKey() < id) {
        if (prevTurn == 1) {
            if (current->getRight() == nullptr)
                return;
            updateExtraRight(current->getRight(), id, amount, 1);
        }
        else {
            if (current->getRight() == nullptr) {
                current->setExtra(amount);
                return;
            }
            current->setExtra(amount);
            updateExtraRight(current->getRight(), id, amount, 1);
        }
    } else if (id < current->getKey()) {
        if (prevTurn == -1) {
            if (current->getLeft() == nullptr)
                return;
            updateExtraRight(current->getLeft(), id, amount, -1);
        }
        else {
            if (current->getLeft() == nullptr) {
                current->setExtra(-amount);
                return;
            }
            current->setExtra(-amount);
            updateExtraRight(current->getLeft(), id, amount, -1);
        }
    } else {
        if(prevTurn == 1) {
            current->setExtra(-amount);
            if(current->getLeft() != nullptr)
                current->getLeft()->setExtra(amount);
        } else {
            if (current->getLeft() != nullptr)
                current->getLeft()->setExtra(amount);
        }
    }
}

template<class Key, class Value>
void Tree<Key, Value>::addPrizeAux(Node<Key, Value> *current, const int &id1, const int &id2, const double &amount)
{
    if (current == nullptr)
        return;

    if (id1 < current->getKey() && id2 < current->getKey())
    {
        addPrizeAux(current->getLeft(), id1, id2, amount);
    }
    else if (id1 > current->getKey() && id2 > current->getKey())
    {
        addPrizeAux(current->getRight(), id1, id2, amount);
    }
    else
    {
        if (current->getKey() == id1) {
            current->setExtra(amount);
            if (current->getLeft() != nullptr)
                current->getLeft()->setExtra(-amount);
            updateExtraRight(current->getRight(), id2, amount, 1);
        } else if (current->getKey() == id2) {
            if (current->getLeft() != nullptr) {
                current->getLeft()->setExtra(amount);
                updateExtraLeft(current->getLeft(), id1, amount, -1);
            }
        } else {
            current->setExtra(amount);
            updateExtraLeft(current->getLeft(), id1, amount, -1);
            updateExtraRight(current->getRight(), id2, amount, 1);
        }
    }
}

template<class Key, class Value>
void Tree<Key, Value>::addPrize(const int &id1, const int &id2, const double &amount)
{
    addPrizeAux(this->m_root, id1, id2, amount);
}


template<class Key, class Value>
Node<Key, Value>* Tree<Key, Value>::find(const Key &key, Node<Key, Value>* current) const
{
    if (current == nullptr) {
        return nullptr;
    }
    if (key == current->getKey()) {
        return current;
    }
    if (key < current->getKey()) {
        return find(key, current->getLeft());
    }
    return find(key, current->getRight());
}

template<class Key, class Value>
Node<Key, Value> *Tree<Key, Value>::getRoot() const
{
    return this->m_root;
}

template<class Key, class Value>
void Tree<Key, Value>::updateExtraOnLeftRotation(Node<Key, Value> *current)
{
    double temp = current->getRight()->getExtra();
    current->getRight()->setExtra(current->getExtra());
    current->setExtra(-(current->getRight()->getExtra()));
    if (current->getRight()->getLeft() != nullptr)
        current->getRight()->getLeft()->setExtra(temp);
}

template<class Key, class Value>
Node<Key, Value>* Tree<Key, Value>::rotateLeft(Node<Key, Value>* current)
{
    Node<Key, Value>* rightSubTree = current->getRight();
    Node<Key, Value>* rightLeftSubTree = rightSubTree->getLeft();

    updateExtraOnLeftRotation(current);

    rightSubTree->setLeft(current);
    current->setRight(rightLeftSubTree);

    int leftHeight = current->getLeft() == nullptr ? -1 : current->getLeft()->getHeight();
    int rightHeight = current->getRight() == nullptr ? -1 : current->getRight()->getHeight();
    current->setHeight(max(leftHeight, rightHeight) + 1);

    leftHeight = rightSubTree->getLeft() == nullptr ? -1 : rightSubTree->getLeft()->getHeight();
    rightHeight = rightSubTree->getRight() == nullptr ? -1 : rightSubTree->getRight()->getHeight();
    rightSubTree->setHeight(max(leftHeight, rightHeight) + 1);

    return rightSubTree;
}

template<class Key, class Value>
void Tree<Key, Value>::updateExtraOnRightRotation(Node<Key, Value> *current)
{
    double temp = current->getLeft()->getExtra();
    current->getLeft()->setExtra(current->getExtra());
    current->setExtra(-(current->getLeft()->getExtra()));
    if (current->getLeft()->getRight() != nullptr)
        current->getLeft()->getRight()->setExtra(temp);
}

template<class Key, class Value>
Node<Key, Value> *Tree<Key, Value>::rotateRight(Node<Key, Value> *current)
{
    Node<Key, Value>* leftSubTree = current->getLeft();
    Node<Key, Value>* leftRightSubTree = leftSubTree->getRight();

    updateExtraOnRightRotation(current);

    leftSubTree->setRight(current);
    current->setLeft(leftRightSubTree);

    int leftHeight = current->getLeft() == nullptr ? -1 : current->getLeft()->getHeight();
    int rightHeight = current->getRight() == nullptr ? -1 : current->getRight()->getHeight();
    current->setHeight(max(leftHeight, rightHeight) + 1);

    leftHeight = leftSubTree->getLeft() == nullptr ? -1 : leftSubTree->getLeft()->getHeight();
    rightHeight = leftSubTree->getRight() == nullptr ? -1 : leftSubTree->getRight()->getHeight();
    leftSubTree->setHeight(max(leftHeight, rightHeight) + 1);

    return leftSubTree;
}

template<class Key, class Value>
Node<Key, Value> *Tree<Key, Value>::balance(Node<Key, Value> *current)
{
    if (current == nullptr) {
        return current;
    }
    int leftHeight = current->getLeft() == nullptr ? -1 : current->getLeft()->getHeight();
    int rightHeight = current->getRight() == nullptr ? -1 : current->getRight()->getHeight();
    current->setHeight(max(leftHeight, rightHeight) + 1);
    int balanceFactor = current->getBalanceFactor();

    // Left heavy
    if (balanceFactor > 1) {
        // Left-Right case
        if (current->getLeft()->getBalanceFactor() < 0) {
            current->setLeft(rotateLeft(current->getLeft()));
        }
        Node<Key, Value>* newRoot = rotateRight(current);
        return newRoot;
    }
    // Right heavy
    else if (balanceFactor < -1) {
        // Right-Left case
        if (current->getRight()->getBalanceFactor() > 0) {
            current->setRight(rotateRight(current->getRight()));
        }

        return rotateLeft(current);
    }

    return current;
}

template<class Key, class Value>
Node<Key, Value>* Tree<Key, Value>::insert(Node<Key, Value>* nodeToInsert ,Node<Key, Value>* current, bool* doesExist)
{
    if (current == nullptr) {
        this->m_size++;
        // Set the extra field of the new node to be the negative of the sum of the extra values on the path to its parent
        Node<Key, Value>* temp = this->getRoot();
        int sum = 0;
        while (temp != nullptr) {
            sum += temp->getExtra();
            if (nodeToInsert->getKey() < temp->getKey()) {
                temp = temp->getLeft();
            } else if (nodeToInsert->getKey() > temp->getKey()) {
                temp = temp->getRight();
            } else {
                break;
            }
        }
        nodeToInsert->setExtra(-sum);
        return nodeToInsert;
    }

    if (nodeToInsert->getKey() < current->getKey()) {
        current->setLeft(insert(nodeToInsert, current->getLeft(), doesExist));
    } else if (nodeToInsert->getKey() > current->getKey()) {
        current->setRight(insert(nodeToInsert, current->getRight(), doesExist));
    } else {
        *doesExist = true;
        delete nodeToInsert;
        return current;
    }

    current = balance(current);

    return current;
}

template <class Key, class Value>
bool Tree<Key, Value>::insert(const Key& key, const Value& value)
{
    bool doesExist = false;
    if (this->m_root == nullptr) {
        this->m_root = new Node<Key, Value>(key, value);
        this->m_minNode = unique_ptr<Node<Key, Value>>(new Node<Key, Value>(key, value));
        this->m_size++;
    }
    else if (key == this->m_minNode->getKey()) {
        return true;
    }
    else {
        auto* node = new Node<Key, Value>(key, value);
        if (key < this->m_minNode->getKey()) {
            this->m_minNode = unique_ptr<Node<Key, Value>>(new Node<Key, Value>(key, value));
        }
        this->m_root = insert(node, this->m_root, &doesExist);
    }
    return doesExist;
}

template <class Key, class Value>
Node<Key, Value>* Tree<Key, Value>::remove(const Key& key, Node<Key, Value>* current, bool* doesExist)
{
    if (current == nullptr) {
        *doesExist = false;
        return current;
    }

    if (key < current->getKey()) {
        current->setLeft(remove(key, current->getLeft(), doesExist));
    } else if (key > current->getKey()) {
        current->setRight(remove(key, current->getRight(), doesExist));
    }
    else
    {
        // Case 1: One or No child
        if (current->getLeft() == nullptr || current->getRight() == nullptr)
        {
            Node<Key, Value>* temp = current->getLeft() ? current->getLeft() : current->getRight();
            // No child
            if (temp == nullptr) {
                temp = current;
                current = nullptr;
            }
            // One child
            else {
                *current = *temp;
            }
            delete temp;
            this->m_size--;
        }
        // Case 2: Two children
        else
        {
            Node<Key, Value>* temp = current->getRight();
            // Find node's successor to swap with
            while (temp->getLeft() != nullptr) {
                temp = temp->getLeft();
            }
            current->setKey(temp->getKey());
            current->setValue(temp->getValue());
            current->setRight(remove(temp->getKey(), current->getRight(), doesExist));
        }
    }

    current = balance(current);
    return current;
}

template <class Key, class Value>
bool Tree<Key, Value>::remove(const Key& key)
{
    bool doesExist = true;
    this->m_root = remove(key, this->m_root, &doesExist);
    if (key == this->m_minNode->getKey()) {
        Node<Key, Value>* temp = findMin(this->m_root);
        if (temp != nullptr) {
            this->m_minNode = unique_ptr<Node<Key, Value>>(new Node<Key, Value>(temp->getKey(), temp->getValue()));
        }
    }
    return doesExist;
}

#endif //WET1_TREE_H

