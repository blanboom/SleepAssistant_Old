/** interrput.h - 中断相关程序
 * 包含中断服务程序和中断初始化函数
 */

#ifndef INCLUDE_INTERRUPT_H_
#define INCLUDE_INTERRUPT_H_

#include "cortexm/ExceptionHandlers.h"

void interruptNVICInit(void);

#endif /* INCLUDE_INTERRUPT_H_ */
