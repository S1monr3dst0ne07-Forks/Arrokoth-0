/*
Arrokoth-0 - A simple, basic programming language.
Copyright (C) 2026  NorbCodes

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ARROKOTH_LINKED_LIST_H
#define ARROKOTH_LINKED_LIST_H

#include <stdint.h>

typedef struct linked_list_node_s
{
    void* Data;
    struct linked_list_node_s* Next;
}
linked_list_node_t;

linked_list_node_t* LinkedListCreate(void* Data);
void LinkedListDestroyAt(linked_list_node_t* List);
void LinkedListDestroySingular(linked_list_node_t* Node);
linked_list_node_t* LinkedListAppend(linked_list_node_t* List, void* Data);
linked_list_node_t* LinkedListAppendDirectly(linked_list_node_t* List, linked_list_node_t* Node);
linked_list_node_t* LinkedListPrepend(linked_list_node_t* List, void* Data);
void LinkedListRemove(linked_list_node_t* List, size_t Indx);
linked_list_node_t* LinkedListGet(linked_list_node_t* List, size_t Indx);

#endif // ARROKOTH_LINKED_LIST_H