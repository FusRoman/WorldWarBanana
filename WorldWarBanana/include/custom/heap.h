#pragma once
#include <stdexcept>
#include <iostream>
#include <vector>

template <class T> class Heap
{

private:
    std::vector<T> m_heap;

    inline int parent(int index) { return (index - 1) / 2; }

    inline int left_child(int index) { return 2 * index + 1; }

    inline int right_child(int index) { return 2 * index + 2; }

    void heapify_down(int i)
    {
        // get left and right child of node at index i
        T left  = left_child(i);
        T right = right_child(i);

        int smallest = i;

        // compare A[i] with its left and right child
        // and find smallest value
        if (left < size() && m_heap[left] < m_heap[i])
            smallest = left;

        if (right < size() && m_heap[right] < m_heap[smallest])
            smallest = right;

        // swap with child having lesser value and
        // call heapify-down on the child
        if (smallest != i)
        {
            swap(m_heap[i], m_heap[smallest]);
            heapify_down(smallest);
        }
    }

    void heapify_up(int i)
    {
        // check if node at index i and its parent violates
        // the heap property
        if (i && m_heap[i] < m_heap[parent(i)])
        {
            // swap the two if heap property is violated
            swap(m_heap[i], m_heap[parent(i)]);

            // call Heapify-up on the parent
            heapify_up(parent(i));
        }
    }

public:
    inline unsigned int size() { return m_heap.size(); }
    inline bool         empty() { return size() == 0; }

    void push(T key)
    {
        // insert the new element to the end of the vector
        m_heap.push_back(key);

        // get element index and call heapify-up procedure
        int index = size() - 1;
        heapify_up(index);
    }
    void pop()
    {
        try
        {
            // if heap has no elements, throw an exception
            if (size() == 0)
                throw std::out_of_range("Vector<X>::at() : "
                                   "index is out of range(Heap underflow)");

            // replace the root of the heap with the last element
            // of the vector
            m_heap[0] = m_heap.back();
            m_heap.pop_back();

            // call heapify-down on root node
            heapify_down(0);
        }
        // catch and print the exception
        catch (const std::out_of_range& oor)
        {
            std::cout << "\n" << oor.what();
        }
    }
    T top()
    {
        try
        {
            // if heap has no elements, throw an exception
            if (size() == 0)
                throw std::out_of_range("Vector<X>::at() : "
                                   "index is out of range(Heap underflow)");

            // else return the top (first) element
            return m_heap.at(0); // or return A[0];
        }
        // catch and print the exception
        catch (const std::out_of_range& oor)
        {
            std::cout << "\n" << oor.what();
        }
    }

    bool contains(T x){
        for(T i : m_heap){
            if(i == x){
                return true;
            }
        }
        return false;
    }
};