//
// Created by ozgur on 9/7/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_SINGLE_LINKED_LIST_H
#define SEMAPHORE_EXAMPLES_CPP_SINGLE_LINKED_LIST_H

#include <iostream>

struct Node {
    int data;
    Node* next;

    Node(int val) : data(val), next(nullptr) {}
};

class SinglyLinkedList {
private:
    Node* head;

public:
    SinglyLinkedList() : head(nullptr) {}

    void append(int val)
    {
        Node* newNode = new Node(val);
        if (!head)
        {
            head = newNode;
        }
        else
        {
            Node* current = head;
            while (current->next)
            {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    bool search(int val)
    {
        Node* current = head;
        while (current)
        {
            if (current->data == val)
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool deleteValue(int val)
    {
        if (!head) return false;

        if (head->data == val)
        {
            Node* temp = head;
            head = head->next;
            delete temp;
            return true;
        }

        Node* current = head;
        while (current->next && current->next->data != val)
        {
            current = current->next;
        }

        if (current->next)
        {
            Node* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return true;
        }

        return false;
    }

    void display()
    {
        Node* current = head;
        while (current)
        {
            std::cout << current->data << " -> ";
            current = current->next;
        }
        std::cout << "nullptr" << std::endl;
    }

    int size()
    {
        int count = 0;
        Node* current = head;
        while (current)
        {
            count++;
            current = current->next;
        }
        return count;
    }

    ~SinglyLinkedList()
    {
        Node* current = head;
        while (current)
        {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
};

#endif //SEMAPHORE_EXAMPLES_CPP_SINGLE_LINKED_LIST_H
