/*
Orcus-0 - A simple, basic programming language.
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

#include <stdlib.h>

#include <orcus-0/linked_list.h>

static linked_list_node_t* MakeNew(void* Data)
{
    linked_list_node_t* Node = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
    Node->Data = Data;
    Node->Next = NULL;
    return Node;
}

linked_list_node_t* LinkedListCreate(void* Data)
{
    return MakeNew(Data);
}

void LinkedListDestroyAt(linked_list_node_t* List)
{
    linked_list_node_t* CurrNode = List;
    while (CurrNode->Next != NULL)
    {
        linked_list_node_t* ToDelete = CurrNode;
        CurrNode = CurrNode->Next;
        free(ToDelete->Data);
        free(ToDelete);
    }
    free(CurrNode->Data);
    free(CurrNode);
}

void LinkedListDestroySingular(linked_list_node_t* Node)
{
    free(Node->Data);
    free(Node);
}

linked_list_node_t* LinkedListAppend(linked_list_node_t* List, void* Data)
{
    if (List == NULL)
    {
        return LinkedListCreate(Data);
    }
    linked_list_node_t* DestNode = List;
    while (DestNode->Next != NULL)
    {
        DestNode = DestNode->Next;
    }
    DestNode->Next = MakeNew(Data);
    return List;
}

linked_list_node_t* LinkedListPrepend(linked_list_node_t* List, void* Data)
{
    linked_list_node_t* NewSrc = MakeNew(Data);
    NewSrc->Next = List;
    return NewSrc;
}

void LinkedListRemove(linked_list_node_t* List, size_t Indx)
{
    linked_list_node_t* PrevNode = NULL;
    linked_list_node_t* CurrNode = List;
    for (size_t I = 0; I < Indx; I++)
    {
        PrevNode = CurrNode;
        CurrNode = CurrNode->Next;
    }
    PrevNode->Next = CurrNode->Next;
    free(CurrNode->Data);
    free(CurrNode);
}

linked_list_node_t* LinkedListGet(linked_list_node_t* List, size_t Indx)
{
    linked_list_node_t* CurrNode = List;
    for (size_t I = 0; I < Indx; I++)
    {
        if (CurrNode->Next == NULL)
        {
            return NULL;
        }
        CurrNode = CurrNode->Next;
    }
    return CurrNode;
}