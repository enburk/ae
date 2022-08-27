type: complex;
type: real     narrows: complex;
type: rational narrows: real;
type: integer  narrows: rational;
type: natural  narrows: integer;

type: complex
{
    real re;
    real im;
}

type: complex extension:
{
    property: re → real;
    property: re ← real;
    property: im → real;
    property: im ← real;
}

type: rational
{
    int p; let: numerator   = p;
    int q; let: denominator = q;

    invariant: q ≠ 0;

    contructor: (int i) { p ← i, q ← 1 }

    function: real = real(same(p,1)) / real(same(q,1));

    operator: = (same x) = p × x.q = q × x.p;
    operator: < (same x) = 
        q > 0 and x.q > 0 or
        q < 0 and x.q < 0 ?
        p × x.q < q × x.p |
        p × x.q > q × x.p;

    operator: + (same x) = same(p × x.q + q × x.p, q × x.q);
    operator: - (same x) = same(p × x.q - q × x.p, q × x.q);
    operator: × (same x) = same(p × x.p,  q × x.q);
    operator: / (same x) = same(p × x.q,  q × x.p);

    operator: ^ (int n) = n < 0 ? same(q^n, p^n) | same(p^n, q^n);

    function: canonicalize
    {
        x := gcd(p, q);
        p /= x, q /= x;
        if: q < 0 then:
        p ← -p, q ← -q;
        return: void;
    }
    function: canonical
    {
        x := self.value;
        x.canonicalize();
        return: x;
    }
}

operator: - (rational x) = rational(-x.p, x.q);

let: int = integer;

let: N = natural;
let: Z = integer;
let: Q = rational;
let: R = real;
let: C = complex;

operator: +(N) → N;
operator: +(Z) → Z;
operator: +(Q) → Q;
operator: +(R) → R;
operator: +(C) → C;

operator: -(N) → Z;
operator: -(Z) → Z;
operator: -(Q) → Q;
operator: -(R) → R;
operator: -(C) → C;

operator: (N) + (N) → N;
operator: (N) - (N) → Z;
operator: (N) × (N) → N;
operator: (N) / (N) → Q;  ;; 0?

operator: (Z) + (Z) → Z;
operator: (Z) - (Z) → Z;
operator: (Z) × (Z) → Z;
operator: (Z) / (Z) → Q;  ;; 0?

operator: (Q) + (Q) → Q;
operator: (Q) - (Q) → Q;
operator: (Q) × (Q) → Q;
operator: (Q) / (Q) → Q;  ;; 0?

operator: (R) + (R) → R;
operator: (R) - (R) → R;
operator: (R) × (R) → R;
operator: (R) / (R) → R;  ;; 0?

operator: (C) + (C) → C;
operator: (C) - (C) → C;
operator: (C) × (C) → C;
operator: (C) / (C) → C;  ;; 0?

type: boolean = true or: false;

operator: not (boolean);
operator: (boolean) or  (boolean);
operator: (boolean) and (boolean);
operator: (boolean) xor (boolean);

operator: (x a) ≤ (x b) = a = b or a < b;
operator: (x a) ≥ (x b) = a = b or a > b;
