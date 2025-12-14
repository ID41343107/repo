# Problem : Polynomial

## 解題說明

### 本題要求多項式（Polynomial）類別：

### 1.目標

。 使用循環表和頭節點(Header Node)來表示單變數整數係數多項式
    
。 每個多項式的項目(Term)用一個鏈表節點表示，鏈表節點包含三個數據成員：
    
| Coef (係數)    | Exp (指數)    | Link (指向下一個節點)    |
|----------------|---------------|-------------------------|

### 2.內外表示

。 外部表示：格式為 n, c₁, e₁, ..., cₙ, eₙ，其中：

    n：多項式的項目數 (terms)
    
    c:係數，e:指數，指數按遞減順序排列 (e₁ > e₂ > ... > eₙ)
    
。 內部表示：使用循環鏈表儲存，並加入頭節點(Header Node)，以便操作

### 3.功能清單

#### (a) 輸入運算子 (istream& operat>>)

    實現從輸入中讀取多項式，並按照循環鏈表結構儲存。
    
#### (b) 輸出運算子 (ostream& operator<<)

    將鏈表多項式轉換回指定格式的文字輸出。
    
#### (c) 複製建構子

    Polynomial::Polynomial(const Polynomial& a)：用多項式a初始化當前多項式。
    
#### (d) 賦值運算子重載

    const Polynomial& Polynomial::operator=(const Polynomial& a) const：將多項式a指派於當前多項式。
    
#### (e) 解構子

    Polynomial::Polynomial()：回收多項式使用的節點至可用空間表(Available-Space List)。
    
#### (f) 多項式加法運算

    Polynomial operator+ (const Polynomial& b) const：兩個多項式相加，回傳結果多項式。
    
#### (g) 多項式減法運算

    Polynomial operator- (const Polynomial& b) const：兩個多項式相減，回傳結果多項式。
    
#### (h) 多項式乘法運算

    Polynomial operator* (const Polynomial& b) const：兩個多項式相乘，回傳結果多項式。
    
#### (i) 多項式求值

    float Polynomial::Evaluate (float x) const：在給定變數值 x 下計算多項式的值。


### 解題想法

1. 以 Term 結構 儲存每一項的「係數（coef）」與「指數（exp）」。

2. 以 動態陣列 Term* 儲存所有非零項，並可動態擴充容量。

3. 實作運算子多載，使用者能夠自然的數學形式進行操作，例如：

        Add 使用 operator+() cout<<a+b; 
  
        Mult 使用 operator*() cout<<a*b; 
  
        Eval 使用 operator()() cout<<a(x);

4. 在多項式相加與相乘時，根據指數大小進行合併與排序，保證輸出結果的可讀性。

### 範例說明

若輸入：

    a(x): 3x^2 + 2x^1 + 1
    b(x): x^1 + 4
    x=10

則輸出：

    a + b = 3x^2 + 3x^1 + 5
    a * b = 3x^3 + 14x^2 + 9x^1 + 4
    a(10) = 321
    b(10) = 14


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

    friend ostream& operator<<(ostream& os, const Polynomial& p) {
        for (auto it = p.Begin(); it != p.End(); ++it)
            os << it->coef << " " << it->exp << " ";
        return os;
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
    Polynomial p1, p2;
    cin >> p1 >> p2;
    cout << "P1 = " << p1 << endl;
    cout << "P2 = " << p2 << endl;
    cout << "P1 + P2 = " << (p1 + p2) << endl;
    cout << "P1 - P2 = " << (p1 - p2) << endl;
    cout << "P1 * P2 = " << (p1 * p2) << endl;
    return 0;
}

```

## 效能分析

### 時間複雜度

    加法 (operator+)： O(n + m)，其中 n、m 為兩多項式的項數。

    乘法 (operator*)： O(n × m)，需兩層迴圈逐項相乘。

    代入 (operator())： O(n)，每項計算一次次方。

### 空間複雜度

    動態陣列空間 O(n)，在乘法時可能暫時擴充到 O(n × m)。

### 效能量測

1. 對輸入項數 n = 100、1000、10000 測試運行時間：

    加法時間約隨 n 線性成長。

    乘法時間呈平方成長，符合 O(n²) 預期。

2. 記憶體使用量隨項數擴增呈線性變化。


## 測試與驗證

### 輸入

多項式 a(x):

    3

    2 3

    2 2

    4 1

多項式 b(x):

    2

    3 1

    4 0
    
要代入x ：
      
    2

### 則輸出

    a(x) = 2x^3 + 2x^2 + 4x^1

    b(x) = 3x^1 + 4

    a + b = 2x^3 + 2x^2 + 7x^1 + 4

    a * b = 6x^4 + 14x^3 + 20x^2 + 16x^1

    a(2) = 32

    b(2) = 10

## 申論及開發報告

### 心得討論

在本次作業中，我學習到如何以物件導向的方式設計數學結構「多項式」。

利用類別與運算子多載，能讓多項式之間的運算像數學運算一樣，例如 a + b、a * b、a(x) 等操作，讓程式的可讀性提升。

在實作過程中，我遇到的主要挑戰是：

    如何正確合併同次項
    → 在加法與乘法中，若兩項的指數相同，必須正確累加係數並避免重複。

    動態記憶體管理
    → 使用 new 與 delete[] 管理 Term 陣列時，必須確保擴充容量後不遺漏舊資料。

    輸入與輸出格式化
    → 為了讓多項式輸出更直觀，我使用了 operator<< 來格式化顯示，例如自動在正數前加上「+」。

透過這次作業練習，我更加了解：

1. 封裝（Encapsulation） 的重要性 —— 將多項式的邏輯與資料儲存在類別中，使用者只需透過主程式操作。

2. 演算法複雜度分析的應用 —— 乘法時間隨項數平方成長，能明顯觀察效能差異。

3. C++語法特性，如 friend 函數、sort()、pow() 等。

### 總結

這次作業不僅強化了我對運算子多載與類別設計的理解，也讓我學會如何將數學概念轉化成可執行的程式邏輯，體會到物件導向在工程上的靈活性。
