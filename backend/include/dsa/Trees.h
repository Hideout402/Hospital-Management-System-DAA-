#ifndef TREES_H
#define TREES_H

#include <iostream>
#include <vector>
using namespace std;

// Binary Search Tree
struct TreeNode
{
    int data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int value)
    {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};

class BinarySearchTree
{
private:
    TreeNode* root;

    TreeNode* insertNode(TreeNode* node, int value)
    {
        if (node == nullptr)
            return new TreeNode(value);

        if (value < node->data)
            node->left = insertNode(node->left, value);
        else
            node->right = insertNode(node->right, value);

        return node;
    }

    void displayInOrder(TreeNode* node)
    {
        if (node == nullptr)
            return;

        displayInOrder(node->left);
        cout << node->data << " ";
        displayInOrder(node->right);
    }

public:
    BinarySearchTree()
    {
        root = nullptr;
    }

    void insert(int value)
    {
        root = insertNode(root, value);
    }

    void display()
    {
        displayInOrder(root);
        cout << endl;
    }
};

// General Tree
struct GeneralTreeNode
{
    string name;
    vector<GeneralTreeNode*> children;

    GeneralTreeNode(string value)
    {
        name = value;
    }

    void addChild(GeneralTreeNode* child)
    {
        children.push_back(child);
    }
};

#endif
