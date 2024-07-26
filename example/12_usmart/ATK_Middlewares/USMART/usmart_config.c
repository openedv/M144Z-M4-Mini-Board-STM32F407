#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* user configuration area
 * This should include the header file declared by the function used (user added)
 */

#include "../../BSP/LCD/lcd.h"

extern void led_set(uint8_t sta);
extern void test_fun(void(*ledset)(uint8_t), uint8_t sta);

/* Function name list initialization (user added)
 * Here the user enters directly the name of the function to be executed and its lookup string
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* If read and write operations are enabled */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr,uint32_t val)",
#endif

    (void *)led_set, "void led_set(uint8_t sta)",
    (void *)test_fun, "void test_fun(void(*ledset)(uint8_t), uint8_t sta)",
};

/******************************************************************************************/

/* The function controls the manager initialization
 * Get the names of each controlled function
 * Get the total number of functions
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* Function quantity */
    0,      /* Arity */
    0,      /* Function ID */
    1,      /* Parameter display type,0, decimal; 1, hexadecimal */
    0,      /* Parameter type.bitx:0, digit; 1, string */
    0,      /* The length temporary table for each parameter requires MAX PARM initialization*/
    0,      /* The parameter of the function requires PARM LEN initialization */
};



















