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
    
    // rb tree iterator
    template <class T>
    struct rb_tree_iterator_base :public tinystl::iterator<tinystl::bidirectional_iterator_tag, T>
    {
        typedef typename rb_tree_traits<T>::base_ptr  base_ptr;

        base_ptr node;  // point to the node itself

        rb_tree_iterator_base() :node(nullptr) {}

        // make iterator move forward ++
        void inc()
        {
            if (node->right != nullptr)
            { 
                node = rb_tree_min(node->right);
            }
            else // if there is no right node
            { 
                auto y = node->parent;
                while (y->right == node)
                {
                    node = y;
                    y = y->parent;
                }
                // Deal with the special case of "looking for the next node of the root node, 
                // but the root node has no right child node"
                if (node->right != y) 
                    node = y;
            }
        }

        // make iterator move back --
        void dec()
        {
            if (node->parent->parent == node && rb_tree_is_red(node))
            { 
                // if the node is header
                node = node->right;  // point ro the max node in the tree
            }
            else if (node->left != nullptr)
            {
                node = rb_tree_max(node->left);
            }
            else
            {  
                // non-header and no left node
                auto y = node->parent;
                while (node == y->left)
                {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }

        bool operator==(const rb_tree_iterator_base& rhs)
        { return node == rhs.node; }

        bool operator!=(const rb_tree_iterator_base& rhs) 
        { return node != rhs.node; }
    };

    template <class T>
    struct rb_tree_iterator :public rb_tree_iterator_base<T>
    {
        typedef rb_tree_traits<T>   tree_traits;

        typedef typename tree_traits::value_type    value_type;
        typedef typename tree_traits::pointer       pointer;
        typedef typename tree_traits::reference     reference;
        typedef typename tree_traits::base_ptr      base_ptr;
        typedef typename tree_traits::node_ptr      node_ptr;

        typedef rb_tree_iterator<T>         iterator;
        typedef rb_tree_const_iterator<T>   const_iterator;
        typedef iterator                    self;

        using rb_tree_iterator_base<T>::node;

        // constructor
        rb_tree_iterator() 
        {}

        rb_tree_iterator(base_ptr x) 
        { node = x; }

        rb_tree_iterator(node_ptr x) 
        { node = x; }

        rb_tree_iterator(const iterator& rhs) 
        { node = rhs.node; }

        rb_tree_iterator(const const_iterator& rhs) 
        { node = rhs.node; }

        // overload operator
        reference operator*()  const 
        { return node->get_node_ptr()->value; }

        pointer operator->() const 
        { return &(operator*()); }

        self& operator++()
        {
            this->inc();
            return *this;
        }
        self operator++(int)
        {
            self tmp(*this);
            this->inc();
            return tmp;
        }
        self& operator--()
        {
            this->dec();
            return *this;
        }
        self operator--(int)
        {
            self tmp(*this);
            this->dec();
            return tmp;
        }
    };

    template <class T>
    struct rb_tree_const_iterator :public rb_tree_iterator_base<T>
    {
        typedef rb_tree_traits<T>   tree_traits;

        typedef typename tree_traits::value_type        value_type;
        typedef typename tree_traits::const_pointer     pointer;
        typedef typename tree_traits::const_reference   reference;
        typedef typename tree_traits::base_ptr          base_ptr;
        typedef typename tree_traits::node_ptr          node_ptr;

        typedef rb_tree_iterator<T>         iterator;
        typedef rb_tree_const_iterator<T>   const_iterator;
        typedef const_iterator              self;

        using rb_tree_iterator_base<T>::node;

        // contructor
        rb_tree_const_iterator() 
        {}

        rb_tree_const_iterator(base_ptr x) 
        { node = x; }

        rb_tree_const_iterator(node_ptr x) 
        { node = x; }

        rb_tree_const_iterator(const iterator& rhs) 
        { node = rhs.node; }

        rb_tree_const_iterator(const const_iterator& rhs) 
        { node = rhs.node; }

        // overload operator
        reference operator*()  const 
        { return node->get_node_ptr()->value; }

        pointer   operator->() const 
        { return &(operator*()); }

        self& operator++()
        {
            this->inc();
            return *this;
        }
        self operator++(int)
        {
            self tmp(*this);
            this->inc();
            return tmp;
        }
        self& operator--()
        {
            this->dec();
            return *this;
        }
        self operator--(int)
        {
            self tmp(*this);
            this->dec();
            return tmp;
        }
    };

    //=================tree algorithm======================================================

    template <class NodePtr>
    NodePtr rb_tree_min(NodePtr x) noexcept
    {
        while (x->left != nullptr)
            x = x->left;
        return x;
    }

    template <class NodePtr>
    NodePtr rb_tree_max(NodePtr x) noexcept
    {
        while (x->right != nullptr)
            x = x->right;
        return x;
    }

    // determine left child
    template <class NodePtr>
    bool rb_tree_is_lchild(NodePtr node) noexcept
    {
        return node == node->parent->left;
    }

    template <class NodePtr>
    bool rb_tree_is_red(NodePtr node) noexcept
    {
        return node->color == rb_tree_red;
    }

    template <class NodePtr>
    void rb_tree_set_black(NodePtr& node) noexcept
    {
        node->color = rb_tree_black;
    }

    template <class NodePtr>
    void rb_tree_set_red(NodePtr& node) noexcept
    {
        node->color = rb_tree_red;
    }

    template <class NodePtr>
    NodePtr rb_tree_next(NodePtr node) noexcept
    {
        if (node->right != nullptr)
            return rb_tree_min(node->right);
        while (!rb_tree_is_lchild(node))
            node = node->parent;
        return node->parent;
    }

    /*---------------------------------------*\
    |       p                         p       |
    |      / \                       / \      |
    |     x   d    rotate left      y   d     |
    |    / \       ===========>    / \        |
    |   a   y                     x   c       |
    |      / \                   / \          |
    |     b   c                 a   b         |
    \*---------------------------------------*/
    // the first parameter: left rotated point
    // the second parameter: root node
    template <class NodePtr>
    void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept
    {
        auto y = x->right;  // y is the left child node of x
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;

        y->parent = x->parent;

        if (x == root)
        { 
            // if x is root node, let y be the root node
            root = y;
        }
        else if (rb_tree_is_lchild(x))
        { 
            x->parent->left = y;
        }
        else
        { 
            // if x is right child node
            x->parent->right = y;
        }
        // adjust the relationship of x and y
        y->left = x;  
        x->parent = y;
    }

    /*----------------------------------------*\
    |     p                         p          |
    |    / \                       / \         |
    |   d   x      rotate right   d   y        |
    |      / \     ===========>      / \       |
    |     y   a                     b   x      |
    |    / \                           / \     |
    |   b   c                         c   a    |
    \*----------------------------------------*/
    // the first parameter: left rotated point
    // the second parameter: root node
    template <class NodePtr>
    void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept
    {
        auto y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;

        y->parent = x->parent;

        if (x == root)
        { 
            root = y;
        }
        else if (rb_tree_is_lchild(x))
        { 
            x->parent->left = y;
        }
        else
        { 
            x->parent->right = y;
        }
        y->right = x;                      
        x->parent = y;
    }
}
#endif
