#ifndef FOREST_MSVC_DATAOBJECT_COMPAT_H
#define FOREST_MSVC_DATAOBJECT_COMPAT_H

#ifdef _MSC_VER
/*
 * dataobject sources contain raw GCC attributes, e.g.:
 *   __attribute__((aligned(32)))
 * MSVC does not parse this syntax; swallow it for this target.
 */
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

#endif
