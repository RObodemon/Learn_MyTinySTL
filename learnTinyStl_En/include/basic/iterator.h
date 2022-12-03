#ifndef TINYSTL_ITERATOR_H_
#define TINYSTL_ITERATOR_H_

#include <cstddef>

#include "type_traits.h"

namespace tinystl
{   
    // five iterator
    struct input_iterator_tag // read-only access to data, only one read operation.
    {};
    struct output_iterator_tag // write-only access to data, only one write operation.
    {};
    // Support read and write operations and support multiple read and write operations.
    struct forward_iterator_tag : public input_iterator_tag 
    {};
    // Support bidirectional movement and support multiple read and write operations.
    struct bidirectional_iterator_tag : public forward_iterator_tag 
    {};
    // Support bidirectional movement and support multiple read and write operations. 
    // p+n, p-n, etc.
    struct random_access_iterator_tag : public bidirectional_iterator_tag 
    {};

    // iterator template
    template <class Category, class T, class Distance = ptrdiff_t, 
        class Pointer = T*, class Reference = T&> 
    struct iterator
    {
        typedef Category    iterator_category; // iterator type
        typedef T           value_type;        // the type of the object pointed to by the iterator
        typedef Pointer     pointer;           // pointer
        typedef Reference   reference;         // reference
        typedef Distance    difference_type;   // distance between two iterators
    };   

    // iterator traits, used to determine iterator_category
    template <class T>
    struct has_iterator_cat
    {
    private:
        struct two 
        { 
          char a; 
          char b; 
        };
        // there is no specific realization of the test
        // test is used to trait
        // I am nor sure about why the three points can be used, it seems that need another variable input
        template <class U> 
        static two test(...); // return a struct (two) which has two char

        // return a char
        template <class U> 
        static char test(typename U::iterator_category* = 0);

    public:
        // The function of test<T>(0) is to match which test function template, 
        // generally speaking, 
        // by determing whether the return value is two chars or one char,
        // get by value.
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template <class Iterator, bool>
    struct iterator_traits_impl 
    {};

    template <class Iterator>
    struct iterator_traits_impl<Iterator, true>
    {
        // typedef creates an alias for the existing type,
        // and typename tells the compiler that this is the type, 
        // std::vector<T>::size_type is a type rather than a member.

        // typename is mainly used to extract nested dependent types.
        // Use typename to extract the parameter type, and name the parameter type once,
        // Seems to have a layer of indirection to the use of the parameter type.
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type        value_type;
        typedef typename Iterator::pointer           pointer;
        typedef typename Iterator::reference         reference;
        typedef typename Iterator::difference_type   difference_type;
    };

    template <class Iterator, bool>
    struct iterator_traits_helper 
    {};
    // std::is_convertible< class A, class B>::value  
    // Tests if A is convertible to B implicitly.   
    // determine if iterator_category can implictly convertible to input or output, 
    // then inherit from iterator_traits_impl
    template <class Iterator>
    struct iterator_traits_helper<Iterator, true>
      : public iterator_traits_impl<Iterator,
      std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
      std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
    {};

    // Extract iterator properties
    template <class Iterator>
    struct iterator_traits 
      : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> 
    {};

    // A partial specialization for raw pointers
    template <class T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag    iterator_category;
        typedef T                             value_type;
        typedef T*                            pointer;
        typedef T&                            reference;
        typedef ptrdiff_t                     difference_type;
    };

    template <class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag    iterator_category;
        typedef T                             value_type;
        typedef const T*                      pointer;
        typedef const T&                      reference;
        typedef ptrdiff_t                     difference_type;
    };

    // 
    // 
    template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of
      : public m_bool_constant<std::is_convertible<
      typename iterator_traits<T>::iterator_category, U>::value>
    {};

    template <class T, class U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type 
    {};

    // extract five iterators
    template <class Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> 
    {};

    template <class Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> 
    {};

    template <class Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> 
    {};

    template <class Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> 
    {};

    template <class Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> 
    {};

    template <class Iterator>
    struct is_iterator :
      public m_bool_constant<is_input_iterator<Iterator>::value ||
        is_output_iterator<Iterator>::value>
    {};

    // extract iterator's category
    template <class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&)
    {
      typedef typename iterator_traits<Iterator>::iterator_category Category;
      return Category();
    }

    // extract iterator's distance_type
    template <class Iterator>
    typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&)
    {
      return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // extract iterator's value_type
    template <class Iterator>
    typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&)
    {
      return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    // calculate the distance between iterators
    // input_iterator_tag
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
          ++first;
          ++n;
        }
        return n;
    }

    // random_access_iterator_tag
    template <class RandomIter>
    typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag)
    {
        return last - first;
    }

    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last)
    {
        return distance_dispatch(first, last, iterator_category(first));
    }

    // advance
    // input_iterator_tag
    template <class InputIterator, class Distance>
    void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
    {
      while (n--) 
        ++i;
    }

    // bidirectional_iterator_tag
    template <class BidirectionalIterator, class Distance>
    void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
    {
      if (n >= 0)
        while (n--)  ++i;
      else
        while (n++)  --i;
    }

    // random_access_iterator_tag
    template <class RandomIter, class Distance>
    void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
    {
      i += n;
    }

    template <class InputIterator, class Distance>
    void advance(InputIterator& i, Distance n)
    {
      advance_dispatch(i, n, iterator_category(i));
    }

    //---------------------------------------------------------------------------------------//
    // reverse_iterator
    // making forward as backward and backward as forward
    template <class Iterator>
    class reverse_iterator
    {
    private:
        Iterator current;  // forward iterator used for record.

    public:
        // five reverse_iteratros
        typedef typename iterator_traits<Iterator>::iterator_category   iterator_category;
        typedef typename iterator_traits<Iterator>::value_type          value_type;
        typedef typename iterator_traits<Iterator>::difference_type     difference_type;
        typedef typename iterator_traits<Iterator>::pointer             pointer;
        typedef typename iterator_traits<Iterator>::reference           reference;

        typedef Iterator                      iterator_type;
        typedef reverse_iterator<Iterator>    self;

    public:
        // construct function
        reverse_iterator() 
        {}
        // explicit constructor is used to prevent implicit conversion
        explicit reverse_iterator(iterator_type i) :current(i) 
        {}
        // copy construction
        reverse_iterator(const self& rhs) :current(rhs.current) 
        {}

    public:
        // get the forward iterator
        // Add const after the member function of the class, 
        // indicating that this function will not make any changes to the data members 
        // (non-static data members to be precise) of this class object.
        iterator_type base() const 
        { 
          return current; 
        }

        // overloaded operator
        reference operator*() const
        { 
            // 实际对应正向迭代器的前一个位置
            auto tmp = current;
            return *--tmp;
        }
        pointer operator->() const
        {
            return &(operator*());
        }

        // 前进(++)变为后退(--)
        self& operator++()
        {
            --current;
            return *this;
        }
        self operator++(int)
        {
            self tmp = *this;
            --current;
            return tmp;
        }
        // 后退(--)变为前进(++)
        self& operator--()
        {
            ++current;
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            ++current;
            return tmp;
        }

        self& operator+=(difference_type n)
        {
            current -= n;
            return *this;
        }
        self operator+(difference_type n) const
        {
            return self(current - n);
        }
        self& operator-=(difference_type n)
        {
            current += n;
            return *this;
        }
        self operator-(difference_type n) const
        {
            return self(current + n);
        }

        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }
    };

    // 重载 operator-
    template <class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
    {
        return rhs.base() - lhs.base();
    }

    // 重载比较操作符
    template <class Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <class Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs,
      const reverse_iterator<Iterator>& rhs)
    {
        return rhs.base() < lhs.base();
    }

    template <class Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return rhs < lhs;
    }

    template <class Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs < rhs);
    }

} // namespace tinystl

#endif // !MYTINYSTL_ITERATOR_H_

