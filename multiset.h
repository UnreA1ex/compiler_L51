#pragma once
#include <iostream>
using namespace std;

class Multiset {
private:

    struct Node {
        int number;
        int count;
        Node* next;
        Node(int num, int c) : number(num), count(c), next(nullptr) {}
        friend ostream& operator<<(ostream& stream, const Node& node) {
            stream << '(' << node.number << ", " << node.count << ')';
            return stream;
        }
        friend ostream& operator<<(ostream& stream, const Node* node) {
            if (node)
                stream << '(' << node->number << ", " << node->count << ')';
            else
                stream << "nullptr";
            return stream;
        }
    };

    Node* head;

public:

    void clear() {
        while (head) {
            Node* cur = head;
            head = head->next;
            delete cur;
        }
        head = nullptr;
    }

    void insert(int number, int count) {
        Node* node = new Node(number, count);

        if (!head || head->number >= node->number) {
            if (head && head->number == node->number)
                head->count += node->count;
            else {
                node->next = head;
                head = node;
            }
            return;
        }

        Node* cur = head;

        while (cur->next && cur->next->number < node->number)
            cur = cur->next;

        if (cur->next)
            if (cur->next->number == node->number)
                cur->next->count += node->count;
            else {
                node->next = cur->next;
                cur->next = node;
            }
        else {
            if (cur->number == node->number)
                cur->count += node->count;
            else {
                node->next = cur->next;
                cur->next = node;
            }
        }

    }

    class Iterator {
    private:
        Node* current;

    public:
        Iterator(Node* begin) : current(begin) {}
        Node& operator*() {
            if (!current)
                throw "Error: Iterator error";
            return *current;
        }
        Node* operator->() {
            if (!current)
                throw "Error: Iterator error";
            return current;
        }
        Iterator& operator++() {
            if (!current)
                throw "Error: Iterator error";
            current = current->next;
            return *this;
        }
        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() const {
        return Iterator(head);
    }
    Iterator end() const {
        return Iterator(nullptr);
    }

    Multiset() : head(nullptr) {}
    Multiset(int number, int count) {
        insert(number, count);
    }

    Multiset(Multiset&& other) noexcept {
        head = other.head;
        other.head = nullptr;
    }
    Multiset(const Multiset& other) {
        Node* current = other.head;

        if (current) {
            head = new Node(current->number, current->count);
            Node* current_this = head;
            current_this->next = nullptr;
            current = current->next;

            while (current) {
                current_this->next = new Node(current->number, current->count);
                current_this = current_this->next;
                current_this->next = nullptr;
                current = current->next;
            }
        }
        else {
            head = nullptr;
        }
    }
    ~Multiset() {
        clear();
    }

    Multiset& operator=(Multiset&& other) noexcept {
        if (this == &other) return *this;

        clear();
        head = other.head;
        other.head = nullptr;

        return *this;
    }

    Multiset& operator=(const Multiset& other) {

        if (this == &other)
            return *this;

        clear();
        if (!other.head) {
            head = nullptr;
            return *this;
        }
        head = new Node(other.head->number, other.head->count);
        Node* current_this = head;
        current_this->next = nullptr;
        auto it = other.begin();
        ++it;
        for (; it != other.end(); ++it) {
            current_this->next = new Node(it->number, it->count);
            current_this = current_this->next;
            current_this->next = nullptr;
        }

        return *this;
    }

    bool isEmpty() {
        if (!head)
            return true;
        else
            return false;
    }

    Multiset Union(const Multiset& other) {
        Multiset union_sets;

        auto it1 = begin();
        auto it2 = other.begin();
        if (it1 == nullptr || it2 == nullptr) {
            if (it1 == nullptr) {
                if (it2 == nullptr)
                    return union_sets;
                else {
                    union_sets.head = new Node(it2->number, it2->count);
                    ++it2;
                }
            }
            else {
                union_sets.head = new Node(it1->number, it1->count);
                ++it1;
            }
        }
        else {
            if (it1->number > it2->number) {
                union_sets.head = new Node(it2->number, it2->count);
                ++it2;
            }
            else {
                if (it1->number == it2->number) {
                    if (it1->count > it2->count)
                        union_sets.head = new Node(it1->number, it1->count);
                    else
                        union_sets.head = new Node(it2->number, it2->count);
                    ++it1; ++it2;
                }
                else {
                    union_sets.head = new Node(it1->number, it1->count);
                    ++it1;
                }
            }
        }

        Node* current = union_sets.head;
        current->next = nullptr;

        while (it1 != end() && it2 != other.end()) {
            if (it1->number <= it2->number) {
                if (it1->number == it2->number) {
                    if (it1->count > it2->count) {
                        current->next = new Node(it1->number, it1->count);
                        current = current->next;
                        current->next = nullptr;
                    }
                    else {
                        current->next = new Node(it2->number, it2->count);
                        current = current->next;
                        current->next = nullptr;
                    }
                    ++it1; ++it2;
                }
                else {
                    current->next = new Node(it1->number, it1->count);
                    current = current->next;
                    current->next = nullptr;
                    ++it1;
                }
            }
            else {
                current->next = new Node(it2->number, it2->count);
                current = current->next;
                current->next = nullptr;
                ++it2;
            }

        }
        while (it1 != end()) {
            current->next = new Node(it1->number, it1->count);
            current = current->next;
            current->next = nullptr;
            ++it1;
        }
        while (it2 != other.end()) {
            current->next = new Node(it2->number, it2->count);
            current = current->next;
            current->next = nullptr;
            ++it2;
        }
        return union_sets;
    }

    Multiset Intersection(const Multiset& other) {
        Multiset intersection_sets;

        auto it1 = begin();
        auto it2 = other.begin();

        if (it1 == nullptr || it2 == nullptr) {
            return intersection_sets;
        }
        else {
            while (it1 != end() && it2 != other.end() && it1->number != it2->number) {
                if (it1->number < it2->number)
                    ++it1;
                else
                    if (it1->number > it2->number)
                        ++it2;
            }
            if (it1 == nullptr || it2 == nullptr) {
                return intersection_sets;
            }
            if (it1->number == it2->number) {
                if (it1->count > it2->count)
                    intersection_sets.head = new Node(it2->number, it2->count);
                else
                    intersection_sets.head = new Node(it1->number, it1->count);
                ++it1; ++it2;
            }
            else
                return intersection_sets;
        }

        Node* current = intersection_sets.head;
        current->next = nullptr;

        while (it1 != end() && it2 != other.end()) {
            if (it1->number <= it2->number) {
                if (it1->number == it2->number) {
                    if (it1->count < it2->count) {
                        current->next = new Node(it1->number, it1->count);
                        current = current->next;
                        current->next = nullptr;
                    }
                    else {
                        current->next = new Node(it2->number, it2->count);
                        current = current->next;
                        current->next = nullptr;
                    }
                    ++it1; ++it2;
                }
                else
                    ++it1;
            }
            else
                ++it2;
        }
        return intersection_sets;
    }

    Multiset Difference(const Multiset& other) {
        Multiset difference_sets;
        auto it1 = begin();
        auto it2 = other.begin();

        if (it1 == nullptr)
            return difference_sets;
        else {
            while (it1 != end() && it2 != other.end()) {
                if (it2->number < it1->number) {
                    ++it2;
                }
                else {
                    if (it1->number < it2->number) {
                        difference_sets.head = new Node(it1->number, it1->count);
                        ++it1;
                        break;
                    }
                    else {
                        if (it1->count > it2->count) {
                            difference_sets.head = new Node(it1->number, it1->count - it2->count);
                            ++it1; ++it2;
                            break;
                        }
                        else {
                            ++it1; ++it2;
                        }
                    }
                }
            }
            if (it1 == end())
                return difference_sets;

            if (it2 == other.end()) {
                if (!difference_sets.head) {
                    difference_sets.head = new Node(it1->number, it1->count);
                    ++it1;
                }
            }
        }

        Node* current = difference_sets.head;
        current->next = nullptr;

        while (it1 != end() && it2 != other.end()) {
            if (it1->number <= it2->number) {
                if (it1->number == it2->number) {
                    if (it1->count > it2->count) {
                        current->next = new Node(it1->number, it1->count - it2->count);
                        current = current->next;
                        current->next = nullptr;
                    }
                    ++it1; ++it2;
                }
                else {
                    current->next = new Node(it1->number, it1->count);
                    current = current->next;
                    current->next = nullptr;
                    ++it1;
                }
            }
            else
                ++it2;
        }
        if (it1 == end())
            return difference_sets;

        if (it2 == other.end())
            while (it1 != end()) {
                current->next = new Node(it1->number, it1->count);
                current = current->next;
                current->next = nullptr;
                ++it1;
            }

        return difference_sets;
    }

    Multiset Symmetric_Difference(const Multiset& other) {
        Multiset sym_diff_sets;
        auto it1 = begin();
        auto it2 = other.begin();

        if (it1 == nullptr && it2 == nullptr)
            return sym_diff_sets;
        else {
            if (it1 == nullptr)
                return other;
            if (it2 == nullptr)
                return *this;
        }
        if (it1->number < it2->number) {
            sym_diff_sets.head = new Node(it1->number, it1->count);
            ++it1;
        }
        else {
            if (it2->number < it1->number) {
                sym_diff_sets.head = new Node(it2->number, it2->count);
                ++it2;
            }
            else {
                sym_diff_sets.head = new Node(it1->number, abs(it1->count - it2->count));
                ++it1; ++it2;
            }
        }

        Node* current = sym_diff_sets.head;
        current->next = nullptr;

        while (it1 != end() && it2 != other.end()) {
            if (it1->number <= it2->number) {
                if (it1->number == it2->number) {
                    if (it1->count != it2->count) {
                        current->next = new Node(it1->number, abs(it1->count - it2->count));
                        current = current->next;
                        current->next = nullptr;
                    }
                    ++it1; ++it2;
                }
                else {
                    current->next = new Node(it1->number, it1->count);
                    current = current->next;
                    current->next = nullptr;
                    ++it1;
                }
            }
            else {
                current->next = new Node(it2->number, it2->count);
                current = current->next;
                current->next = nullptr;
                ++it2;
            }
        }
        if (it1 == end())
            while (it2 != other.end()) {
                current->next = new Node(it2->number, it2->count);
                current = current->next;
                current->next = nullptr;
                ++it2;
            }
        else
            while (it1 != end()) {
                current->next = new Node(it1->number, it1->count);
                current = current->next;
                current->next = nullptr;
                ++it1;
            }

        if (!sym_diff_sets.head->count) {
            Node* temp = sym_diff_sets.head;
            sym_diff_sets.head = sym_diff_sets.head->next;
            delete temp;
        }

        return sym_diff_sets;
    }

    Node* Get_Multiset() {
        return head;
    }

    Node& operator[](int index) const {
        int i = 0;
        for (auto it = begin(); it != end(); ++it) {
            if (i == index) {
                return *it;
            }
            i++;
        }
        throw "Error: Index out of range";
    }

    int operator[](int index) {
        int i = 0;
        for (auto it = begin(); it != end(); ++it) {
            if (i + it->count >= index) {
                return it->number;
            }
            i += it->count;
        }
        throw "Error: Index out of range";
    }

    int Power() {
        int power = 0;
        for (auto it = begin(); it != end(); ++it)
            power += it->count;
        return power;
    }

    friend bool operator==(const Multiset& a, const Multiset& b) {
        auto it1 = a.begin();
        auto it2 = b.begin();
        while (it1 != a.end() && it2 != b.end()) {
            if ((it1->number != it2->number) || (it1->count != it2->count))
                return false;
            ++it1; ++it2;
        }
        return it1 == a.end() && it2 == b.end();
    }

    friend bool operator!=(const Multiset& lhs, const Multiset& rhs) {
        return !(lhs == rhs);
    }

    // Проверка на вхождение *this в множество other
    bool in(const Multiset& other) {
        auto it1 = begin();
        auto it2 = other.begin();
        while (it1 != end() && it2 != other.end()) {
            if (it2->number <= it1->number) {
                if (it2->number == it1->number) {
                    if (it2->count < it1->count)
                        return false;
                    ++it1; ++it2;
                }
                else
                    ++it2;
            }
            else
                return false;
        }
        if (it1 != end())
            return false;
        return true;
    }

    Multiset& Add_First_Missing() {
        int missing = 0;
        for (auto it = begin(); it != end(); ++it) {
            if (it->number == missing)
                missing++;
            else {
                insert(missing, 1);
                return *this;
            }
        }
        insert(missing, 1);
        return *this;
    }

    friend ostream& operator<<(ostream& stream, const Multiset& multiset) {
        stream << '$';

        if (!multiset.head) {
            stream << '$';
            return stream;
        }

        bool first = true;
        for (auto it = multiset.begin(); it != multiset.end(); ++it) {
            for (int i = 0; i < it->count; i++) {
                if (!first) {
                    stream << ',';
                }
                stream << it->number;
                first = false;
            }
        }
        stream << '$';
        return stream;
    }

    friend istream& operator>>(istream& stream, Multiset& multiset) {
        char delimiter;

        multiset.clear();

        if (!(stream >> delimiter) || (delimiter != '$')) {
            cerr << "Error: Expected '$', but got '" << delimiter << "'\n";
            throw "Error: Input error";
        }

        int number;
        int count = 1;
        if (stream.peek() == '$') {
            stream.ignore();
            return stream;
        }
        else {
            while (stream >> number) {
                multiset.insert(number, count);
                if (!(stream >> delimiter) || (delimiter != ',' && delimiter != '$')) {
                    cerr << "Error: Expected ',' or '$', but got '" << delimiter << "'\n";
                    throw "Error: Input error";
                }
                if (delimiter == '$') {
                    return stream;
                }
            }
        }
        cerr << "Error: Unexpected symbol";
        throw "Error: Input error";
    }
};