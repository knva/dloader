/*
 * @Author: sinpo828
 * @Date: 2021-02-10 08:46:33
 * @LastEditors: sinpo828
 * @LastEditTime: 2021-02-10 09:04:30
 * @Description: file content
 */
#ifndef __FDL__
#define __FDL__
// old protocol to update FDL1
/**
 * -> ae 0c 00 00 00   ff 00 00
 * -> 00 00 00 00 00   00 00 00 00 00 00 00
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00
 * 
 * -> ae 11 00 00 00   ff 00 00
 * -> 04 00 00 00 00   80 83 00 40 33 00 00
 * -> 50 44 4c 31 00  // PDL1
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00
 * 
 * 
 * -> ae 0c 08 00 00   ff 00 00
 * -> 05 00 00 00 00   00 00 00 00 08 00 00
 * -> data  2048 (0x800)
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00 
 * 
 * -> ae 0c 08 00 00   ff 00 00
 * -> 05 00 00 00 01   00 00 00 00 08 00 00
 * -> data  2048
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00
 * 
 *             ................
 * -> ae 0c 08 00 00   ff 00 00
 * -> 05 00 00 00 02   00 00 00 00 08 00 00  HOSTFDL_DATA
 * -> data  2048
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00
 * 
 * -> ae 4c 03 00 00   ff 00 00
 * -> 05 00 00 00 06   00 00 00 40 03 00 00
 * -> data  832 (0x340)
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00
 * 
 * -> ae 10 00 00 00   ff 00 00
 * -> 06 00 00 00 00   00 00 00 00 00 00 00
 * -> 1c 3c 6e 06
 * <- ae 04 00 00 00   ff 00 00 00 00 00 00
 * -> ae 0c 00 00 00   ff 00 00 00 00 00 00
 * -> 07 00 00 00 06   00 00 00 00 00 00 00
 * 
 */



#endif //__FDL__