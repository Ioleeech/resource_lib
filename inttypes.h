#ifndef __INTTYPES_H__
#define __INTTYPES_H__

typedef                   void  void_t;

typedef                    int  sint_t;
typedef unsigned           int  uint_t;
typedef                   char  char_t;

typedef                   char  sint8_t;
typedef unsigned          char  uint8_t;
typedef              short int  sint16_t;
typedef unsigned     short int  uint16_t;
typedef                    int  sint32_t;
typedef unsigned           int  uint32_t;
typedef          long long int  sint64_t;
typedef unsigned long long int  uint64_t;

typedef enum _bool_e {
    FALSE = 0,
    TRUE  = 1
} bool_e;

#endif // __INTTYPES_H__
