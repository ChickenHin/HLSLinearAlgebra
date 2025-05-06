#include <iostream>
#include <iomanip>
#include "Posit.h"

int main() {
    using P16 = Posit<16, 2>;

    auto print = [](const char* label, const P16& p) {
        std::cout << label << " bits=0x" << std::hex << p.bits.to_uint()
                  << " float approx=" << std::dec << p.to_float() << "\n";
    };

    // Test 1: Addition
    P16 a(0x4000); // ~1.0
    P16 b(0x4800); // ~1.25
    P16 c = a + b;
    print("Add 1 + 1.25 =", c);

    // Test 2: Subtraction
    P16 d = b - a;
    print("Sub 1.25 - 1 =", d);

    // Test 3: Multiplication
    P16 e = a * b;
    print("Mul 1 * 1.25 =", e);

    // Test 4: Division
    P16 f = b / a;
    print("Div 1.25 / 1 =", f);

    // Test 5: Zero and NaR
    P16 zero(0x0000);
    P16 nar(0x8000);
    print("Zero =", zero);
    print("NaR =", nar);
    print("Add NaR + 1 =", nar + a);
    print("Mul Zero * 1 =", zero * a);

    // Test 6: Denormal handling (small value + 1)
    P16 tiny(0x0001); // very small posit
    P16 g = tiny + a;
    print("Tiny + 1 =", g);

    // Test 7: Overflow check (maxpos * maxpos)
    P16 maxpos(0x7FFF);
    P16 h = maxpos * maxpos;
    print("Maxpos * Maxpos =", h);

    return 0;
}
