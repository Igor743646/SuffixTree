#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

class suffix_tree {

    using nodeId_type = std::size_t;
    using iter_type = std::size_t;

    struct Node {

        nodeId_type id;
        iter_type l, r;
        std::unordered_map<char, Node*> table;
        Node* suffix_pointer;

        Node();
        Node(iter_type, iter_type);
        Node(iter_type, iter_type, Node*);
        Node(nodeId_type, iter_type, iter_type, Node*);

        iter_type size();
        bool isLeaf();
        void printSelf(const std::string&, int);
        void print(const std::string&, int);

        ~Node();
    };

    class Iter {

        std::string* pattern;
        Node* current;
        Node* edge;
        Node* prev;
        iter_type length;

    public:

        Iter();
        Iter(std::string*, Node*, Node*, Node*, iter_type);

        void setPrev(Node*);
        Node* getPrev();
        Node* getCurrent();
        bool isPrev();
        void resetPrev();

        bool hasMove(char);
        void moveAnyWhere();
        void move(char);
        void createEdge(iter_type, nodeId_type);
        void suf();
    };

    std::string pattern;
    iter_type pattern_size;
    Iter iter;
    Node pre_root;
    Node root;

public:

    suffix_tree(const std::string&, char = '\0');

    void build();
    bool contains(const std::string&);
    iter_type find(const std::string&);
    void print();

    ~suffix_tree();
};