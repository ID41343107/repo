# Problem : Polynomial

## 解題說明

本題要求多項式（Polynomial）類別，能夠進行：

    加法運算（operator+()）

    乘法運算（operator*()）

    值代入運算（operator()()）

    以及多項式的輸入與輸出格式化（operator>>()、operator<<()）

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
#include <cmath>
#include <algorithm>
using namespace std;

class Polynomial; // forward declaration

class Term {
    friend Polynomial;
    friend ostream& operator<<(ostream& output, const Polynomial& Poly);
private:
    float coef; // 系數
    int exp;    // 指數
};

class Polynomial {
private:
    Term* termArray;  // 儲存非零項的動態陣列
    int capacity;     // 陣列大小
    int terms;        // 非零項數量

public:
    // p(x) = 0
    Polynomial() : capacity(10), terms(0) {
        termArray = new Term[capacity];
    }
    ~Polynomial() {
        delete[] termArray;
    }

    void NewTerm(const float c, const int e) {
        if (c == 0) return;
        if (terms == capacity) {
            capacity *= 2;
            Term* temp = new Term[capacity];
            copy(termArray, termArray + terms, temp);
            delete[] termArray;
            termArray = temp;
        }
        termArray[terms].coef = c;
        termArray[terms++].exp = e;
    }

    Polynomial operator+(const Polynomial& poly) const {
        Polynomial result;
        int a = 0, b = 0;
        while (a < terms && b < poly.terms) {
            if (termArray[a].exp == poly.termArray[b].exp) {
                float sum = termArray[a].coef + poly.termArray[b].coef;
                if (sum != 0)
                    result.NewTerm(sum, termArray[a].exp);
                a++; b++;
            }
            else if (termArray[a].exp > poly.termArray[b].exp) {
                result.NewTerm(termArray[a].coef, termArray[a].exp);
                a++;
            }
            else {
                result.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
                b++;
            }
        }
        for (; a < terms; a++)
            result.NewTerm(termArray[a].coef, termArray[a].exp);
        for (; b < poly.terms; b++)
            result.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
        return result;
    }

    Polynomial operator*(const Polynomial& poly) const {
        Polynomial result;
        for (int i = 0; i < terms; i++) {
            for (int j = 0; j < poly.terms; j++) {
                float c = termArray[i].coef * poly.termArray[j].coef;
                int e = termArray[i].exp + poly.termArray[j].exp;
                bool found = false;
                for (int k = 0; k < result.terms; k++) {
                    if (result.termArray[k].exp == e) {
                        result.termArray[k].coef += c;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    result.NewTerm(c, e);
            }
        }
        sort(result.termArray, result.termArray + result.terms,
            [](Term a, Term b) { return a.exp > b.exp; });
        return result;
    }

    float operator()(float x) const {
        float sum = 0;
        for (int i = 0; i < terms; i++)
            sum += termArray[i].coef * pow(x, termArray[i].exp);
        return sum;
    }

    friend istream& operator>>(istream& input, Polynomial& Poly) {
        int n;
        float coef;
        int exp;
        input >> n;
        for (int i = 0; i < n; i++) {
            input >> coef >> exp;
            Poly.NewTerm(coef, exp);
        }
        return input;
    }

    friend ostream& operator<<(ostream& os, const Polynomial& poly) {
        if (poly.terms == 0) {
            os << "0";
            return os;
        }
        for (int i = 0; i < poly.terms; i++) {
            if (i > 0 && poly.termArray[i].coef > 0)
                os << " + ";
            else if (poly.termArray[i].coef < 0)
                os << " ";
            os << poly.termArray[i].coef;
            if (poly.termArray[i].exp != 0)
                os << "x^" << poly.termArray[i].exp;
        }
        return os;
    }
};

int main() {
    Polynomial a, b;

    cout << "多項式 a(x):" << endl;
    cin >> a;
    cout << "\n多項式 b(x):" << endl;
    cin >> b;

    cout << "\na(x) = " << a << endl;
    cout << "b(x) = " << b << endl;

    cout << "\na + b = " << a + b << endl;
    cout << "a * b = " << a * b << endl;

    float x;
    cout << "\n要代入的 x ：";
    cin >> x;
    cout << "a(" << x << ") = " << a(x) << endl;
    cout << "b(" << x << ") = " << b(x) << endl;

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
