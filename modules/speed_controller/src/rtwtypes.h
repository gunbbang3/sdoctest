/*
 * File: rtwtypes.h
 *
 * The standard fixed-width/real type header Simulink Coder/Embedded Coder
 * places next to every generated model. It is boilerplate, identical
 * across generated models, and (like the rest of this module's src/
 * folder) is not meant to be hand-edited -- see speed_controller.h and
 * ../models/speed_controller.md.
 */
#ifndef RTWTYPES_H_
#define RTWTYPES_H_

typedef double real_T;
typedef float real32_T;
typedef signed char int8_T;
typedef unsigned char uint8_T;
typedef short int16_T;
typedef unsigned short uint16_T;
typedef int int32_T;
typedef unsigned int uint32_T;
typedef unsigned char boolean_T;

#ifndef true
#define true (1U)
#endif

#ifndef false
#define false (0U)
#endif

#endif /* RTWTYPES_H_ */
