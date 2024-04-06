#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_first(void *acc_void, void *elem_void) {
	array_t *acc = (array_t *)acc_void;

	// adaug elementul curent la inceputul liste
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

array_t get_even_indexed_strings(array_t list) {
	list.elem_size *= 2;  // iau cate 2 elemente odata
	list.len++;  // modific lungimea listei pentru noile elemente
	list.len /= 2;
	// Avand string-uri, acestea se termina in '\0'.
	// Cand "lipesc" doua zone de memorie apartinand
	// a doua string-uri, tot ce va fi dupa primul
	// \0 va fi ignorat, deci sirurile de pe pozitii
	// impare nu mai sunt luate in considerare.

	return list;
}

array_t generate_square_matrix(int n) {
	(void)n;
	return (array_t){0};
}
