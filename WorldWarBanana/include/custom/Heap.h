#pragma once
#include <iostream>
#include <stdexcept>
#include <vector>

#include "macros.h"

template <class T> class Heap
{
public:
    typedef typename std::vector<T>            heap_type;
    typedef typename heap_type::iterator       iterator;
    typedef typename heap_type::const_iterator const_iterator;

    inline iterator       begin() noexcept { return m_heap.begin(); }
    inline const_iterator cbegin() const noexcept { return m_heap.cbegin(); }
    inline iterator       end() noexcept { return m_heap.end(); }
    inline const_iterator cend() const noexcept { return m_heap.cend(); }

    inline unsigned int size() { return m_heap.size(); }
    inline bool         empty() { return size() == 0; }

    void push(T& key)
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
    T& top()
    {
        // if heap has no elements, throw an exception
        if (size() == 0)
            throw std::out_of_range("Vector<X>::at() : "
                                    "index is out of range(Heap underflow)");

        // else return the top (first) element
        return m_heap.at(0); // or return A[0];
    }

    T* contains(T& x)
    {
        for (T& i: m_heap)
        {
            if (i == x)
            {
                return &i;
            }
        }
        return nullptr;
    }

    friend std::ostream& operator<<(std::ostream& os, Heap<T>& v)
    {
        for (T& i: v)
        {
            os << i << std::endl;
        }
        return os;
    }

private:
    heap_type m_heap;

    inline int parent(int index) { return (index - 1) / 2; }

    inline int left_child(int index) { return 2 * index + 1; }

    inline int right_child(int index) { return 2 * index + 2; }

    void heapify_down(int i)
    {
        // get left and right child of node at index i
        uint left  = left_child(i);
        uint right = right_child(i);

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

    void swap(T& a, T& b)
    {
        T tmp = a;
        a     = b;
        b     = tmp;
    }
};