/**
 * @file
 * @brief   Instruction Cache Controller function prototypes and data types.
 */

/* ****************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2016-10-10 19:01:16 -0500 (Mon, 10 Oct 2016) $
 * $Revision: 24662 $
 *
 *************************************************************************** */

/* Define to prevent redundant inclusion */
#ifndef _ICC_H_
#define _ICC_H_

/* **** Includes **** */
#include "icc_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Doxy group definition for this peripheral module */

/**
 * @ingroup sysconfig
 * @defgroup icc Instruction Cache Controller (ICC)
 * @{
 */
/**
 * @brief      Enable and flush the instruction cache controller.
 */
void ICC_Enable(void);

/**
 * @brief      Disable the instruction cache controller.
 */
void ICC_Disable(void);

/**
 * @brief      Flush the instruction cache controller.
 */
__STATIC_INLINE void ICC_Flush()
{
    ICC_Disable();
    ICC_Enable();
}
/**@} end of group icc */
#ifdef __cplusplus
}
#endif

#endif /* _ICC_H_ */
