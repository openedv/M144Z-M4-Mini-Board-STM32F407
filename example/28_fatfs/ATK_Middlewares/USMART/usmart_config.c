#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* user configuration area
 * This should include the header file declared by the function used (user added)
 */

#include "../../BSP/LCD/lcd.h"
#include "../../SYSTEM/delay/delay.h"
#include "../../Middlewares/Third_Party/FatFS/exfuns/fattester.h"


/* Function name list initialization (user added)
 * Here the user enters directly the name of the function to be executed and its lookup string
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* If read and write operations are enabled */
	    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
	    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
	#endif
	    (void *)delay_ms, "void delay_ms(uint16_t nms)",
	    (void *)delay_us, "void delay_us(uint32_t nus)",

	    (void *)mf_mount, "uint8_t mf_mount(uint8_t* path,uint8_t mt)",
	    (void *)mf_open, "uint8_t mf_open(uint8_t*path,uint8_t mode)",
	    (void *)mf_close, "uint8_t mf_close(void)",
	    (void *)mf_read, "uint8_t mf_read(uint16_t len)",
	    (void *)mf_write, "uint8_t mf_write(uint8_t*dat,uint16_t len)",
	    (void *)mf_opendir, "uint8_t mf_opendir(uint8_t* path)",
	    (void *)mf_closedir, "uint8_t mf_closedir(void)",
	    (void *)mf_readdir, "uint8_t mf_readdir(void)",
	    (void *)mf_scan_files, "uint8_t mf_scan_files(uint8_t * path)",
	    (void *)mf_showfree, "uint32_t mf_showfree(uint8_t *path)",
	    (void *)mf_lseek, "uint8_t mf_lseek(uint32_t offset)",
	    (void *)mf_tell, "uint32_t mf_tell(void)",
	    (void *)mf_size, "uint32_t mf_size(void)",
	    (void *)mf_mkdir, "uint8_t mf_mkdir(uint8_t*path)",
	    (void *)mf_fmkfs, "uint8_t mf_fmkfs(uint8_t* path,uint8_t opt,uint16_t au)",
	    (void *)mf_unlink, "uint8_t mf_unlink(uint8_t *path)",
	    (void *)mf_rename, "uint8_t mf_rename(uint8_t *oldname,uint8_t* newname)",
	    (void *)mf_getlabel, "void mf_getlabel(uint8_t *path)",
	    (void *)mf_setlabel, "void mf_setlabel(uint8_t *path)",
	    (void *)mf_gets, "void mf_gets(uint16_t size)",
	    (void *)mf_putc, "uint8_t mf_putc(uint8_t c)",
	    (void *)mf_puts, "uint8_t mf_puts(uint8_t *str)",
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



















