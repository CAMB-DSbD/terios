/*
 * Copyright (C) 2023 Kian Cross
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#define BITS_SET(v, n, c) v |= (c) << (n)
#define BIT_SET(v, n) BITS_SET(v, n, 1l)
#define BIT_CLR(v, n) v &= ~(1 << n)

#endif
