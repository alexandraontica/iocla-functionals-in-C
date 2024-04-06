#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

array_t reverse(array_t list) {
	(void)list;
	return (array_t){0};
}

void get_number(void *new_nr, void **parts) {
	number_t *number = (number_t *)new_nr;

	int integ = *(int *)parts[0];
	number->integer_part = integ;

	int fr = *(int *)parts[1];
	number->fractional_part = fr;

	number->string = malloc(20);
	sprintf(number->string, "%d.%d", integ, fr);
}

void number_destructor(void *number) {
	number_t *number_struct = (number_t *)number;
	free(number_struct->string);
}

array_t create_number_array(array_t integer_part, array_t fractional_part) {
	(void)integer_part;
	(void)fractional_part;

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
	student_t *stud = (student_t *)stud_void;
	char **name = (char **)name_void;

	*name = malloc(20);
	char *name_str = *name;

	sprintf(name_str, "%s", stud->name);
}

void student_destructor(void *elem)
{
	free(((student_t *)elem)->name);
}

array_t get_passing_students_names(array_t list) {
	array_t passing_students = filter(check_grade, list);

	int len = 0;
	reduce(get_names_length, &len, passing_students);

	array_t names = map(get_names, len + 1, student_destructor,
						passing_students);

	return names;
	return passing_students;
}

array_t check_bigger_sum(array_t list_list, array_t int_list) {
	(void)list_list;
	(void)int_list;
	return (array_t){0};
}

array_t get_even_indexed_strings(array_t list) {
	(void)list;
	return (array_t){0};
}

array_t generate_square_matrix(int n) {
	(void)n;
	return (array_t){0};
}
