/*
 * libhugetlbfs - Easy use of Linux hugepages
 * Copyright (C) 2005-2006 David Gibson & Adam Litke, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef _LIBHUGETLBFS_INTERNAL_H
#define _LIBHUGETLBFS_INTERNAL_H

#include <elf.h>
#include <link.h>
#include <limits.h>

#ifndef __LIBHUGETLBFS__
#error This header should not be included by library users.
#endif /* __LIBHUGETLBFS__ */

#include "libhugetlbfs_privutils.h"

#define stringify_1(x)	#x
#define stringify(x)	stringify_1(x)

#define ALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_UP(x,a)	(((x) + (a)) & ~((a) - 1))
#define ALIGN_DOWN(x,a) ((x) & ~((a) - 1))

#if defined(__powerpc64__) || defined (__powerpc__)
#define SLICE_LOW_SHIFT		28
#define SLICE_HIGH_SHIFT	40
#elif defined(__ia64__)
#define SLICE_HIGH_SHIFT	63
#endif

/*
 * When adding a library local variable externalise the symbol as
 * normal, plus add a #define of the form below.  This define effectively
 * renames the routine into the local namespace __lh_* which is forced
 * local in the linker script version.lds.  Some routines may need to be
 * exported in the utilities library these are marked __pu_* which marks
 * them for export in libhugetlbfs_privutils; their definitions should
 * appear in libhugetlbfs_privutils.h rather than here.
 */
extern int __hugetlbfs_verbose;
extern int __hugetlbfs_debug;
extern int __hugetlbfs_prefault;
#define hugetlbfs_setup_elflink __lh_hugetlbfs_setup_elflink
extern void hugetlbfs_setup_elflink();
#define hugetlbfs_setup_morecore __lh_hugetlbfs_setup_morecore
extern void hugetlbfs_setup_morecore();
#define hugetlbfs_setup_debug __lh_hugetlbfs_setup_debug
extern void hugetlbfs_setup_debug();
#define setup_mounts __lh_setup_mounts
extern void setup_mounts();
#define setup_features __lh_setup_features
extern void setup_features();
extern char __hugetlbfs_hostname[];
#define hugetlbfs_prefault __lh_hugetlbfs_prefault
extern int hugetlbfs_prefault(int fd, void *addr, size_t length);
#define parse_page_size __lh_parse_page_size
extern long parse_page_size(const char *str);

#ifndef REPORT_UTIL
#define REPORT_UTIL "libhugetlbfs"
#endif
#ifndef REPORT
#define REPORT(level, prefix, format, ...) \
	do { \
		if (__hugetlbfs_debug || __hugetlbfs_verbose >= level) { \
			fprintf(stderr, REPORT_UTIL " [%s:%d]: " prefix ": " \
				format, __hugetlbfs_hostname, getpid(), \
				##__VA_ARGS__); \
			fflush(stderr); \
		} \
	} while (0)

#define REPORT_CONT(level, prefix, ...) \
	do { \
		if (__hugetlbfs_debug || __hugetlbfs_verbose >= level) { \
			fprintf(stderr, ##__VA_ARGS__); \
			fflush(stderr); \
		} \
	} while (0)
#endif

#include "libhugetlbfs_debug.h"

#if defined(__powerpc64__) && !defined(__LP64__)
/* Older binutils fail to provide this symbol */
#define __LP64__
#endif

/* Multiple huge page size support */
struct hpage_size {
	unsigned long pagesize;
	char mount[PATH_MAX+1];
};

struct hpage_pool {
	unsigned long pagesize;
	unsigned long minimum;
	unsigned long maximum;
	unsigned long size;
	int is_default;
};

#define size_to_smaller_unit __lh_size_to_smaller_unit
extern long size_to_smaller_unit(long size);

#define file_read_ulong __lh_file_read_ulong
extern long file_read_ulong(char *file, const char *tag);
#define file_write_ulong __lh_file_write_ulong
extern int file_write_ulong(char *file, unsigned long val);

#define hpool_sizes __lh_hpool_sizes
extern int hpool_sizes(struct hpage_pool *, int);
#define get_pool_size __lh_get_pool_size
extern int get_pool_size(long, struct hpage_pool *);

/* Arch-specific callbacks */
extern int direct_syscall(int sysnum, ...);
extern ElfW(Word) plt_extrasz(ElfW(Dyn) *dyntab);

#define MEMINFO "/proc/meminfo"
#define PROC_HUGEPAGES_DIR "/proc/sys/vm/"
#define SYSFS_HUGEPAGES_DIR "/sys/kernel/mm/hugepages/"

#endif /* _LIBHUGETLBFS_INTERNAL_H */
