#ifndef _TREE_H_
#define _TREE_H_

// rb_tree : red-black tree

#include <initializer_list>
#include <cassert>

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"

namespace tinystl
{

    // rb tree type of node's color
    typedef bool rb_tree_color_type;
    // flase: red
    // true: black
    static constexpr rb_tree_color_type rb_tree_red   = false;
    static constexpr rb_tree_color_type rb_tree_black = true;

    // forward declaration
    template <class T> struct rb_tree_node_base;
    template <class T> struct rb_tree_node;

    template <class T> struct rb_tree_iterator;
    template <class T> struct rb_tree_const_iterator;

    // rb tree value traits
    template <class T, bool>
    struct rb_tree_value_traits_imp
    {
        typedef T key_type;
        typedef T mapped_type;
        typedef T value_type;

        template <class Ty>
        static const key_type& get_key(const Ty& value)
        {
            return value;
        }

        template <class Ty>
        static const value_type& get_value(const Ty& value)
        {
            return value;
        }
    };

    template <class T>
    struct rb_tree_value_traits_imp<T, true>
    {
        // remove_cv:
        // An instance of remove_cv<T> holds a modified-type 
        // that is T1 when T is of the form const T1, volatile T1, or const volatile T1, 
        // otherwise T.
        typedef typename std::remove_cv<typename T::first_type>::type   key_type;
        typedef typename T::second_type                                 mapped_type;
        typedef T                                                       value_type;

        template <class Ty>
        static const key_type& get_key(const Ty& value)
        {
            return value.first;
        }

        template <class Ty>
        static const value_type& get_value(const Ty& value)
        {
            return value;
        }
    };

    template <class T>
    struct rb_tree_value_traits
    {
        static constexpr bool is_map = tinystl::is_pair<T>::value;

        typedef rb_tree_value_traits_imp<T, is_map>         value_traits_type;

        typedef typename value_traits_type::key_type        key_type;
        typedef typename value_traits_type::mapped_type     mapped_type;
        typedef typename value_traits_type::value_type      value_type;

        template <class Ty>
        static const key_type& get_key(const Ty& value)
        {
            return value_traits_type::get_key(value);
        }

        template <class Ty>
        static const value_type& get_value(const Ty& value)
        {
            return value_traits_type::get_value(value);
        }
    };

    // rb tree node traits
    template <class T>
    struct rb_tree_node_traits
    {
        typedef rb_tree_color_type                  color_type;

        typedef rb_tree_value_traits<T>             value_traits;
        typedef typename value_traits::key_type     key_type;
        typedef typename value_traits::mapped_type  mapped_type;
        typedef typename value_traits::value_type   value_type;

        typedef rb_tree_node_base<T>*   base_ptr;
        typedef rb_tree_node<T>*        node_ptr;
    };

    // rb tree node design
    template <class T>
    struct rb_tree_node_base
    {
        typedef rb_tree_color_type    color_type;
        typedef rb_tree_node_base<T>* base_ptr;
        typedef rb_tree_node<T>*      node_ptr;

        base_ptr   parent;  // parent node
        base_ptr   left;    // left child node
        base_ptr   right;   // right child node
        color_type color;   // node's color

        base_ptr get_base_ptr()
        {
            return &*this;
        }

        node_ptr get_node_ptr()
        {
            // reinterpret_cast:
            // Allows any pointer to be converted into any other pointer type. 
            // Also allows any integral type to be converted into any pointer type and vice versa.
            return reinterpret_cast<node_ptr>(&*this);
        }

        node_ptr& get_node_ref()
        {
            return reinterpret_cast<node_ptr&>(*this);
        }
    };

    template <class T>
    struct rb_tree_node :public rb_tree_node_base<T>
    {
        typedef rb_tree_node_base<T>*   base_ptr;
        typedef rb_tree_node<T>*        node_ptr;

        T value; // value

        base_ptr get_base_ptr()
        {
            return static_cast<base_ptr>(&*this);
        }

        node_ptr get_node_ptr()
        {
            return &*this;
        }
    };

    // rb tree traits
    template <class T>
    struct rb_tree_traits
    {
        typedef rb_tree_value_traits<T>     value_traits;

        typedef typename value_traits::key_type     key_type;
        typedef typename value_traits::mapped_type  mapped_type;
        typedef typename value_traits::value_type   value_type;

        typedef value_type*         pointer;
        typedef value_type&         reference;
        typedef const value_type*   const_pointer;
        typedef const value_type&   const_reference;

        typedef rb_tree_node_base<T>    base_type;
        typedef rb_tree_node<T>         node_type;

        typedef base_type*      base_ptr;
        typedef node_type*      node_ptr;
    };
}
#endif
