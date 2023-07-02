#include "suffix_tree.h"

suffix_tree::Node::Node() 
    : id(0), l(0), r(0), suffix_pointer(nullptr) {}
suffix_tree::Node::Node(iter_type _l, iter_type _r)
    : id(0), l(_l), r(_r), suffix_pointer(nullptr) {}
suffix_tree::Node::Node(iter_type _l, iter_type _r, Node* p)
    : id(0), l(_l), r(_r), suffix_pointer(p) {}
suffix_tree::Node::Node(nodeId_type _id, iter_type _l, iter_type _r, Node* p)
    : id(_id), l(_l), r(_r), suffix_pointer(p) {}

suffix_tree::iter_type suffix_tree::Node::size() {
    return r + 1 - l;
}

bool suffix_tree::Node::isLeaf() {
    return table.empty();
}

void suffix_tree::Node::printSelf(const std::string& pattern, int c) {
    for (iter_type i = 0; i < c; i++)
        std::cout << "\t";

    if (!l and !r) {
        std::cout << ".\n";
        return;
    }

    for (iter_type i = l; i <= r; i++)
        std::cout << pattern[i - 1];

    if (isLeaf())
        std::cout << " (" << id << ") ";

    std::cout << std::endl;
}

void suffix_tree::Node::print(const std::string& pattern, int c) {
    std::size_t count_of_children = table.size();
    std::size_t j = 0;

    for (auto& [k, v] : table) {
        v->print(pattern, c + 1);
        j++;
        if (count_of_children / 2 == j or count_of_children == 1) {
            printSelf(pattern, c);
        }
    }

    if (count_of_children == 0) {
        printSelf(pattern, c);
    }
}

suffix_tree::Node::~Node() {
    suffix_pointer = nullptr;

    for (auto& [k, v] : table) {
        if (v) {
            delete v;
            v = nullptr;
        }
    }
}

suffix_tree::Iter::Iter() : pattern(nullptr), current(nullptr), edge(nullptr), prev(nullptr), length(0) {}
suffix_tree::Iter::Iter(std::string* _pat, Node* _cur, Node* _edge, Node* _prev, iter_type _size)
    : pattern(_pat), current(_cur), edge(_edge), prev(_prev), length(_size) {}

void suffix_tree::Iter::setPrev(Node* _prev) { prev = _prev; }
suffix_tree::Node* suffix_tree::Iter::getPrev() { return prev; }
suffix_tree::Node* suffix_tree::Iter::getCurrent() { return current; }
bool suffix_tree::Iter::isPrev() { return prev != nullptr; }
void suffix_tree::Iter::resetPrev() { prev = nullptr; }

void suffix_tree::Iter::moveAnyWhere() {
    if (current == edge and current->isLeaf())
        return;

    if (current == edge) {
        auto iterator = current->table.begin();

        if (iterator->second->l == iterator->second->r) {
            current = edge = iterator->second;
            length = 0;
        }
        else {
            edge = iterator->second;
            length = 1;
        }
    }
    else {
        length++;

        if (length == edge->size()) {
            current = edge;
            length = 0;
        }
    }
}

bool suffix_tree::Iter::hasMove(char x) {
    return (current == edge and current->table.contains(x)) or
        (current != edge and (*pattern)[edge->l + length - 1] == x);
}

void suffix_tree::Iter::move(char x) {
    if (current == edge) {
        auto iterator = current->table.find(x);

        if (iterator->second->l == iterator->second->r) {
            current = edge = iterator->second;
            length = 0;
        }
        else {
            edge = iterator->second;
            length = 1;
        }
    }
    else {
        length++;

        if (length == edge->size()) {
            current = edge;
            length = 0;
        }
    }
}

void suffix_tree::Iter::createEdge(iter_type i, nodeId_type counter) {
    if (current == edge) {
        Node* new_leaf = new Node(counter++, i + 1, pattern->size(), nullptr);
        current->table[(*pattern)[i]] = new_leaf;

        if (isPrev()) {
            getPrev()->suffix_pointer = current;
            resetPrev();
        }
    }
    else {
        Node* new_leaf = new Node(counter++, i + 1, pattern->size(), nullptr);
        Node* new_inside = new Node(edge->l, edge->l + length - 1, nullptr);
        edge->l = edge->l + length;

        new_inside->table[(*pattern)[edge->l - 1]] = edge;
        new_inside->table[(*pattern)[i]] = new_leaf;
        current->table[(*pattern)[new_inside->l - 1]] = new_inside;

        if (isPrev()) {
            prev->suffix_pointer = new_inside;
        }

        prev = edge = new_inside;
    }
}

void suffix_tree::Iter::suf() {
    if (current == edge) {
        edge = current = current->suffix_pointer;
        return;
    }

    iter_type l = edge->l;
    iter_type n = edge->r + 1 - l;

    edge = current = current->suffix_pointer;

    while (n) {
        char x = (*pattern)[l - 1];
        Node* temp = current->table[x];
        if (temp->size() > n) {
            edge = temp;
            length = n;
            n -= n;
        }
        else {
            current = edge = temp;
            n -= temp->size();
            l += temp->size();
        }
    }
}

suffix_tree::suffix_tree(const std::string& s, char end) {
    pattern = s + end;
    pattern_size = 0;
    pre_root = Node(0, 0, nullptr);
    root = Node(0, 0, &pre_root);
}

void suffix_tree::build() {

    iter = Iter(&pattern, &root, &root, nullptr, 0);

    for (iter_type i = 0; i < pattern.size(); i++) {
        char x = pattern[i];

        pre_root.table[x] = &root;
        iter.resetPrev();

        while (!iter.hasMove(x)) {
            iter.createEdge(i, pattern_size++);
            iter.suf();
        }

        if (iter.isPrev()) {
            iter.getPrev()->suffix_pointer = iter.getCurrent();
        }

        iter.move(x);
    }
}

bool suffix_tree::contains(const std::string& s) {

    iter = Iter(&pattern, &root, &root, nullptr, 0);

    for (iter_type i = 0; i < s.size(); i++) {
        if (!iter.hasMove(s[i])) {
            return false;
        }

        iter.move(s[i]);
    }

    return true;
}

suffix_tree::iter_type suffix_tree::find(const std::string& s) {

    iter = Iter(&pattern, &root, &root, nullptr, 0);

    for (iter_type i = 0; i < s.size(); i++) {
        if (!iter.hasMove(s[i])) {
            return pattern_size - 1;
        }

        iter.move(s[i]);
    }

    while (!iter.getCurrent()->isLeaf()) {
        iter.moveAnyWhere();
    }

    return iter.getCurrent()->id;
}

suffix_tree::~suffix_tree() {
    pre_root.table.clear();
}

void suffix_tree::print() {
    root.print(pattern, 0);
}