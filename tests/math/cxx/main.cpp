#include <hx/math.h>
#include <hx/test.h>

/* The library's headers from C++.
 *
 * A function of each header that has one in the archive is called, so the program links only
 * when the header declares it as C. The report's own header is held the same way. Each row is
 * a result the header promises to the last place. */

namespace {

volatile hx_fix16_t below_zero = HX_FIX16(-1.0);
volatile hx_fix16_t four = HX_FIX16(4.0);
volatile hx_fix16_t none = 0;

} // namespace

int
main()
{
        /* fix16.h: 0 below 0, and never under the root, at most one place over */
        hx_test_equal("fix16.sqrt.below-zero", static_cast<uint32_t>(hx_sqrt(below_zero)), 0);
        hx_test_equal("fix16.sqrt",
            static_cast<uint32_t>(hx_sqrt(four) - HX_FIX16(2.0)) <= 1U, 1);

        /* trig.h: 0 for (0, 0) */
        hx_test_equal("trig.atan2", static_cast<uint16_t>(hx_atan2(none, none)), 0);

        /* vec2.h: the length, exact */
        const hx_vec2_t sides = { HX_FIX16(3.0), HX_FIX16(4.0) };
        hx_test_equal("vec2.length", static_cast<uint32_t>(hx_vec2_length(&sides)),
            static_cast<uint32_t>(HX_FIX16(5.0)));

        /* vec3.h: x by y is z */
        const hx_vec3_t x = { HX_FIX16(1.0), 0, 0 };
        const hx_vec3_t y = { 0, HX_FIX16(1.0), 0 };
        hx_vec3_t z;
        hx_vec3_cross(&x, &y, &z);
        hx_test_equal("vec3.cross.x", static_cast<uint32_t>(z.x), 0);
        hx_test_equal("vec3.cross.y", static_cast<uint32_t>(z.y), 0);
        hx_test_equal("vec3.cross.z", static_cast<uint32_t>(z.z),
            static_cast<uint32_t>(HX_FIX16(1.0)));

        /* mat.h */
        hx_mat_t identity;
        hx_mat_identity(&identity);
        hx_test_equal("mat.identity.diagonal", static_cast<uint16_t>(identity.rotation[1][1]),
            static_cast<uint16_t>(HX_FIX14(1.0)));
        hx_test_equal("mat.identity.other", static_cast<uint16_t>(identity.rotation[1][2]), 0);
        hx_test_equal("mat.identity.translation",
            static_cast<uint32_t>(identity.translation.z), 0);

        hx_test_end();
        hx_test_show();
        return 0;
}
