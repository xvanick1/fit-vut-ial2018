#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

void list_init(ColorList *list) {
	list->active = NULL;
	list->first = NULL;
	list->last = NULL;
}

void insert_last(ColorList *list, int color) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	ColorElement *new_elem_ptr = calloc(1, sizeof(*new_elem_ptr));
	if(new_elem_ptr == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	new_elem_ptr->color = color; // priradim hodnotu
	// Pravy ukazatel miri na NULL, aby pri vyhledavani bylo jasne, ze je konec
	new_elem_ptr->right_ptr = NULL;
	// Novy prvek davam za posledni a ukazuji na nej levym ukazatelen
	new_elem_ptr->left_ptr = list->last;

	// Pokud uz seznam mel posledniho, tak posledni prvek bude ukazovat doprava 
	// na ten novy
	if(list->last != NULL) {
		list->last->right_ptr = new_elem_ptr;
	}
	// Pokud seznam nemel posledniho, tak novy prvek je zaroven i prvni
	else {
		list->first = new_elem_ptr;
	}

	// Nakonec oznacim novy prvek jako posledni.
	// Je nutne to udelat az naposled, protoze predtim pracuji s byvalym
	// poslednim prvkem
	list->last = new_elem_ptr;
}

void make_first_active(ColorList *list) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	list->active = list->first;
}

int copy_active(ColorList *list) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(list->active != NULL)
		return list->active->color;
		// *val = L->Act->data;
	else {
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}
}

int copy_first(ColorList *list) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	// Když je seznam prázdný, signalizuje se chyba.
	if (!list->first)
	{
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}

	return list->first->color;
}

void make_next_active(ColorList *list) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(list->active != NULL) 
		list->active = list->active->right_ptr;
}

void delete_first(ColorList *list) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	ColorElement *elem_ptr;
	// Pokud je seznam prazdny, nic se nedeje
	if(list->first != NULL) {
		// Nutno ulozit ukazatel na 1. prvek, protoze potom se meni
		elem_ptr = list->first;
		// Pokud je 1. prvek aktivni, rusi se aktivita
		if(list->active == list->first) {
			list->active = NULL;
		}
		// Pokud mel seznam jediny prvek, zrusi se
		// printf("Color %d has adress %p\n", list->first->color, (void *)list->first);
		// printf("Color %d has adress %p\n", list->last->color, (void *)list->last);
		if(list->first == list->last) {
			list->first = NULL;
			list->last = NULL;
		}
		// Pokud mel seznam vice nez 1 prvek
		else {
			// if(list->first != NULL)
			// 	printf("FIRST IS NOT NULL\n");
			// if(list->first->right_ptr == NULL)
			// 	printf("RPTR IS NULL\n");
			list->first = list->first->right_ptr; // 2. prvek je nyni 1.
			// if(list->first == NULL)
			// 	printf("FIRST IS NULL\n");
			list->first->left_ptr = NULL; // 1. prvek musi ukazovat na NULL
			// Pokud nyni prvni prvek ukazuje doprava na NULL, 
			// znamena to, ze je zaroven posledni
			// if(list->first->right_ptr == NULL)
			// 	list->last = list->first;
		}
		free(elem_ptr);
	}
	// ColorElement *first = list->first;
	// if (first) // seznam není prádzný
	// {
	// 	// Pokud je první prvek aktivní, aktivita se ztrácí.
	// 	if (first == list->active)
	// 	{
	// 		list->active = NULL;
	// 	}

	// 	if (first == list->last) // první prvek je zároveň poslední (seznam má jediný prvek)
	// 	{
	// 		list->last = NULL; // poslední prvek bude NULL
	// 	}
	// 	else // v seznamu je více prvku
	// 	{
	// 		first->right_ptr->left_ptr = NULL; // levý ukazatel druhého prvku bude NULL
	// 	}

	// 	// první prvek bude druhý prvek (může být i NULL v případě, že má seznam jediný prvek)
	// 	list->first = first->right_ptr;
	// 	free(first); // uvolnění prvního prvku
	// }
}

void delete_last(ColorList *list) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	ColorElement *elem_ptr;
	// Pokud je seznam prazdny, nic se nedeje
	if(list->last != NULL) {
		// Nutno ulozit ukazatel na posledni prvek, protoze potom se meni
		elem_ptr = list->last;
		// Pokud je posleni prvek aktivni, rusi se aktivita
		if(list->active == list->last)
			list->active = NULL;
		// Pokud mel seznam jediny prvek, zrusi se
		if(list->first == list->last) {
			list->first = NULL;
			list->last = NULL;
		}
		// Pokud mel seznam vice nez 1 prvek
		else {
			list->last = list->last->left_ptr; // predposledni prvek je nyni posledni
			list->last->right_ptr = NULL; // posledni prvek musi ukazovat na NULL
			// if(list->last->left_ptr == NULL)
			// 	list->first = list->last;
		}
		free(elem_ptr);
	}
	// ColorElement *last = list->last;
	// if (last) // seznam není prádzný
	// {
	// 	// Pokud je poslední prvek aktivní, aktivita se ztrácí.
	// 	if (last == list->active)
	// 	{
	// 		list->active = NULL;
	// 	}

	// 	if (last == list->first) // poslední prvek je zároveň první (seznam má jediný prvek)
	// 	{
	// 		list->first = NULL; // první prvek bude NULL
	// 	}
	// 	else // v seznamu je více prvku
	// 	{
	// 		last->left_ptr->right_ptr = NULL; // pravý ukazatel předposledního prvku bude NULL
	// 	}

	// 	// poslední prvek bude předposlední prvek (může být i NULL v případě, že má seznam jediný prvek)
	// 	list->last = last->left_ptr;
	// 	free(last); // uvolnění posledního prvku
	// }
}

void delete_active(ColorList *list) {
	if(list->active == list->first) {
		delete_first(list);
	}
	else if(list->active == list->last) {
		delete_last(list);
	}
	else {
		ColorElement *elem_ptr;

		/* If list is empty, nothing will happen */
		if(list->first != NULL) {
			elem_ptr = list->active;

			/* List is no longer active */
			list->active = NULL;

			/* Left neighbor now points to one element behind */
			elem_ptr->left_ptr->right_ptr = elem_ptr->right_ptr;

			/* Right neighbor now points to one element before */
			elem_ptr->right_ptr->left_ptr = elem_ptr->left_ptr;

			free(elem_ptr);
		}
	}
}

void delete_list(ColorList *list) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	ColorElement *elem_ptr = list->first;
	while(list->first != NULL) {
		list->first = list->first->right_ptr;
		free(elem_ptr);
		elem_ptr = list->first;
	}

	list->first = NULL;
	list->last = NULL;
	list->active = NULL;
	// ColorElement *current;
	// ColorElement *next;
	// if(list->first != NULL) {
	// 	current = list->first;
	// 	while(current != NULL) {
	// 		next = current->right_ptr;
	// 		free(current);
	// 		current = next;
	// 	}

	// 	list->first = NULL;
	// 	list->active = NULL;
	// 	list->last = NULL;
	// }
}

void copy_list(ColorList *list, ColorList *copy) {

	list_init(copy);

	/* Copy only if list isn't empty */
	if(list->first != NULL) {
		make_first_active(list);

		/* Copy until last element */
		while(is_list_active(list)) {

			/* For every element in original create new element 
			in copy list with that same color */
			insert_last(copy, copy_active(list));

			make_next_active(list);
		}
	}

}

bool is_list_active(ColorList *list) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return list->active != NULL;
}

/* Find out if color is in colorset. If yes, return true and leave 
it marked active, so it can be deleted */
bool find_color(ColorList *list, int color) {

	/* Search only if list isn't empty */
	if(list->first != NULL) {
		make_first_active(list);

		/* Search until last element */
		while(is_list_active(list)) {
			/* Color found, leave element active */
			if(copy_active(list) == color) {
				// printf("Found color: %d\n",copy_active(&list));
				return true;
			}
			/* Since list is sorted by color number and I only delete 
			colors from beginning, they stay sorted, so if I encounter
			bigger color, it's clear that smaller color isn't there */
			else if(copy_active(list) > color) {
				return false;
			}
			make_next_active(list);
		}
	}

	/* Color wasn't found */
	return false;
}

void print_list(ColorList *list, int node_id) {
	/* Print only if list isn't empty */
	if(list->first != NULL) {

		printf("Node %d: { ", node_id);

		make_first_active(list);
		/* Iterate until last element */
		while(is_list_active(list)) {
			/* Color found, leave element active */
			printf("%d ",list->active->color);
			// printf("\nActive has adress %p\n", (void *)list->active);
			make_next_active(list);
		}

		printf("}\n");
	}
	else {
		printf("List is empty\n");
	}
}


