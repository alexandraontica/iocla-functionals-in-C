#include "functional.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void for_each(void (*func)(void *), array_t list)
{
	char *elem = (char *)list.data;

	for (int i = 0; i < list.len; i++) {
		func(elem);
		elem += list.elem_size;
	}
}

array_t map(void (*func)(void *, void *),
			int new_list_elem_size,
			void (*new_list_destructor)(void *),
			array_t list)
{
	array_t new_list;
	new_list.data = calloc(list.len, new_list_elem_size);
	new_list.destructor = new_list_destructor;
	new_list.elem_size = new_list_elem_size;
	new_list.len = list.len;

	char *old = (char *)list.data;
	char *new = (char *)new_list.data;

	for (int i = 0; i < list.len; i++) {
		func(new, old);

		old += list.elem_size;
		new += new_list_elem_size;
	}

	if (list.destructor) {
		old = (char *)list.data;

		for (int i = 0; i < list.len; i++) {
			list.destructor(old);
			old += list.elem_size;
		}
	}

	free(list.data);

	return new_list;
}

array_t filter(boolean(*func)(void *), array_t list)
{
	array_t new_list;
	new_list.data = calloc(list.len, list.elem_size);
	// verificare alocare
	new_list.destructor = list.destructor;
	new_list.elem_size = list.elem_size;
	new_list.len = 0;

	char *old = (char *)list.data;

	for (int i = 0; i < list.len; i++) {
		if (func(old)) {
			memcpy((char *)new_list.data + new_list.len * new_list.elem_size,
				   old, list.elem_size);
			new_list.len++;
		}

		old += list.elem_size;
	}

	// free ca lumea???
	free(list.data);

	return new_list;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list)
{
	char *elem = (char *)list.data;

	for (int i = 0; i < list.len; i++) {
		func(acc, elem);
		elem += list.elem_size;
	}

	return acc;
}

void for_each_multiple(void(*func)(void **), int varg_c, ...)
{
	va_list arg_list;

	va_start(arg_list, varg_c);

	int lmin = BIG_ENOUGH;
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(arg_list, array_t);
		if	(list.len < lmin)
			lmin = list.len;
	}

	va_end(arg_list);

	for (int i = 0; i < lmin; i++) {
		va_start(arg_list, varg_c);

		void *elem_poz_i[varg_c];

		for (int j = 0; j < varg_c; j++) {
			array_t list = va_arg(arg_list, array_t);

			elem_poz_i[j] = (char *)list.data;
			elem_poz_i[j] += i * list.elem_size;
		}

		func(elem_poz_i);

		va_end(arg_list);
	}
}

array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...)
{
	va_list arg_list;

	va_start(arg_list, varg_c);

	int lmin = BIG_ENOUGH;
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(arg_list, array_t);
		if (list.len < lmin)
			lmin = list.len;
	}

	va_end(arg_list);

	array_t new_list;
	new_list.data = malloc(new_list_elem_size * lmin);
	new_list.destructor = new_list_destructor;
	new_list.elem_size = new_list_elem_size;
	new_list.len = lmin;

	char *new = (char *)new_list.data;

	for (int i = 0; i < lmin; i++) {
		va_start(arg_list, varg_c);

		void *elem_poz_i[varg_c];

		for (int j = 0; j < varg_c; j++) {
			array_t list = va_arg(arg_list, array_t);

			elem_poz_i[j] = (char *)list.data;
			elem_poz_i[j] += i * list.elem_size;
		}

		func(new, elem_poz_i);
		new += new_list_elem_size;

		va_end(arg_list);
	}

	va_start(arg_list, varg_c);
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(arg_list, array_t);

		if (list.destructor) {
			for (int j = 0; j < list.len; j++) {
				void *elem = (char *)list.data + j * list.elem_size;
				list.destructor(elem);
			}
		}
		free(list.data);
	}
	va_end(arg_list);

	return new_list;
}

void *reduce_multiple(void(*func)(void *, void **), void *acc, int varg_c, ...)
{
	va_list arg_list;

	va_start(arg_list, varg_c);

	int lmin = BIG_ENOUGH;
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(arg_list, array_t);
		if (list.len < lmin)
			lmin = list.len;
	}

	va_end(arg_list);

	for (int i = 0; i < lmin; i++) {
		va_start(arg_list, varg_c);

		void *elem_poz_i[varg_c];

		for (int j = 0; j < varg_c; j++) {
			array_t list = va_arg(arg_list, array_t);

			elem_poz_i[j] = (char *)list.data;
			elem_poz_i[j] += i * list.elem_size;
		}

		func(acc, elem_poz_i);

		va_end(arg_list);
	}

	return acc;
}
