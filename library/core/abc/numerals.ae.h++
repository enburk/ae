struct integer { int64_t x; };

operator: (Z) + (Z) → Z;
operator: (Z) - (Z) → Z;
operator: (Z) × (Z) → Z;

operator: (Z) div (Z) → Z;
operator: (Z) mod (Z) → Z;

struct real { double x; };

operator: (R) + (R) → R;
operator: (R) - (R) → R;
operator: (R) × (R) → R;
operator: (R) / (R) → R;

type: real extension:
{
    property: round → int;
    property: floor → int;
    property: ceil  → int;
}

coercion: (int) → real;
