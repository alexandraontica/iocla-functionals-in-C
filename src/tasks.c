#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_first(void *acc_void, void *elem_void) {
	array_t *acc = (array_t *)acc_void;

	// adaug elementul curent la inceputul listei
	// deja existente in acumulator
	void *aux = calloc(acc->len - 1, acc->elem_size);
	memcpy(aux, acc->data, acc->elem_size * (acc->len - 1));
	memcpy(acc->data, elem_void, acc->elem_size);
	memcpy((int *)acc->data + 1, aux, acc->elem_size * (acc->len - 1));

	free(aux);
}

array_t reverse(array_t list) {
	// transmit ca acumulator pentru functia reduce
	// un string in care construiesc lista inversata
	array_t rev;
	rev.data = calloc(list.len, list.elem_size);
	rev.destructor = list.destructor;
	rev.elem_size = list.elem_size;
	rev.len = list.len;

	reduce(add_first, &rev, list);

	return rev;
}

void get_number(void *new_nr, void **parts) {
	number_t *number = (number_t *)new_nr;

	int integ = *(int *)parts[0];
	number->integer_part = integ;

	int fr = *(int *)parts[1];
	number->fractional_part = fr;

	number->string = malloc(20);  // presupun ca nu am numere cu mai mult de
								  // 20 cifre
	sprintf(number->string, "%d.%d", integ, fr);
}

void number_destructor(void *number) {
	number_t *number_struct = (number_t *)number;
	free(number_struct->string);
}

array_t create_number_array(array_t integer_part, array_t fractional_part) {
	// ma folosesc de multiple ca sa pot parcurge ambele liste,
	// cea cu partile intregi si cea cu partile fractionare,
	// in acelasi timp
	array_t number = map_multiple(get_number, sizeof(number_t),
								  number_destructor, 2, integer_part,
								  fractional_part);

	return number;
}

boolean check_grade(void *grade) {
	if (*(int *)grade >= 5.0)
		return 1;
	else
		return 0;
}

void get_names_length(void *len_void, void *stud_void) {
	student_t *stud = (student_t *)stud_void;
	int *len = (int *)len_void;

	if ((int)strlen(stud->name) > *len)
		*len = strlen(stud->name);
}

void get_names(void *name_void, void *stud_void) {
	// pastrez doar numele din structura
	student_t *stud = (student_t *)stud_void;
	char **name = (char **)name_void;

	*name = malloc(20);  // presupun ca nu am nume cu mai mult de
						 // 20 de caractere
	char *name_str = *name;

	sprintf(name_str, "%s", stud->name);
}

void student_destructor(void *elem)
{
	free(((student_t *)elem)->name);
}

array_t get_passing_students_names(array_t list) {
	// pastrez doar studentii care trec
	array_t passing_students = filter(check_grade, list);

	// calculez lungimea maxima a unui nume
	// deoarece imi trebuie ca argument pentru map
	// dimensiunea (size) unui element
	int len = 0;
	reduce(get_names_length, &len, passing_students);

	// creez variabila de tip array_t doar cu numele studentilor
	array_t names = map(get_names, len + 1, student_destructor,
						passing_students);

	return names;
}

void sum(void *acc_void, void *elem_void) {
	int *acc = (int *)acc_void;
	int *elem = (int *)elem_void;

	*acc += *elem;
}

void check_greater(void *new_void, void **elem_lists) {
	boolean *new = (boolean *)new_void;
	array_t list = *(array_t *)elem_lists[0];
	int integ = *(int *)elem_lists[1];

	// calculez suma elementelor din lista curenta
	int sum_list = 0;
	reduce(sum, &sum_list, list);

	if (sum_list >= integ)
		*new = 1;
	else
		*new = 0;
}

array_t check_bigger_sum(array_t list_list, array_t int_list) {
	// folosesc map_multiple ca sa pot parcurge in acelasi timp
	// cele doua liste
	array_t bool_list = map_multiple(check_greater, sizeof(boolean),
									 NULL, 2, list_list, int_list);
	return bool_list;
}

void create_int_list(void *acc_void, void *elem_void) {
	// o voi folosi in reduce ca sa creez o lista de int-uri consecutive
	int *elem = (int *)elem_void;
	int *acc = (int *)acc_void;
	int x = ++(*acc);
	*elem = x;
}

void add_even(void *new_void, void **arg) {
	char **new = (char **)new_void;
	char *str = (char *)arg[0];
	int index = *(int *)arg[1];

	if (index % 2 == 0)
		*new = strdup(*(char **)str);
	else
		*new = NULL;
}

boolean check_str(void *elem_void) {
	char **elem = (char **)elem_void;

	if (*elem)
		return 1;
	else
		return 0;
}

array_t get_even_indexed_strings(array_t list) {
	array_t int_list;
	int_list.data = malloc(list.len * sizeof(int));
	int_list.destructor = NULL;
	int_list.elem_size = sizeof(int);
	int_list.len = list.len;

	int acc = -1;
	reduce(create_int_list, &acc, int_list);

	array_t new_list = map_multiple(add_even, list.elem_size, list.destructor,
									2, list, int_list);

	array_t final_list = filter(check_str, new_list);
	return final_list;
}

void alloc_elem(void *n_void, void *elem_void) {
	int *n = (int *)n_void;

	array_t int_list;
	int_list.data = calloc(*n, sizeof(int));
	int_list.destructor = NULL;
	int_list.elem_size = sizeof(int);
	int_list.len = *n;

	int acc = 0;
	reduce(create_int_list, &acc, int_list);

	*(array_t *)elem_void = int_list;
}

void add(void *acc_void, void *integ_void) {
	int *acc = (int *)acc_void;
	int *ineg = (int *)integ_void;

	*ineg = *ineg + *acc;
}

void atrib(void **arg) {
	int *new = (int *)arg[0];
	int *old = (int *)arg[1];

	*new = *old;
}

void change_matrix_elem(void *new_void, void **arg) {
	array_t *matrix_elem = (array_t *)new_void;
	array_t *array = (array_t *)arg[0];
	int to_add = *(int *)arg[1];

	reduce(add, &to_add, *array);

	array_t int_list;
	int_list.data = calloc(array->len, sizeof(int));
	int_list.destructor = NULL;
	int_list.elem_size = sizeof(int);
	int_list.len = array->len;
	for_each_multiple(atrib, 2, int_list, *array);

	*matrix_elem = int_list;
}

void simple_list_destructor(void *elem)
{
	free(((array_t *)elem)->data);
}

array_t generate_square_matrix(int n) {
	array_t list_list;
	list_list.data = calloc(n, sizeof(array_t));
	list_list.destructor = simple_list_destructor;
	list_list.elem_size = sizeof(array_t);
	list_list.len = n;

	int acc = n;
	reduce(alloc_elem, &acc, list_list);

	array_t int_list;
	int_list.data = malloc(n * sizeof(int));
	int_list.destructor = NULL;
	int_list.elem_size = sizeof(int);
	int_list.len = n;

	acc = -1;
	reduce(create_int_list, &acc, int_list);

	array_t new_list = map_multiple(change_matrix_elem, sizeof(array_t),
									simple_list_destructor, 2, list_list,
									int_list);

	return new_list;
}
