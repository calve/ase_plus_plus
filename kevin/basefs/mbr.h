/* ------------------------------
   $Id: mbr.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   MBR definition and manipualtion.
   Philippe Marquet, october 2002
   
*/

#ifndef _MBR_H_
#define _MBR_H_

#define MAX_VOL		8

enum vol_type_e {
    base,
    annex,
    other
};

struct vol_descr_s {
    unsigned int vol_first_cylinder;
    unsigned int vol_first_sector;
    unsigned int vol_n_bloc;
    enum vol_type_e vol_type;
};

struct mbr_descr_s {
    unsigned int mbr_magic; /* MBR_MAGIC */
    unsigned int mbr_n_vol; 
    struct vol_descr_s mbr_vol[MAX_VOL]; /* first mbr_n_vol are in used */
};

extern struct mbr_descr_s mbr;

/* return RETURN_FAILURE in case of a non preexisting mbr */
int load_mbr();

void save_mbr();

char char_of_vol_type(enum vol_type_e);
enum vol_type_e vol_type_of_char(char);

#endif
