#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <string>

using namespace std;

template <typename T>
struct Node {
    T key;
    string data;
    Node<T> *next;

    Node(T key, string data, Node<T> *next) : key(key), data(data), next(next) {
    }
};

template <typename T>
class List {
private:
    Node<T> *head;
    int count;

public:

    List() : head(nullptr), count(0) {
    }
    
    Node<T> * getFirst() {
        return head;
    }

    virtual ~List() {
        clear();
    }

    void print() {
        Node<T> *current = head;

        while (current != NULL) {
            cout << current->key << "\t";
            cout << current->data << endl;
            current = current->next;
        }
        
        if (head == nullptr) {
            cout << "list is empty " << endl;
        }
        
        
    }

    int getCount() {
        return count;
    }

    string search(T key) {
        Node<T> *current = head;

        while (current != nullptr) {
            if (current->key == key) {
                return current->data;
            }
            current = current->next;
        }

        return "";
    }

    

    string searchByPosition(int position) {
        Node<T> *current = head;

        while (current != nullptr) {
            if (position == 0) {
                return current->data;
            }
            current = current->next;
            position--;
        }

        return "";
    }

    void add(T key, string data) {
        Node<T> *newNode = new Node<T>(key, data, nullptr);

        if (head == nullptr) {
            head = newNode;
        } else {
            Node<T> *current = head;

            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        count++;
    }
    
    void add(T key) {
        add(key, "");
    }

    void removeFirst() {
        if (head == nullptr) {
            printf("u trying to remove the first ellement of an empty list \n");
            return;
        }

        Node<T> *temp = head;
        head = head->next;
        delete temp;

        count--;
    }

    void remove(T key) {
        if (head == nullptr) {
            return;
        }

        Node<T> *current = head;
        Node<T> *previous = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                if (previous == nullptr) {
                    // remove first node
                    head = current->next;
                } else {
                    previous->next = current->next;
                }
                delete current;
                
                count--;
                return;
            }
            previous = current;
            current = current->next;
        }

        count--;
    }
    
    T* keysToArray() {
        int n = this->count;
        
        T * array= new T[n];
        
        Node<T> *current = head;
        
        int i =0;
        
        while (current != nullptr) {
            array[i++] = current->key;
            current = current->next;
        }
        
        return array;
    }
    
    T* dataToArray() {
        int n = this->count;
        
        T * array= new T[n];
        
        Node<T> *current = head;
        
        int i =0;
        
        while (current != nullptr) {
            array[i++] = current->data;
            current = current->next;
        }
        
        return array;
    }

    void append(List<T>* other) {
    if (other == nullptr) {
        return;
    }
    if (this->head == nullptr) {
        this->head = other->head;
    } else {
        Node<T>* current = this->head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = other->head;
    }
    this->count += other->count;
    other->head = nullptr;
    delete other;
}
    
    void clear() {
        Node<T> *current = head;
        while (current != nullptr) {
            Node<T> *next = current->next;
            delete current;
            current = next;
        }
        
        this->head = nullptr;
    }
};

#endif /* LIST_H */
