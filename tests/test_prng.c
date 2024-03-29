/*************************************************************************************************/
/*  Testing PRNG                                                                                 */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_prng.h"
#include "_prng.h"
#include "test.h"
#include "testing.h"



/*************************************************************************************************/
/*  Tests                                                                                        */
/*************************************************************************************************/

int test_prng_1(TstSuite* suite)
{
    DvzPrng* prng = dvz_prng();
    uint64_t uuid = dvz_prng_uuid(prng);
    log_info("random uuid is %" PRIu64, uuid);
    dvz_prng_destroy(prng);
    return 0;
}
