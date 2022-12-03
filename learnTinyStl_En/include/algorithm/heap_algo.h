#ifndef HEAP_ALGO_H_
#define HEAP_ALGO_H_

// heap: push_heap, pop_heap, sort_heap, make_heap

#include "iterator.h"
// Heap is a special complete binary tree data structure, 
// divided into maximum heap and minimum heap.
// Maximum heap: a complete binary tree whose root node is greater 
// than the left and right child nodes
// Minimum heap: a complete binary tree whose root node is smaller 
// than the left and right child nodes. The heap can be stored in an array.
namespace tinystl
{

    //-----------------------------------------------------------------------------------------
    // push_heap ()
    // Accept two iterators, first and last. 
    // Remeber: The new element has been inserted to the end, make heap to the structure

    // holdIndex: last element's idx
    // topIndex:  first element's idx
    template <class RandomIter, class Distance, class T>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value)
    {
        // see push_heap.md file
        // get the parent node
        auto parent = (holeIndex - 1) / 2;
        // if holeIndex <= topIndex, the heap is null
        // then determine which one is big, to makesure where to put value
        while (holeIndex > topIndex && *(first + parent) < value)
        {
            // Use operator<, so the heap is max-heap

            // the value of parent node to down
            *(first + holeIndex) = *(first + parent);
            // update holeIndex
            holeIndex = parent;
            // update parent node
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomIter, class Distance>
    void push_heap_d(RandomIter first, RandomIter last, Distance*)
    {
        tinystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
    }

    template <class RandomIter>
    void push_heap(RandomIter first, RandomIter last)
    { 
        // The new element should have been placed at the very end of the bottom container
        tinystl::push_heap_d(first, last, distance_type(first));
    }

    // overload version: comp()
    // holdIndex: last element's idx
    // topIndex:  first element's idx
    template <class RandomIter, class Distance, class T, class Compared>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, 
        T value, Compared comp)
    {
        auto parent = (holeIndex - 1) / 2;
        while (holeIndex > topIndex && comp(*(first + parent), value))
        {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomIter, class Compared, class Distance>
    void push_heap_d(RandomIter first, RandomIter last, Distance*, Compared comp)
    {
        tinystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0),
            *(last - 1), comp);
    }

    template <class RandomIter, class Compared>
    void push_heap(RandomIter first, RandomIter last, Compared comp)
    {
        tinystl::push_heap_d(first, last, distance_type(first), comp);
    }

    //-----------------------------------------------------------------------------------------
    // pop_heap
    // Accept two iterators, first and last. Take the root node of the heap 
    // and put it at the end of the container. Lastly, make the heap

    template <class RandomIter, class T, class Distance>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value)
    {
        // percolate down
        auto topIndex = holeIndex;
        // way to get the rihgt child node
        auto rchild = 2 * holeIndex + 2;

        while (rchild < len)
        {
            // determine the biggest node between right node and left node
            if (*(first + rchild) < *(first + rchild - 1))
                --rchild;
            // push the bigger one
            *(first + holeIndex) = *(first + rchild);
            // update the idx
            holeIndex = rchild;
            // update the idx of right child node
            rchild = 2 * (rchild + 1);
        }
        if (rchild == len)
        {  
            // if there is no right child node
            // push left child node
            *(first + holeIndex) = *(first + (rchild - 1));
            holeIndex = rchild - 1;
        }
        // percolate up: push the last value in
        tinystl::push_heap_aux(first, holeIndex, topIndex, value);
    }

    template <class RandomIter, class T, class Distance>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, 
        T value, Distance*)
    {
        // First move the first value to the end node, 
        // then make [first, last - 1) it a max-heap again
        // result = last-1
        *result = *first;
        tinystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
    }

    template <class RandomIter>
    void pop_heap(RandomIter first, RandomIter last)
    {
        tinystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
    }

    // overload version
    template <class RandomIter, class T, class Distance, class Compared>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value,
                    Compared comp)
    {
        // percolate down
        auto topIndex = holeIndex;
        auto rchild = 2 * holeIndex + 2;
        while (rchild < len)
        {
            if (comp(*(first + rchild), *(first + rchild - 1)))  --rchild;
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = 2 * (rchild + 1);
        }
        if (rchild == len)
        {
            *(first + holeIndex) = *(first + (rchild - 1));
            holeIndex = rchild - 1;
        }
        // percolate up
        tinystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
    }

    template <class RandomIter, class T, class Distance, class Compared>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, 
                    T value, Distance*, Compared comp)
    {
        *result = *first; 
        tinystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
    }

    template <class RandomIter, class Compared>
    void pop_heap(RandomIter first, RandomIter last, Compared comp)
    {
        tinystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1),
            distance_type(first), comp);
    }

    //-----------------------------------------------------------------------------------------
    // sort_heap
    // Continue to perform pop_heap operations 
    // until the difference between the beginning and the end is at most 1
    template <class RandomIter>
    void sort_heap(RandomIter first, RandomIter last)
    {
        while (last - first > 1)
        {
            tinystl::pop_heap(first, last--);
        }
    }

    // overload version
    template <class RandomIter, class Compared>
    void sort_heap(RandomIter first, RandomIter last, Compared comp)
    {
        while (last - first > 1)
        {
            tinystl::pop_heap(first, last--, comp);
        }
    }

    //-----------------------------------------------------------------------------------------
    // make_heap
    // Turn the data in the container into a heap
    template <class RandomIter, class Distance>
    void make_heap_aux(RandomIter first, RandomIter last, Distance*)
    {
        // less than two elements, it is kind of heap
        if (last - first < 2)
            return;
        auto len = last - first;
        auto holeIndex = (len - 2) / 2;
        while (true)
        {
            tinystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
            if (holeIndex == 0)
                return;
            holeIndex--;
        }
    }

    template <class RandomIter>
    void make_heap(RandomIter first, RandomIter last)
    {
        // extract type
        tinystl::make_heap_aux(first, last, distance_type(first));;
    }

    // overload
    template <class RandomIter, class Distance, class Compared>
    void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compared comp)
    {
        if (last - first < 2)
            return;
        auto len = last - first;
        auto holeIndex = (len - 2) / 2;
        while (true)
        {
            tinystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
            if (holeIndex == 0)
            return;
            holeIndex--;
        }
    }

    template <class RandomIter, class Compared>
    void make_heap(RandomIter first, RandomIter last, Compared comp)
    {
        tinystl::make_heap_aux(first, last, distance_type(first), comp);
    }

} // namespace tinystl
#endif 