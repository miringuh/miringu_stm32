#if !defined(_SD_REGS)
#define _SD_REGS

uint8_t buffd[16];
uint8_t buffx[128];

/** status for card in the ready state */
uint8_t const R1_READY_STATE = 0X00;
/** status for card in the idle state */
uint8_t const R1_IDLE_STATE = 0X01;
/** status bit for illegal command */
uint8_t const R1_ILLEGAL_COMMAND = 0X04;
/** start data token for read or write single block*/
uint8_t const DATA_START_BLOCK = 0XFE;
/** stop token for write multiple blocks*/
uint8_t const STOP_TRAN_TOKEN = 0XFD;
/** start data token for write multiple blocks*/
uint8_t const WRITE_MULTIPLE_TOKEN = 0XFC;
/** mask for data response tokens after a write block operation */
uint8_t const DATA_RES_MASK = 0X1F;
/** write data accepted token */
uint8_t const DATA_RES_ACCEPTED = 0X05;
//------------------------------------------------------------------------------
// BYTE 15
#define CSD_STRUCT 0XC0  // 2
#define CSD_RESERV1 0X3F // 6
// BYTE 14
#define CSD_TAAC 0XFF // 8
// BYTE 13
#define CSD_NSAC 0XFF // 8
// BYTE 12
#define CSD_TRANS_SPEED 0XFF // 8
// BYTE 11
#define CSD_CCC_H 0XFF // 8 12
// BYTE 10
#define CSD_CCC_L 0XF0    // 4
#define CSD_RDBL_LEN 0X0F // 4
// BYTE 9
#define CSD_RD_BLK_PARTIAL 0X08  // 1
#define CSD_WR_BLK_MISALIGN 0X04 // 1
#define CSD_RD_BLK_MISALIGN 0X02 // 1
#define CSD_DSR_IMP 0X01         // 1
#define CSD_RESERV2_H 0XF0       // 4
// BYTE 8
#define CSD_RESERV2_L 0XC0   // 2
#define CSD_CARD_SIZE_H 0X3F // 6  22
// BYTE 7
#define CSD_CARD_SIZE_M 0XFF // 8
// BYTE 6
#define CSD_CARD_SIZE_L 0XFF // 8
// BYTE 5
#define CSD_RESERVE3 0X80      // 1
#define CSD_ERASE_BLK_EN 0X40  // 1
#define CSD_SECTOR_SIZE_H 0X3E // 6
// BYTE 4
#define CSD_SECTOR_SIZE_L 0X80 // 1
#define CSD_WP_GRP_SIZE 0X7F   // 7
// BYTE 3
#define CSD_WP_GRP_EN 0X80    // 1
#define CSD_RESERVE4 0X60     // 2
#define CSD_R2W_FACTOR 0X1C   // 3
#define CSD_WR_BLK_LEN_H 0X03 // 2
// BYTE 2
#define CSD_WR_BLK_LEN_L 0XC0   // 2
#define CSD_WR_BLK_PARTIAL 0X40 // 1
#define CSD_RESERVE5 0X3E       // 5
// BYTE 1
#define CSD_FILE_FORMAT_GRP 0X80 // 1
#define CSD_COPY 0X40            // 1
#define CSD_PERM_WR_PROT 0X20    // 1
#define CSD_TMP_WR_PROT 0X10     // 1
#define CSD_FILE_FORMAT 0X0C     // 2
#define CSD_RESERVE6 0X03        // 2
// BYTE 0
#define CSD_CRC 0X7F    // 6
#define CSD_ALWAYS 0X01 // 1



void getByteValue(uint8_t val[]) // buffx 128
{
    uint8_t minibuff[16];
    uint8_t count = 0;
    uint8_t counter = 128;
    uint8_t cnt = 0x80;
    // uint8_t cc = 0x80;
    for (uint8_t i = 0; i < 16; i++)
    {
        minibuff[i] = val[count];
        for (uint8_t j = 8; j > 0; j--)
        {
            if ((minibuff[i] & cnt) >= 1)
            {
                buffx[counter] = 1;
            }
            if ((minibuff[i] & cnt) <= 0)
            {
                buffx[counter] = 0;
            }
            // eusart_send(buffx[counter]);
            cnt = cnt >> 1;
            counter--;
        }
        count++;
        cnt = 0x80;
        // cc = 0x80;
    }
}
// //
typedef struct
{
    uint8_t csd_struct : 2;  // 2
    uint8_t csd_reserv1 : 6; // 6
    //
    uint8_t csd_taac : 8; // 8
    //
    uint8_t csd_nsac : 8; // 8
    //
    uint8_t csd_transpeed : 8; // 8
    //
    uint8_t csd_ccc_h; // 8
    //
    uint8_t csd_ccc_l : 4;    // 4
    uint8_t csd_rdbl_len : 4; // 4
    //
    uint8_t csd_rdbl_partial : 1;  // 1
    uint8_t csd_wrbl_misalign : 1; // 1
    uint8_t csd_rdbl_misalign : 1; // 1
    uint8_t csd_dsr_imp : 1;       // 1
    uint8_t csd_reserv2_h : 4;     // 4
    //
    uint8_t csd_reserv2_l : 2;   // 2
    uint8_t csd_card_size_h : 6; // 6
    //
    uint8_t csd_card_size_m; // 8
    //
    uint8_t csd_card_size_l; // 8
    //
    uint8_t csd_reserve3 : 1;      // 1
    uint8_t csd_erase_bl_en : 1;   // 1
    uint8_t csd_sector_size_h : 6; // 6 erasable sector
    //
    uint8_t csd_sector_size_l : 1; // 1 erasable sector
    uint8_t csd_wd_grp_size : 7;   // 7
    //
    uint8_t csd_wd_grp_en : 1;    // 1
    uint8_t csd_reserve4 : 2;     // 2
    uint8_t csd_r2w_factor : 3;   // 3
    uint8_t csd_wr_blk_len_h : 2; // 2
    //
    uint8_t csd_wr_blk_len_l : 2;  // 2
    uint8_t csd_wr_bl_partial : 1; // 1
    uint8_t csd_reserve5 : 5;      // 5
    //
    uint8_t csd_file_format_grp : 1; // 1
    uint8_t csd_copy : 1;            // 1
    uint8_t csd_perm_wr_prot : 1;    // 1
    uint8_t csd_temp_wr_prot : 1;    // 1
    uint8_t csd_file_format : 2;     // 2
    uint8_t csd_reserve6 : 2;        // 2
    //
    uint8_t csd_crc : 7;    // 7
    uint8_t csd_always : 1; // 1

} csd_t_v2;
void getparserV2(uint8_t val[])
{
    getByteValue(val);
    csd_t_v2 csd;
    csd.csd_struct = buffx[127] + buffx[126];
    csd.csd_reserv1 = buffx[125] + buffx[124] + buffx[123] + buffx[122] + buffx[121] + buffx[120];
    csd.csd_taac = buffx[119] + buffx[118] + buffx[117] + buffx[116] + buffx[115] + buffx[114] + buffx[113] + buffx[112];
    csd.csd_nsac = buffx[111] + buffx[110] + buffx[109] + buffx[108] + buffx[107] + buffx[106] + buffx[105] + buffx[104];
    csd.csd_transpeed = buffx[103] + buffx[102] + buffx[101] + buffx[100] + buffx[99] + buffx[98] + buffx[97] + buffx[96];
    csd.csd_ccc_h = buffx[95] + buffx[94] + buffx[93] + buffx[92] + buffx[91] + buffx[90] + buffx[89] + buffx[88];
    csd.csd_ccc_l = buffx[87] + buffx[86] + buffx[85] + buffx[84];
    csd.csd_rdbl_len = buffx[83] + buffx[82] + buffx[81] + buffx[80];

    csd.csd_rdbl_partial = buffx[79];                                  // 1
    csd.csd_wrbl_misalign = buffx[78];                                 // 1
    csd.csd_rdbl_misalign = buffx[77];                                 // 1
    csd.csd_dsr_imp = buffx[76];                                       // 1
    csd.csd_reserv2_h = buffx[75] + buffx[74] + buffx[73] + buffx[72]; // 4
    //
    csd.csd_reserv2_l = buffx[71] + buffx[70];                                                   // 2
    csd.csd_card_size_h = buffx[69] + buffx[68] + buffx[67] + buffx[66] + buffx[65] + buffx[64]; // 6
    //
    csd.csd_card_size_m = buffx[63] + buffx[62] + buffx[61] + buffx[60] + buffx[59] + buffx[58] + buffx[57] + buffx[56]; // 8
    //
    csd.csd_card_size_l = buffx[55] + buffx[54] + buffx[53] + buffx[52] + buffx[51] + buffx[50] + buffx[49] + buffx[48]; // 8
    //
    csd.csd_reserve3 = buffx[47];                                                                  // 1
    csd.csd_erase_bl_en = buffx[46];                                                               // 1
    csd.csd_sector_size_h = buffx[45] + buffx[44] + buffx[43] + buffx[42] + buffx[41] + buffx[40]; // 6
    //
    csd.csd_sector_size_l = buffx[39];                                                                       // 1
    csd.csd_wd_grp_size = buffx[38] + buffx[37] + buffx[36] + buffx[35] + buffx[34] + buffx[33] + buffx[32]; // 7
    //
    csd.csd_wd_grp_en = buffx[31];                          // 1
    csd.csd_reserve4 = buffx[30] + buffx[29];               // 2
    csd.csd_r2w_factor = buffx[28] + buffx[27] + buffx[26]; // 3
    csd.csd_wr_blk_len_h = buffx[25] + buffx[24];           // 2
    //
    csd.csd_wr_blk_len_l = buffx[23] + buffx[22];                                 // 2
    csd.csd_wr_bl_partial = buffx[21];                                            // 1
    csd.csd_reserve5 = buffx[20] + buffx[19] + buffx[18] + buffx[17] + buffx[16]; // 5    //
    csd.csd_file_format_grp = buffx[15];                                          // 1
    csd.csd_copy = buffx[14];                                                     // 1
    csd.csd_perm_wr_prot = buffx[13];                                             // 1
    csd.csd_temp_wr_prot = buffx[12];                                             // 1
    csd.csd_file_format = buffx[11] + buffx[10];                                  // 2
    csd.csd_reserve6 = buffx[9] + buffx[8];                                       // 2
    //
    csd.csd_crc = buffx[7] + buffx[6] + buffx[5] + buffx[4] + buffx[3] + buffx[2] + buffx[1]; // 7
    csd.csd_always = buffx[0];                                                                // 1
}

typedef struct
{
    uint8_t csd_struct : 2;  // 2
    uint8_t csd_reserv1 : 6; // 6
    //
    uint8_t csd_taac;
    //
    uint8_t csd_nsac;
    //
    uint8_t csd_transpeed;
    //
    uint8_t csd_ccc_h;
    //
    uint8_t csd_ccc_l : 4;    // 4
    uint8_t csd_rdbl_len : 4; // 4
    //
    uint8_t csd_rdbl_partial : 1;  // 1
    uint8_t csd_wrbl_misalign : 1; // 1
    uint8_t csd_rdbl_misalign : 1; // 1
    uint8_t csd_dsr_imp : 1;       // 1
    uint8_t csd_reserv2 : 2;       // 2
    uint8_t csd_card_size_h : 2;   // 2 12
    //
    uint8_t csd_card_size_m; // 8
    //
    uint8_t csd_card_size_l : 2;    // 2
    uint8_t csd_vdd_r_curr_min : 3; // 3
    uint8_t csd_vdd_r_curr_max : 3; // 3
    //
    uint8_t csd_vdd_w_curr_min : 3; // 3
    uint8_t csd_vdd_w_curr_max : 3; // 3
    uint8_t csd_c_size_mult_h : 2;  // 2 3
    //
    uint8_t csd_c_size_mult_l : 1; // 1
    uint8_t csd_er_blk_en : 1;     // 1
    uint8_t csd_sector_size_h : 6; // 6 7
    //
    uint8_t csd_sector_size_l : 1; // 1
    uint8_t csd_wp_grp_size : 7;   // 7
    //
    uint8_t csd_wp_grp_en : 1;    // 1
    uint8_t csd_reserv3 : 2;      // 2
    uint8_t csd_r2w_factor : 3;   // 3
    uint8_t csd_wr_blk_len_h : 2; // 2 4
    //
    uint8_t csd_wr_blk_len_l : 2;   // 2
    uint8_t csd_wr_blk_partial : 1; // 1
    uint8_t csd_reserv4 : 5;        // 5
    //
    uint8_t csd_file_format_grp : 1; // 1 r/w
    uint8_t csd_copy : 1;            // 1 r/w
    uint8_t csd_perm_wr_prot : 1;    // 1 r/w
    uint8_t csd_temp_wr_prot : 1;    // 1 r/w
    uint8_t csd_file_format : 2;     // 2 r/w
    uint8_t csd_reserv5 : 2;         // 2
    //
    uint8_t csd_crc : 7;    // 7
    uint8_t csd_always : 1; // 1

} csd_t_v1;
void getparserV1(uint8_t val[])
{
    getByteValue(val);
    csd_t_v1 csd;

    csd.csd_struct = buffx[127] + buffx[126];
    csd.csd_reserv1 = buffx[125] + buffx[124] + buffx[123] + buffx[122] + buffx[121] + buffx[120];
    //
    csd.csd_taac = buffx[119] + buffx[118] + buffx[117] + buffx[116] + buffx[115] + buffx[114] + buffx[113] + buffx[112];
    //
    csd.csd_nsac = buffx[111] + buffx[110] + buffx[109] + buffx[108] + buffx[107] + buffx[106] + buffx[105] + buffx[104];
    //
    csd.csd_transpeed = buffx[103] + buffx[102] + buffx[101] + buffx[100] + buffx[99] + buffx[98] + buffx[97] + buffx[96];
    //
    csd.csd_ccc_h = buffx[95] + buffx[94] + buffx[93] + buffx[92] + buffx[91] + buffx[90] + buffx[89] + buffx[88];
    //
    csd.csd_ccc_l = buffx[87] + buffx[86] + buffx[85] + buffx[84];    // 4
    csd.csd_rdbl_len = buffx[83] + buffx[82] + buffx[81] + buffx[80]; // 4
    //
    csd.csd_rdbl_partial = buffx[79];            // 1
    csd.csd_wrbl_misalign = buffx[78];           // 1
    csd.csd_rdbl_misalign = buffx[77];           // 1
    csd.csd_dsr_imp = buffx[76];                 // 1
    csd.csd_reserv2 = buffx[75] + buffx[74];     // 2
    csd.csd_card_size_h = buffx[73] + buffx[72]; // 2 12
    //
    csd.csd_card_size_m = buffx[71] + buffx[70] + buffx[69] + buffx[68] + buffx[67] + buffx[66] + buffx[65] + buffx[64]; // 8
    //
    csd.csd_card_size_l = buffx[63] + buffx[62];                // 2
    csd.csd_vdd_r_curr_min = buffx[61] + buffx[60] + buffx[59]; // 3
    csd.csd_vdd_r_curr_max = buffx[58] + buffx[57] + buffx[56]; // 3
    //
    csd.csd_vdd_w_curr_min = buffx[55] + buffx[54] + buffx[53]; // 3
    csd.csd_vdd_w_curr_max = buffx[52] + buffx[51] + buffx[50]; // 3
    csd.csd_c_size_mult_h = buffx[49] + buffx[48];              // 2 3
    //
    csd.csd_c_size_mult_l = buffx[47];                                                             // 1
    csd.csd_er_blk_en = buffx[46];                                                                 // 1
    csd.csd_sector_size_h = buffx[45] + buffx[44] + buffx[43] + buffx[42] + buffx[41] + buffx[40]; // 6 7
    //
    csd.csd_sector_size_l = buffx[39];                                                                       // 1
    csd.csd_wp_grp_size = buffx[38] + buffx[37] + buffx[36] + buffx[35] + buffx[34] + buffx[33] + buffx[32]; // 7
    //
    csd.csd_wp_grp_en = buffx[31];                          // 1
    csd.csd_reserv3 = buffx[30] + buffx[29];                // 2
    csd.csd_r2w_factor = buffx[28] + buffx[27] + buffx[26]; // 3
    csd.csd_wr_blk_len_h = buffx[25] + buffx[24];           // 2 4
    //
    csd.csd_wr_blk_len_l = buffx[23] + buffx[22];                                // 2
    csd.csd_wr_blk_partial = buffx[21];                                          // 1
    csd.csd_reserv4 = buffx[20] + buffx[19] + buffx[18] + buffx[17] + buffx[16]; // 5
    //
    csd.csd_file_format_grp = buffx[15];         // 1 r/w
    csd.csd_copy = buffx[14];                    // 1 r/w
    csd.csd_perm_wr_prot = buffx[13];            // 1 r/w
    csd.csd_temp_wr_prot = buffx[12];            // 1 r/w
    csd.csd_file_format = buffx[11] + buffx[10]; // 2 r/w
    csd.csd_reserv5 = buffx[9] + buffx[8];       // 2
    //
    csd.csd_crc = buffx[7] + buffx[6] + buffx[5] + buffx[4] + buffx[3] + buffx[2] + buffx[1]; // 7
    csd.csd_always = buffx[0];                                                                // 1
}
// //
typedef struct{

}csv_t;

// void getCsdValueV2(uint8_t val[])
// {
//     // csd_t_v2 csv;
//     csv.csd_struct = val[15];
//     csv.csd_reserv1 = val[15];
//     //
//     csv.csd_taac = val[14];
//     //
//     csv.csd_nsac = val[13];
//     //
//     csv.csd_transpeed = val[12];
//     //
//     csv.csd_ccc_h = val[11];
//     //
//     csv.csd_ccc_l = val[10];
//     csv.csd_rdbl_len = val[10];
//     //
//     csv.csd_rdbl_partial = val[9];
//     csv.csd_wrbl_misalign = val[9];
//     csv.csd_rdbl_misalign = val[9];
//     csv.csd_dsr_imp = val[9];
//     csv.csd_reserv2_h = val[9];
//     //
//     csv.csd_reserv2_l = val[8];
//     csv.csd_card_size_h = val[8];
//     // eusart_send(csv.csd_card_size_h);
//     //
//     csv.csd_card_size_m = val[7];
//     // eusart_send(csv.csd_card_size_m);
//     //
//     csv.csd_card_size_l = val[6];
//     // eusart_send(csv.csd_card_size_l);
//     //
//     csv.csd_reserve3 = val[5];
//     csv.csd_erase_bl_en = val[5];
//     csv.csd_sector_size_h = val[5];
//     //
//     csv.csd_sector_size_l = val[4];
//     csv.csd_wd_grp_size = val[4];
//     //
//     csv.csd_wd_grp_en = val[3];
//     csv.csd_reserve4 = val[3];
//     csv.csd_r2w_factor = val[3];
//     csv.csd_wr_blk_len_h = val[3];
//     //
//     csv.csd_wr_blk_len_l = val[2];
//     csv.csd_wr_bl_partial = val[2];
//     csv.csd_reserve5 = val[2];
//     //
//     csv.csd_file_format_grp = val[1];
//     csv.csd_copy = val[1];
//     csv.csd_perm_wr_prot = val[1];
//     csv.csd_temp_wr_prot = val[1];
//     csv.csd_file_format = val[1];
//     csv.csd_reserve6 = val[1];
//     //
//     csv.csd_crc = val[0];
//     csv.csd_always = val[0];
// }
// void getCsdValueV1(uint8_t val[])
// {
//     // csd_t_v1 csv;
//     csv.csd_struct = val[15];
//     csv.csd_reserv1 = val[15];
//     //
//     csv.csd_taac = val[14];
//     //
//     csv.csd_nsac = val[13];
//     //
//     csv.csd_transpeed = val[12];
//     //
//     csv.csd_ccc_h = val[11];
//     //
//     csv.csd_ccc_l = val[10];    // 4
//     csv.csd_rdbl_len = val[10]; // 4
//     //
//     csv.csd_rdbl_partial = val[9];  // 1
//     csv.csd_wrbl_misalign = val[9]; // 1
//     csv.csd_rdbl_misalign = val[9]; // 1
//     csv.csd_dsr_imp = val[9];       // 1
//     csv.csd_reserv2 = val[9];       // 2
//     csv.csd_card_size_h = val[9];   // 2 12
//     //
//     csv.csd_card_size_m = val[8]; // 8
//     //
//     csv.csd_card_size_l = val[7];    // 2
//     csv.csd_vdd_r_curr_min = val[7]; // 3
//     csv.csd_vdd_r_curr_max = val[7]; // 3
//     //
//     csv.csd_vdd_w_curr_min = val[6]; // 3
//     csv.csd_vdd_w_curr_max = val[6]; // 3
//     csv.csd_c_size_mult_h = val[6];  // 2 3
//     //
//     csv.csd_c_size_mult_l = val[5]; // 1
//     csv.csd_er_blk_en = val[5];     // 1
//     csv.csd_sector_size_h = val[5]; // 6 7
//     //
//     csv.csd_sector_size_l = val[4]; // 1
//     csv.csd_wp_grp_size = val[4];   // 7
//     //
//     csv.csd_wp_grp_en = val[3];    // 1
//     csv.csd_reserv3 = val[3];      // 2
//     csv.csd_r2w_factor = val[3];   // 3
//     csv.csd_wr_blk_len_h = val[3]; // 2 4
//     //
//     csv.csd_wr_blk_len_l = val[2];   // 2
//     csv.csd_wr_blk_partial = val[2]; // 1
//     csv.csd_reserv4 = val[2];        // 5
//     //
//     csv.csd_file_format_grp = val[1]; // 1 r/w
//     csv.csd_copy = val[1];            // 1 r/w
//     csv.csd_perm_wr_prot = val[1];    // 1 r/w
//     csv.csd_temp_wr_prot = val[1];    // 1 r/w
//     csv.csd_file_format = val[1];     // 2 r/w
//     csv.csd_reserv5 = val[1];         // 2
//     //
//     csv.csd_crc = val[0];    // 7
//     csv.csd_always = val[0]; // 1
// }

#endif // _SD_REGS
