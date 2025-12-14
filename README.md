# Homework 3 
[Programming Project] Develop a C++ class Polynomial to represent and manipulate univariate polynomials with integer coefficients (use circular linked lists with header nodes). Each term of the polynomial will be represented as a node. Thus, a node in this system will have three data members as below:

| coef  | exp  | link  |
|-------|------|-------|

Each polynomial is to be represented as a circular list with header node. To delete polynomials efficiently, we need to use an available-space list and associated functions as described in Section 4.5. The external (i.e., for input or output) representation of a univariate polynomial will be assumed to be a sequence of integers of the form: n, c₁, e₁, c₂, e₂, c₃, е₃, ..., cₙ, eₙ, where e; represents an exponent and c₁ a coefficient; n gives the number of terms in the polynomial. The exponents are in decreasing order----e₁> e₂ > ... > eₙ.

## Write and test the following functions:

(a) istream& operator>>(istream& is, Polynomial& x): Read in an input polynomial and convert it to its circular list representation using a header node.

(b) ostream& operator<<(ostream& os, Polynomial& x): Convert x from its linked list representation to its external representation and output it.

(c) Polynomial :: Polynomial(const Polynomial& a) [Copy Constructor]: Initialize the polynomial *this to the polynomial a.

(d) const Polynomial& Polynomial::operator=(const Polynomial& a) const [Assignment Operator]: Assign polynomial a to *this.

(e) Polynomial:: Polynomial () [Destructor]: Return all nodes of the polynomial *this to the available-space list.

(f) Polynomial operator+ (const Polynomial& b) const [Addition]: Create and return the polynomial *this + b.

(g) Polynomial operator- (const Polynomial& b) const [Subtraction]: Create and return the polynomial *this - b.

(h) Polynomial operator*(const Polynomial& b) const [Multiplication]: Create and return the polynomial *this * b.

(i) float Polynomial::Evaluate (float x) const: Evaluate the polynomial *this at x and return the result.

## 依序實作:

1. template <class T>class ChainNode

2. template<class T>class Chain

3. template<class T> class ChainIterator

   Chain<int>::iterator xHere = x.Begin();

   Chain<int>::iterator xEnd = x.End();

5. template <class T>class Polynomial

   Polynomial Representation

   Circular List Representation of Polynomials

7. Available Lists (加分項)
