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
	ColorElement new_elem_ptr = calloc(1, sizeof(*new_elem_ptr));
	if(new_elem_ptr == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	new_elem_ptr->data = color; // priradim hodnotu
	// Novy prvek davam za posledni a ukazuji na nej levym ukazatelen
	new_elem_ptr->left_ptr = list->last;
	// Pravy ukazatel miri na NULL, aby pri vyhledavani bylo jasne, ze je konec
	new_elem_ptr->right_ptr = NULL;

	// Pokud uz seznam mel posledniho, tak posledni prvek bude ukazovat doprava 
	// na ten novy
	// MUSIM ZDE MENIT PODMINKU NA LAST?
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