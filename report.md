# Problem : Polynomial

## 解題說明

作業要求實作一個 Polynomial 類別，用來表示與操作單變數整數係數多項式。

### 資料成員:

| coef(係數)   | exp(指數)    | link(指向下一個節點)    |
|---------|--------|---------|

### 實作:

1. template <class T>class ChainNode

2. template<class T>class Chain

3. template<class T> class ChainIterator

   Chain<int>::iterator xHere = x.Begin();

   Chain<int>::iterator xEnd = x.End();

4. template <class T>class Polynomial

   Polynomial Representation

   Circular List Representation of Polynomials

5. Available Lists (加分項)

## 解題想法

。 利用 Circular Linked List 搭配 Header Node 來表示多項式，每一項以節點儲存其係數與指數。Header node 不存資料，能簡化串列的插入、刪除與空串列的判斷。

。 多項式中的項目依指數遞減排序，使加法與減法可像合併兩個排序串列般同時走訪，提升效率。新增項目時，若指數相同則合併係數，合併後係數為零則刪除該節點。

。 有效管理記憶體，刪除的節點會回收到 Available List，新增節點時優先重複使用，減少動態配置成本。

。 乘法用雙層迴圈，將兩項相乘並插入結果多項式中，由系統自動合併同指數項。多項式求值則逐項計算 coef^exp 並加總。

## 程式實作

以下為主程式

```cpp
#include <iostream>
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
    cin>>x;
    cout << "P1 = " << p1 << endl;
    cout << "P2 = " << p2 << endl;
    cout << "P1 + P2 = " << (p1 + p2) << endl;
    cout << "P1 - P2 = " << (p1 - p2) << endl;
    cout << "P1 * P2 = " << (p1 * p2) << endl;
    cout << "P1("<< x <<") = " << p1.Eval(x) << endl;
    cout << "P2("<< x <<") = " << p2.Eval(x) << endl;
    return 0;
}

```

## 效能分析

### 時間複雜度

設多項式 P 與 Q 的項數分別為 n 與 m。

| 操作                         | 時間複雜度                             |
| --------------------------- | -------------------------------------- |
|  新增（NewTerm）             |   **O(n)**                             |   
| 輸入（operator>>）           | **O(n²)**                              |
| 輸出（operator<<）           | **O(n)**                               |
| 加法（operator+）            | **O(n+m)**                             |
| 減法（operator-）            | **O(n+m)**                             |
| 乘法（operator*）            | **O(n·m·k)**（k為結果項數）             |
| 求值（Eval）                 | **O(n)**                               |
| 取和還節點 (Available list)  | **O(1)**                               |

### 空間複雜度

設多項式 P 與 Q 的項數分別為 n 與 m。

| 操作             | 額外需求         | 空間複雜度        |
| ---------------- | -------------- | ----------------- |
| 儲存單一多項式    | 儲存 n 個項目      | **O(n)**        |
| 輸入             | 建立 n 個節點      | **O(n)**        |
| 輸出             | 僅使用暫存變數      | **O(1)**       |
| 加法             | 結果最多 n+m 個項目 | **O(n+m)**      |
| 減法             | 結果最多 n+m 個項目 | **O(n+m)**      |
| 乘法             | 結果最多 n·m 個項目 | **O(n·m)**      |
| 求值             | 使用常數暫存空間    | **O(1)**        |
| Available list  | 無                 | **O(n)**        |


### 效能量測

。 加法與減法運算利用多項式指數遞減排序，可同時走訪兩個串列完成，隨多項式項數線性成長，時間複雜度約 O(n+m)。

。 多項式求值與輸出僅需走訪所有項目，時間複雜度為 O(n)。乘法需對每一項與另一個多項式的每一項相乘，並插入結果多項式，最壞情況時間複雜度為 O(n·m·k)（k 為結果項數），隨項數增加快速上升。

。 使用 available list 能重複利用節點，減少 new/delete 次數，提高實際執行效率，但不改變時間複雜度。

。 實際測試可觀察到加法與減法隨項數增加執行時間穩定成長，而乘法則是主要的效能瓶頸。

## 測試與驗證

### 輸入

    3 2 3 2 2 4 1
    
    2 3 1 4 0
    
    2

### 輸出

    P1 = 2x^3 + 2x^2 + 4x^1
    
    P2 = 3x^1 + 4x^0
    
    P1 + P2 = 2x^3 + 2x^2 + 7x^1 + 4x^0
    
    P1 - P2 = 2x^3 + 2x^2 + 1x^1 + -4x^0
    
    P1 * P2 = 6x^4 + 14x^3 + 20x^2 + 16x^1
    
    P1(2) = 32
    
    P2(2) = 10

## 申論及開發報告

### 心得討論

透過本次作業，我對多項式的鏈結串列表示法有深刻的理解。使用 circular linked list 搭配 header node，使插入、刪除與空串列判斷更加簡潔，程式結構清楚。

實作 available list 讓我體會到記憶體重複利用的重要性，雖然在理論上不改變複雜度，但在實作上能有效提升效率。

在多項式運算中，加法與減法利用排序串列進行合併，效率良好，但乘法仍是效能瓶頸，特別是在項數增加時，需考慮可能的最佳化策略。

整體而言，這次作業讓我熟悉了物件導向設計、動態記憶體管理以及鏈結串列操作，對資料結構與演算法的實作能力有明顯提升。

### 總結

作業以 circular linked list 搭配 header node 表示多項式，並實作了加法、減法、乘法及求值運算。

透過 available list，有效管理節點記憶體，提升執行效率。

整體程式結構清晰，操作符合理論時間與空間複雜度分析，並提供了良好的可讀性與可維護性。

此次實作讓我對鏈結串列、多項式運算及記憶體管理有更深的理解和操作經驗。
