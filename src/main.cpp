#include <iostream>
#include <cmath>
using namespace std;

template<class T>
class ChainNode {
public:
    T element;
    ChainNode<T>* link;
    ChainNode(const T& e = T(), ChainNode<T>* l = nullptr):element(e), link(l) {}
};

template<class T>
class ChainIterator {
public:
    ChainNode<T>* current;
    ChainIterator(ChainNode<T>* start = nullptr) : current(start) {}
    T& operator*() const { return current->element; }
    T* operator->() const { return &current->element; }
    ChainIterator<T>& operator++() {
        current = current->link;
        return *this;
    }
    ChainIterator<T> operator++(int) {
        ChainIterator<T> old = *this;
        current = current->link;
        return old;
    }
    bool operator==(const ChainIterator<T>& rhs) const {
        return current == rhs.current;
    }
    bool operator!=(const ChainIterator<T>& rhs) const {
        return current != rhs.current;
    }
};

template<class T>
class Chain {
protected:
    ChainNode<T>* header;
public:
    typedef ChainIterator<T> iterator;
    Chain() {
        header = new ChainNode<T>();
        header->link = header;
    }
    ~Chain() { Release(); delete header; }
    bool IsEmpty() const {
        return header->link == header;
    }
    iterator Begin() const { return iterator(header->link); }
    iterator End() const { return iterator(header); }
    void Release() {
        ChainNode<T>* cur = header->link;
        while (cur != header) {
            ChainNode<T>* del = cur;
            cur = cur->link;
            delete del;
        }
        header->link = header;
    }
    void InsertBack(const T& e) {
        ChainNode<T>* cur = header;
        while (cur->link != header)
            cur = cur->link;
        cur->link = new ChainNode<T>(e, header);
    }
};

struct Term {
    int coef;
    int exp;
};

class AvailableList {
private:
    ChainNode<Term>* head;
public:
    AvailableList() : head(nullptr) {}
    bool IsEmpty() const { return head == nullptr; }
    void GetBack(ChainNode<Term>* node) {
        node->link = head;
        head = node;
    }
    ChainNode<Term>* GetNode() {
        if (IsEmpty()) return nullptr;
        ChainNode<Term>* node = head;
        head = head->link;
        node->link = nullptr;
        return node;
    }
};

class Polynomial : public Chain<Term> {
private:
    static AvailableList Ava;

public:
    Polynomial() : Chain<Term>() {}
    ~Polynomial() {
        ChainNode<Term>* cur = header->link;
        while (cur != header) {
            ChainNode<Term>* del = cur;
            cur = cur->link;
            Ava.GetBack(del);
        }
        header->link = header;
    }
    void NewTerm(int c, int e) {
        if (c == 0) return;
        ChainNode<Term>* prev = header;
        ChainNode<Term>* cur = header->link;
        while (cur != header && cur->element.exp > e) {
            prev = cur;
            cur = cur->link;
        }
        if (cur != header && cur->element.exp == e) {
            cur->element.coef += c;
            if (cur->element.coef == 0) {
                prev->link = cur->link;
                Ava.GetBack(cur);
            }
        }
        else {
            ChainNode<Term>* node = Ava.GetNode();
            if (!node) node = new ChainNode<Term>();
            node->element.coef = c;
            node->element.exp = e;
            node->link = cur;
            prev->link = node;
        }
    }

    Polynomial operator+(const Polynomial& b) const {
        Polynomial c;
        auto it1 = Begin();
        auto it2 = b.Begin();
        while (it1 != End() && it2 != b.End()) {
            if (it1->exp == it2->exp) {
                c.NewTerm(it1->coef + it2->coef, it1->exp);
                ++it1; ++it2;
            }
            else if (it1->exp > it2->exp) {
                c.NewTerm(it1->coef, it1->exp);
                ++it1;
            }
            else {
                c.NewTerm(it2->coef, it2->exp);
                ++it2;
            }
        }
        while (it1 != End()) {
            c.NewTerm(it1->coef, it1->exp);
            ++it1;
        }
        while (it2 != b.End()) {
            c.NewTerm(it2->coef, it2->exp);
            ++it2;
        }
        return c;
    }

    Polynomial operator-(const Polynomial& b) const {
        Polynomial c;
        auto it1 = Begin();
        auto it2 = b.Begin();
        while (it1 != End() && it2 != b.End()) {
            if (it1->exp == it2->exp) {
                c.NewTerm(it1->coef - it2->coef, it1->exp);
                ++it1; ++it2;
            }
            else if (it1->exp > it2->exp) {
                c.NewTerm(it1->coef, it1->exp);
                ++it1;
            }
            else {
                c.NewTerm(-it2->coef, it2->exp);
                ++it2;
            }
        }
        while (it1 != End()) {
            c.NewTerm(it1->coef, it1->exp);
            ++it1;
        }
        while (it2 != b.End()) {
            c.NewTerm(-it2->coef, it2->exp);
            ++it2;
        }
        return c;
    }

    Polynomial operator*(const Polynomial& b) const {
        Polynomial c;
        for (auto it1 = Begin(); it1 != End(); ++it1) {
            for (auto it2 = b.Begin(); it2 != b.End(); ++it2) {
                c.NewTerm(it1->coef * it2->coef, it1->exp + it2->exp);
            }
        }
        return c;
    }

    friend ostream& operator<< (ostream& os, const Polynomial& poly) {
    for (ChainIterator<Term> it = poly.Begin(); it != poly.End(); ++it) {
    if (it!=poly.Begin()) os << " + ";
    os << it->coef << "x^" << it->exp << " ";
    }
    return os;
    }

    float Eval(float x) const {
        float result = 0;
        for (auto it = Begin(); it != End(); ++it) {
            result += it->coef * std::pow(x, it->exp);
        }
        return result;
    }

    friend istream& operator>>(istream& is, Polynomial& p) {
        int n, c, e;
        is >> n;
        for (int i = 0; i < n; i++) {
            is >> c >> e;
            p.NewTerm(c, e);
        }
        return is;
    }
};

AvailableList Polynomial::Ava;

int main() {
    int x;
    Polynomial p1, p2;
    cin >> p1 >> p2;
    cin >> x;
    cout << "P1 = " << p1 << endl;
    cout << "P2 = " << p2 << endl;
    cout << "P1 + P2 = " << (p1 + p2) << endl;
    cout << "P1 - P2 = " << (p1 - p2) << endl;
    cout << "P1 * P2 = " << (p1 * p2) << endl;
    cout << "P1("<< x <<") = " << p1.Eval(x) << endl;
    cout << "P2("<< x <<") = " << p2.Eval(x) << endl;
    return 0;
}
