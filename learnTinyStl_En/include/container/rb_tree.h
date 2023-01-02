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
#include "allocator.h"

namespace tinystl
{
    // rb tree type of node's color
    typedef bool rb_tree_color_type;

    // flase: red
    // true: black
    static constexpr rb_tree_color_type rb_tree_red   = false;
    static constexpr rb_tree_color_type rb_tree_black = true;

    // advance declaration
    template <class T> 
    struct rb_tree_node_base;

    template <class T> 
    struct rb_tree_node;

    template <class T> 
    struct rb_tree_iterator;

    template <class T> 
    struct rb_tree_const_iterator;

    // rb tree value traits
    template <class T, bool>
    struct rb_tree_value_traits_imp
    {
        typedef T   key_type;
        typedef T   mapped_type;
        typedef T   value_type;

        template <class Ty>
        static const key_type& get_key(const Ty& key)
        {
            return key;
        }

        template <class Ty>
        static const value_type& get_value(const Ty& value)
        {
            return value;
        }
    };

    template <class T> // <key, mapped_type> //
    struct rb_tree_value_traits_imp<T, true>
    {
        // remove_cv:
        // An instance of remove_cv<T> holds a modified-type that is T1 
        // when T is of the form const T1, volatile T1, or const volatile T1, 
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
        // Determine whether it is a pair
        static constexpr bool is_map = tinystl::is_pair<T>::value;

        // extract the type: 
        // if is_map is true......
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
        // color_type is bool
        typedef rb_tree_color_type      color_type;
        typedef rb_tree_node_base<T>*   base_ptr;
        typedef rb_tree_node<T>*        node_ptr;

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
            // Allow any pointer to be converted into any other pointer type. 
            // Also allows any integral type to be converted into any pointer type 
            // and vice versa.
            return reinterpret_cast<node_ptr>(&*this); // pointer
        }

        node_ptr& get_node_ref()
        {
            return reinterpret_cast<node_ptr&>(*this); // reference
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
    struct rb_tree_iterator_base 
        :public tinystl::iterator<tinystl::bidirectional_iterator_tag, T>
    {
        typedef typename rb_tree_traits<T>::base_ptr  base_ptr;

        base_ptr node;  // point to the node itself
        // constructor, node points to the nullptr
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
                // if there is no next node, return the header node
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
            // if the node is header
            if (node->parent->parent == node && rb_tree_is_red(node))
            { 
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

    //=========== rotate ==================================================================
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

    //========================= insert ====================================================
    // Rebalance the rb tree after inserting nodes, 
    // the first parameter is the new node, and the second parameter is the root node
    //
    // case 1: The newly added node is located at the root node, 
    //      so the newly added node is black
    // case 2: The parent node of the newly added node is black, 
    //      without breaking the balance, return directly
    // case 3: Both the parent node and the uncle node are red, 
    //      so the parent node and uncle node are black, and the grandparent node is red,
    //      Then make the grandparent node the current node and continue processing
    // case 4: the parent node is red, the uncle node is NIL or black, 
    //      the parent node is the left (right) child, the current node is the right (left) child,
    //      Let the parent node become the current node, 
    //      and then use the current node as the fulcrum to rotate left (right)
    // case 5: the parent node is red, the uncle node is NIL or black, 
    //      the parent node is the left (right) child, the current node is the left (right) child,
    //      Make the parent node black, the grandparent node red, 
    //      rotate right (left) with the grandparent node as the fulcrum
    //
    // blogs: http://blog.csdn.net/v_JULY_v/article/details/6105630
    //          http://blog.csdn.net/v_JULY_v/article/details/6109153
    template <class NodePtr>
    void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept
    {
        rb_tree_set_red(x);  // add new node which is red
        while (x != root && rb_tree_is_red(x->parent))
        {
            if (rb_tree_is_lchild(x->parent))
            { 
                auto uncle = x->parent->parent->right;
                if (uncle != nullptr && rb_tree_is_red(uncle))
                { 
                    // case 3
                    rb_tree_set_black(x->parent);
                    rb_tree_set_black(uncle);
                    x = x->parent->parent;
                    rb_tree_set_red(x);
                }
                else
                { 
                    // no uncle node, or uncle node is black
                    if (!rb_tree_is_lchild(x))
                    { 
                        // case 4
                        x = x->parent;
                        rb_tree_rotate_left(x, root);
                    }
                    // case 5
                    rb_tree_set_black(x->parent);
                    rb_tree_set_red(x->parent->parent);
                    rb_tree_rotate_right(x->parent->parent, root);
                    break;
                }
            }
            else
            { 
                auto uncle = x->parent->parent->left;
                if (uncle != nullptr && rb_tree_is_red(uncle))
                { 
                    // case 3
                    rb_tree_set_black(x->parent);
                    rb_tree_set_black(uncle);
                    x = x->parent->parent;
                    rb_tree_set_red(x);
                }
                else
                { 
                    // no uncle node or uncle node is black
                    if (rb_tree_is_lchild(x))
                    { 
                        // case 4
                        x = x->parent;
                        rb_tree_rotate_right(x, root);
                    }
                    // case 5
                    rb_tree_set_black(x->parent);
                    rb_tree_set_red(x->parent->parent);
                    rb_tree_rotate_left(x->parent->parent, root);
                    break;
                }
            }
        }
        rb_tree_set_black(root);
    }

    //========== erase ===================================================================
    // erase node and reblance rb_tree
    // first parameter:  erase node
    // second parameter: root node
    // third parameter:  minmum node
    // fourth parameter: maximum node
    // blogs: http://blog.csdn.net/v_JULY_v/article/details/6105630
    //        http://blog.csdn.net/v_JULY_v/article/details/6109153
    template <class NodePtr>
    NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, 
                                    NodePtr& leftmost, NodePtr& rightmost)
    {
        // y is the possible replacement node, pointing to the node to be removed eventually
        auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
        // x is "a signle child node" or "NIL node" of y
        auto x = y->left != nullptr ? y->left : y->right;
        // xp here is parent node of x
        NodePtr xp = nullptr;

        // y != z 
        // Explain that z has two non-empty child nodes, 
        // at this time y points to the leftmost node of the right subtree of z, 
        // and x points to the right child node of y.

        // Replace z's position with y, replace y's position with x, and finally point y to z
        if (y != z)
        {
            z->left->parent = y;
            y->left = z->left;

            // If y is not the right child of z, then the right child of z must have a left child
            if (y != z->right)
            { 
                // where x replaces y
                xp = y->parent;
                if (x != nullptr)
                    x->parent = y->parent;

                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            }
            else
            {
                xp = y;
            }

            // connect y to z's parent node
            if (root == z)
                root = y;
            else if (rb_tree_is_lchild(z))
                z->parent->left = y;
            else
                z->parent->right = y;

            y->parent = z->parent;
            tinystl::swap(y->color, z->color);
            y = z;
        }
        else // "y == z" notes that z has at most one child
        { 
            xp = y->parent;
            if (x)  
                x->parent = y->parent;

            // connect x to z's parent node
            if (root == z)
                root = x;
            else if (rb_tree_is_lchild(z))
                z->parent->left = x;
            else
                z->parent->right = x;

            // At this time z may be the leftmost node or the rightmost node, update the data
            if (leftmost == z)
                leftmost = x == nullptr ? xp : rb_tree_min(x);
            if (rightmost == z)
                rightmost = x == nullptr ? xp : rb_tree_max(x);
        }

        // At this point, y points to the node to be deleted, 
        // x is the replacement node, and the adjustment starts from the x node.
        // If the deleted node is red, the nature of the tree has not been destroyed, 
        // otherwise adjust according to the following conditions 
        // (x is the left child node as an example):

        // case 1: The sibling node is red, the parent node is red, 
        //      the sibling node is black, left (right) rotation, continue processing
        // case 2: The sibling node is black, and both child nodes are black or NIL, 
        //      so the sibling node is red, and the parent node becomes the current node, 
        //      continue processing
        // case 3: the sibling node is black, the left child node is red or NIL, 
        //      the right child node is black or NIL, Make the sibling node red, 
        //      the left child node of the sibling node is black, 
        //      rotate right (left) with the sibling node as the fulcrum, 
        //      and continue processing
        // case 4: The brother node is black, the right child node is red, 
        //      the brother node is the color of the parent node, the parent node is black, 
        //      the right child node of the brother node is black, 
        //      and the parent node is used as the fulcrum to rotate left (right), 
        //      and the nature of the tree is adjusted. , the algorithm ends
        if (!rb_tree_is_red(y))
        { 
            // When x is black, adjust it, otherwise just change x to black
            while (x != root && (x == nullptr || !rb_tree_is_red(x)))
            {
                if (x == xp->left)
                {
                    auto brother = xp->right;
                    if (rb_tree_is_red(brother))
                    { // case 1
                        rb_tree_set_black(brother);
                        rb_tree_set_red(xp);
                        rb_tree_rotate_left(xp, root);
                        brother = xp->right;
                    }
                    // case 1 transfers into one of case 2、3、4 
                    if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                        (brother->right == nullptr || !rb_tree_is_red(brother->right)))
                    { 
                        // case 2
                        rb_tree_set_red(brother);
                        x = xp;
                        xp = xp->parent;
                    }
                    else
                    { 
                        if (brother->right == nullptr || !rb_tree_is_red(brother->right))
                        { 
                            // case 3
                            if (brother->left != nullptr)
                            rb_tree_set_black(brother->left);
                            rb_tree_set_red(brother);
                            rb_tree_rotate_right(brother, root);
                            brother = xp->right;
                        }
                        // case 4
                        brother->color = xp->color;
                        rb_tree_set_black(xp);
                        if (brother->right != nullptr)  
                            rb_tree_set_black(brother->right);
                        rb_tree_rotate_left(xp, root);
                        break;
                    }
                }
                else
                { 
                    auto brother = xp->left;
                    if (rb_tree_is_red(brother))
                    { // case 1
                        rb_tree_set_black(brother);
                        rb_tree_set_red(xp);
                        rb_tree_rotate_right(xp, root);
                        brother = xp->left;
                    }
                    if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                        (brother->right == nullptr || !rb_tree_is_red(brother->right)))
                    { 
                        // case 2
                        rb_tree_set_red(brother);
                        x = xp;
                        xp = xp->parent;
                    }
                    else
                    {
                    if (brother->left == nullptr || !rb_tree_is_red(brother->left))
                    { 
                        // case 3
                        if (brother->right != nullptr)
                            rb_tree_set_black(brother->right);

                        rb_tree_set_red(brother);
                        rb_tree_rotate_left(brother, root);
                        brother = xp->left;
                    }
                    // case 4
                    brother->color = xp->color;
                    rb_tree_set_black(xp);
                    if (brother->left != nullptr)  
                        rb_tree_set_black(brother->left);
                    rb_tree_rotate_right(xp, root);
                    break;
                    }
                }
            }
            if (x != nullptr)
            rb_tree_set_black(x);
        }
        return y;
    }

    // rb_tree=============================================================================
    // first parameter:  value type
    // second parameter: Key-value comparison type
    template <class T, class Compare>
    class rb_tree
    {
    public:
        typedef rb_tree_traits<T>           tree_traits;
        typedef rb_tree_value_traits<T>     value_traits;

        typedef typename tree_traits::base_type     base_type;
        typedef typename tree_traits::base_ptr      base_ptr;
        typedef typename tree_traits::node_type     node_type;
        typedef typename tree_traits::node_ptr      node_ptr;
        typedef typename tree_traits::key_type      key_type;
        typedef typename tree_traits::mapped_type   mapped_type;
        typedef typename tree_traits::value_type    value_type;

        typedef Compare     key_compare;
        
        // allocators
        typedef tinystl::allocator<T>           allocator_type;
        typedef tinystl::allocator<T>           data_allocator;
        typedef tinystl::allocator<base_type>   base_allocator;
        typedef tinystl::allocator<node_type>   node_allocator;

        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;

        // iterators
        typedef rb_tree_iterator<T>                         iterator;
        typedef rb_tree_const_iterator<T>                   const_iterator;
        typedef tinystl::reverse_iterator<iterator>         reverse_iterator;
        typedef tinystl::reverse_iterator<const_iterator>   const_reverse_iterator;

        allocator_type get_allocator() const 
        { return node_allocator(); }

        key_compare key_comp() const 
        { return key_comp_; }

    private:
        // rb tree
        // Special node, which is the parent node of each other with the root node
        base_ptr    header_;      
        size_type   node_count_;  // # of nodes
        key_compare key_comp_;    // Key-value comparison rule

    private:
        // root node, min node and max node
        base_ptr& root() const 
        { return header_->parent; }

        base_ptr& leftmost() const 
        { return header_->left; }

        base_ptr& rightmost() const 
        { return header_->right; }

    public:
        // constructor
        rb_tree() 
        { rb_tree_init(); }

        // copy constructor
        rb_tree(const rb_tree& rhs);
        // move constructor
        rb_tree(rb_tree&& rhs) noexcept;

        // copy assignment
        rb_tree& operator=(const rb_tree& rhs);
        // move assignment
        rb_tree& operator=(rb_tree&& rhs);

        // deconstructor
        ~rb_tree() 
        { clear(); }

    public:
        // iterator related operations
        iterator begin() noexcept 
        { return leftmost(); }

        const_iterator begin() const noexcept
        { return leftmost(); }

        iterator end() noexcept
        { return header_; }

        const_iterator end() const noexcept
        { return header_; }

        reverse_iterator rbegin() noexcept
        { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept
        { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept
        { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator cbegin() const noexcept
        { return begin(); }

        const_iterator cend() const noexcept
        { return end(); }

        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }

        const_reverse_iterator crend() const noexcept
        { return rend(); }

        // container related operations====================================================
        bool empty() const noexcept 
        { return node_count_ == 0; }

        size_type size() const noexcept 
        { return node_count_; }

        size_type max_size() const noexcept 
        { return static_cast<size_type>(-1); }

        // emplace
        template <class ...Args>
        iterator  emplace_multi(Args&& ...args);

        template <class ...Args>
        tinystl::pair<iterator, bool> emplace_unique(Args&& ...args);

        template <class ...Args>
        iterator  emplace_multi_use_hint(iterator hint, Args&& ...args);

        template <class ...Args>
        iterator  emplace_unique_use_hint(iterator hint, Args&& ...args);

        // insert
        iterator insert_multi(const value_type& value);
        
        iterator insert_multi(value_type&& value)
        {
            return emplace_multi(tinystl::move(value));
        }

        iterator  insert_multi(iterator hint, const value_type& value)
        {
            return emplace_multi_use_hint(hint, value);
        }
        iterator  insert_multi(iterator hint, value_type&& value)
        {
            return emplace_multi_use_hint(hint, tinystl::move(value));
        }

        template <class InputIterator>
        void insert_multi(InputIterator first, InputIterator last)
        {
            size_type n = tinystl::distance(first, last);
            THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
            for (; n > 0; --n, ++first)
                insert_multi(end(), *first);
        }

        tinystl::pair<iterator, bool> insert_unique(const value_type& value);
        tinystl::pair<iterator, bool> insert_unique(value_type&& value)
        {
            return emplace_unique(tinystl::move(value));
        }

        iterator  insert_unique(iterator hint, const value_type& value)
        {
            return emplace_unique_use_hint(hint, value);
        }
        iterator  insert_unique(iterator hint, value_type&& value)
        {
            return emplace_unique_use_hint(hint, tinystl::move(value));
        }

        template <class InputIterator>
        void insert_unique(InputIterator first, InputIterator last)
        {
            size_type n = tinystl::distance(first, last);
            THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
            for (; n > 0; --n, ++first)
            insert_unique(end(), *first);
        }

        // erase
        iterator erase(iterator hint);

        size_type erase_multi(const key_type& key);
        size_type erase_unique(const key_type& key);

        void erase(iterator first, iterator last);

        void clear();

        // rb_tree related operations------------------------------------------------------
        iterator find(const key_type& key);
        const_iterator find(const key_type& key) const;

        size_type count_multi(const key_type& key) const
        {
            auto p = equal_range_multi(key);
            return static_cast<size_type>(tinystl::distance(p.first, p.second));
        }
        size_type count_unique(const key_type& key) const
        {
            return find(key) != end() ? 1 : 0;
        }

        iterator lower_bound(const key_type& key);
        const_iterator lower_bound(const key_type& key) const;

        iterator upper_bound(const key_type& key);
        const_iterator upper_bound(const key_type& key) const;

        tinystl::pair<iterator, iterator>             
        equal_range_multi(const key_type& key)
        {
            return tinystl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
        }
        tinystl::pair<const_iterator, const_iterator>
        equal_range_multi(const key_type& key) const
        {
            return tinystl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
        }

        tinystl::pair<iterator, iterator>             
        equal_range_unique(const key_type& key)
        {
            iterator it = find(key);
            auto next = it;
            return it == end() ? tinystl::make_pair(it, it) : tinystl::make_pair(it, ++next);
        }
        tinystl::pair<const_iterator, const_iterator> 
        equal_range_unique(const key_type& key) const
        {
            const_iterator it = find(key);
            auto next = it;
            return it == end() ? tinystl::make_pair(it, it) : tinystl::make_pair(it, ++next);
        }

        void swap(rb_tree& rhs) noexcept;

    private:

        // node related
        template <class ...Args>
        node_ptr create_node(Args&&... args);
        node_ptr clone_node(base_ptr x);
        void     destroy_node(node_ptr p);

        // init / reset
        void     rb_tree_init();
        void     reset();

        // get insert pos
        tinystl::pair<base_ptr, bool> 
                get_insert_multi_pos(const key_type& key);
        tinystl::pair<tinystl::pair<base_ptr, bool>, bool> 
                get_insert_unique_pos(const key_type& key);

        // insert value / insert node
        iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
        iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

        // insert use hint
        iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
        iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

        // copy tree / erase tree
        base_ptr copy_from(base_ptr x, base_ptr p);
        void     erase_since(base_ptr x);
    };

    //==========implement==================================================================
    template <class T, class Compare>
    rb_tree<T, Compare>::rb_tree(const rb_tree& rhs)
    {
        rb_tree_init();
        if (rhs.node_count_ != 0)
        {
            root() = copy_from(rhs.root(), header_);
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }

    // move constructor
    template <class T, class Compare>
    rb_tree<T, Compare>::rb_tree(rb_tree&& rhs) noexcept
        :header_(tinystl::move(rhs.header_)),
        node_count_(rhs.node_count_),
        key_comp_(rhs.key_comp_)
    {
        rhs.reset();
    }

    // copy assignment
    template <class T, class Compare>
    rb_tree<T, Compare>& rb_tree<T, Compare>::operator=(const rb_tree& rhs)
    {
        if (this != &rhs)
        {
            clear();

            if (rhs.node_count_ != 0)
            {
                root() = copy_from(rhs.root(), header_);
                leftmost() = rb_tree_min(root());
                rightmost() = rb_tree_max(root());
            }

            node_count_ = rhs.node_count_;
            key_comp_ = rhs.key_comp_;
        }
        return *this;
    }

    // move assignment
    template <class T, class Compare>
    rb_tree<T, Compare>& rb_tree<T, Compare>::operator=(rb_tree&& rhs)
    {
        clear();
        header_ = tinystl::move(rhs.header_);
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
        rhs.reset();
        return *this;
    }

    // Insert elements in place, key values allow duplicates
    template <class T, class Compare>
    template <class ...Args>
    typename rb_tree<T, Compare>::iterator rb_tree<T, Compare>::emplace_multi(Args&& ...args)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinystl::forward<Args>(args)...);
        auto res = get_insert_multi_pos(value_traits::get_key(np->value));
        return insert_node_at(res.first, np, res.second);
    }

    // Insert elements in place, key values don't allow duplicates
    template <class T, class Compare>
    template <class ...Args>
    tinystl::pair<typename rb_tree<T, Compare>::iterator, bool> 
    rb_tree<T, Compare>::emplace_unique(Args&& ...args)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinystl::forward<Args>(args)...);
        auto res = get_insert_unique_pos(value_traits::get_key(np->value));
        if (res.second)
        { 
            // successful insert
            return tinystl::make_pair(insert_node_at(res.first.first, np, res.first.second), true);
        }
        destroy_node(np);
        return tinystl::make_pair(iterator(res.first.first), false);
    }

    // Insert elements in place, and the key value is allowed to be repeated. 
    // When the hint position is close to the insertion position, 
    // the time complexity of the insertion operation can be reduced
    template <class T, class Compare>
    template <class ...Args>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::emplace_multi_use_hint(iterator hint, Args&& ...args)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinystl::forward<Args>(args)...);
        if (node_count_ == 0)
        {
            return insert_node_at(header_, np, true);
        }
        key_type key = value_traits::get_key(np->value);
        if (hint == begin())
        { 
            if (key_comp_(key, value_traits::get_key(*hint)))
            {
                return insert_node_at(hint.node, np, true);
            }
            else
            {
                auto pos = get_insert_multi_pos(key);
                return insert_node_at(pos.first, np, pos.second);
            }
        }
        else if (hint == end())
        { 
            if (!key_comp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value)))
            {
                return insert_node_at(rightmost(), np, false);
            }
            else
            {
                auto pos = get_insert_multi_pos(key);
                return insert_node_at(pos.first, np, pos.second);
            }
        }
        return insert_multi_use_hint(hint, key, np);
    }

    // Insert elements in place, and the key value is not allowed to be repeated. 
    // When the hint position is close to the insertion position, 
    // the time complexity of the insertion operation can be reduced
    template<class T, class Compare>
    template<class ...Args>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::emplace_unique_use_hint(iterator hint, Args&& ...args)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinystl::forward<Args>(args)...);
        if (node_count_ == 0)
        {
            return insert_node_at(header_, np, true);
        }
        key_type key = value_traits::get_key(np->value);
        if (hint == begin())
        { 
            if (key_comp_(key, value_traits::get_key(*hint)))
            {
                return insert_node_at(hint.node, np, true);
            }
            else
            {
                auto pos = get_insert_unique_pos(key);
                if (!pos.second)
                {
                    destroy_node(np);
                    return pos.first.first;
                }
                return insert_node_at(pos.first.first, np, pos.first.second);
            }
        }
        else if (hint == end())
        { 
            if (key_comp_(value_traits::get_key(rightmost()->get_node_ptr()->value), key))
            {
                return insert_node_at(rightmost(), np, false);
            }
            else
            {
                auto pos = get_insert_unique_pos(key);
                if (!pos.second)
                {
                    destroy_node(np);
                    return pos.first.first;
                }
                return insert_node_at(pos.first.first, np, pos.first.second);
            }
        }
        return insert_unique_use_hint(hint, key, np);
    }

    // Insert element, node key value allows duplication
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::insert_multi(const value_type& value)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        auto res = get_insert_multi_pos(value_traits::get_key(value));
        return insert_value_at(res.first, value, res.second);
    }

    // Insert a new value, the node key value is not allowed to be repeated, return a pair, 
    // if the insertion is successful, the second parameter of the pair is true, 
    // otherwise it is false
    template <class T, class Compare>
    tinystl::pair<typename rb_tree<T, Compare>::iterator, bool>
    rb_tree<T, Compare>::insert_unique(const value_type& value)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        auto res = get_insert_unique_pos(value_traits::get_key(value));
        if (res.second)
        { 
            return tinystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
        }
        return tinystl::make_pair(res.first.first, false);
    }

    // Delete the node at the hint position
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator rb_tree<T, Compare>::erase(iterator hint)
    {
        auto node = hint.node->get_node_ptr();
        iterator next(node);
        ++next;
        
        rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
        destroy_node(node);
        --node_count_;
        return next;
    }

    // Delete elements whose key value is equal to key, and return the number of deletions
    template <class T, class Compare>
    typename rb_tree<T, Compare>::size_type
    rb_tree<T, Compare>::erase_multi(const key_type& key)
    {
        auto p = equal_range_multi(key);
        size_type n = tinystl::distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }

    // Delete elements whose key value is equal to key, and return the number of deletions
    template <class T, class Compare>
    typename rb_tree<T, Compare>::size_type
    rb_tree<T, Compare>::erase_unique(const key_type& key)
    {
        auto it = find(key);
        if (it != end())
        {
            erase(it);
            return 1;
        }
        return 0;
    }

    // Delete elements in [first, last)
    template <class T, class Compare>
    void rb_tree<T, Compare>::erase(iterator first, iterator last)
    {
        if (first == begin() && last == end())
        {
            clear();
        }
        else
        {
            while (first != last)
            erase(first++);
        }
    }

    // clear rb tree
    template <class T, class Compare>
    void rb_tree<T, Compare>::clear()
    {
        if (node_count_ != 0)
        {   
            // delete first and then update
            erase_since(root());
            leftmost() = header_;
            root() = nullptr;
            rightmost() = header_;
            node_count_ = 0;
        }
    }

    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::find(const key_type& key)
    {
        auto y = header_;  // The last node not less than key
        auto x = root();
        while (x != nullptr)
        {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
            { 
                y = x, x = x->left;
            }
            else
            { 
                x = x->right;
            }
        }
        iterator j = iterator(y);
        return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
    }

    template <class T, class Compare>
    typename rb_tree<T, Compare>::const_iterator
    rb_tree<T, Compare>::find(const key_type& key) const
    {
        auto y = header_;  
        auto x = root();
        while (x != nullptr)
        {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
            { 
                y = x, x = x->left;
            }
            else
            { 
                x = x->right;
            }
        }
        const_iterator j = const_iterator(y);
        return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
    }

    // The first position where the key value is not less than key
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::lower_bound(const key_type& key)
    {
        auto y = header_;
        auto x = root();
        while (x != nullptr)
        {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
            { 
                // key <= x
                y = x, x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        return iterator(y);
    }

    template <class T, class Compare>
    typename rb_tree<T, Compare>::const_iterator
    rb_tree<T, Compare>::lower_bound(const key_type& key) const
    {
        auto y = header_;
        auto x = root();
        while (x != nullptr)
        {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
            { 
                // key <= x
                y = x, x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        return const_iterator(y);
    }

    // The last position where the key value is not less than key
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::upper_bound(const key_type& key)
    {
        auto y = header_;
        auto x = root();
        while (x != nullptr)
        {
            if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value)))
            { 
                // key < x
                y = x, x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        return iterator(y);
    }

    template <class T, class Compare>
    typename rb_tree<T, Compare>::const_iterator
    rb_tree<T, Compare>::upper_bound(const key_type& key) const
    {
        auto y = header_;
        auto x = root();
        while (x != nullptr)
        {
            if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value)))
            { 
                // key < x
                y = x, x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        return const_iterator(y);
    }

    // swap rb tree
    template <class T, class Compare>
    void rb_tree<T, Compare>::swap(rb_tree& rhs) noexcept
    {
        if (this != &rhs)
        {
            tinystl::swap(header_, rhs.header_);
            tinystl::swap(node_count_, rhs.node_count_);
            tinystl::swap(key_comp_, rhs.key_comp_);
        }
    }

    //=====================================================================================
    // helper function
    template <class T, class Compare>
    template <class ...Args>
    typename rb_tree<T, Compare>::node_ptr
    rb_tree<T, Compare>::create_node(Args&&... args)
    {
        auto tmp = node_allocator::allocate(1);
        try
        {
            data_allocator::construct(tinystl::address_of(tmp->value), tinystl::forward<Args>(args)...);
            tmp->left = nullptr;
            tmp->right = nullptr;
            tmp->parent = nullptr;
        }
        catch (...)
        {
            node_allocator::deallocate(tmp);
            throw;
        }
        return tmp;
    }

    template <class T, class Compare>
    typename rb_tree<T, Compare>::node_ptr
    rb_tree<T, Compare>::clone_node(base_ptr x)
    {
        node_ptr tmp = create_node(x->get_node_ptr()->value);
        tmp->color = x->color;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    template <class T, class Compare>
    void rb_tree<T, Compare>::destroy_node(node_ptr p)
    {
        data_allocator::destroy(&p->value);
        node_allocator::deallocate(p);
    }

    // constructor helper function
    template <class T, class Compare>
    void rb_tree<T, Compare>::rb_tree_init()
    {
        header_ = base_allocator::allocate(1);
        header_->color = rb_tree_red;  // header_ 's color is red, different form root
        root() = nullptr;
        leftmost() = header_;
        rightmost() = header_;
        node_count_ = 0;
    }

    template <class T, class Compare>
    void rb_tree<T, Compare>::reset()
    {
        header_ = nullptr;
        node_count_ = 0;
    }

    template <class T, class Compare>
    tinystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>
    rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key)
    {
        auto x = root();
        auto y = header_;
        bool add_to_left = true;
        while (x != nullptr)
        {
            y = x;
            add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
            x = add_to_left ? x->left : x->right;
        }
        return tinystl::make_pair(y, add_to_left);
    }

    template <class T, class Compare>
    tinystl::pair<tinystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
    rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key)
    { 
        auto x = root();
        auto y = header_;
        bool add_to_left = true;
        while (x != nullptr)
        {
            y = x;
            add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
            x = add_to_left ? x->left : x->right;
        }
        iterator j = iterator(y);  
        if (add_to_left)
        {
            if (y == header_ || j == begin())
            {
                return tinystl::make_pair(tinystl::make_pair(y, true), true);
            }
            else
            { 
                --j;
            }
        }
        if (key_comp_(value_traits::get_key(*j), key))  
        { 
            return tinystl::make_pair(tinystl::make_pair(y, add_to_left), true);
        }
        return tinystl::make_pair(tinystl::make_pair(y, add_to_left), false);
    }

    // first parameter:  the parent node of the insertion point, 
    // second parameter: value is the value to be inserted, 
    // third parameter:  add_to_left indicates whether to insert on the left
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::insert_value_at(base_ptr x, const value_type& value, 
                                         bool add_to_left)
    {
        node_ptr node = create_node(value);
        node->parent = x;
        auto base_node = node->get_base_ptr();
        if (x == header_)
        {
            root() = base_node;
            leftmost() = base_node;
            rightmost() = base_node;
        }
        else if (add_to_left)
        {
            x->left = base_node;
            if (leftmost() == x)
                leftmost() = base_node;
        }
        else
        {
            x->right = base_node;
            if (rightmost() == x)
                rightmost() = base_node;
        }
        rb_tree_insert_rebalance(base_node, root());
        ++node_count_;
        return iterator(node);
    }
    
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator
    rb_tree<T, Compare>::insert_node_at(base_ptr x, node_ptr node, bool add_to_left)
    {
        node->parent = x;
        auto base_node = node->get_base_ptr();
        if (x == header_)
        {
            root() = base_node;
            leftmost() = base_node;
            rightmost() = base_node;
        }
        else if (add_to_left)
        {
            x->left = base_node;
            if (leftmost() == x)
                leftmost() = base_node;
        }
        else
        {
            x->right = base_node;
            if (rightmost() == x)
                rightmost() = base_node;
        }
        rb_tree_insert_rebalance(base_node, root());
        ++node_count_;
        return iterator(node);
    }

    // Insert an element, the key value is allowed to be repeated, use hint
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator 
    rb_tree<T, Compare>::insert_multi_use_hint(iterator hint, key_type key, node_ptr node)
    {
        // Look for insertable locations near hint
        auto np = hint.node;
        auto before = hint;
        --before;
        auto bnp = before.node;
        if (!key_comp_(key, value_traits::get_key(*before)) &&
            !key_comp_(value_traits::get_key(*hint), key))
        { 
            // before <= node <= hint
            if (bnp->right == nullptr)
            {
                return insert_node_at(bnp, node, false);
            }
            else if (np->left == nullptr)
            {
                return insert_node_at(np, node, true);
            }
        }
        auto pos = get_insert_multi_pos(key);
        return insert_node_at(pos.first, node, pos.second);
    }

    // Insert elements, the key value is not allowed to be repeated, use hint
    template <class T, class Compare>
    typename rb_tree<T, Compare>::iterator 
    rb_tree<T, Compare>::insert_unique_use_hint(iterator hint, key_type key, node_ptr node)
    {
        // Look for insertable locations near hint
        auto np = hint.node;
        auto before = hint;
        --before;
        auto bnp = before.node;
        if (key_comp_(value_traits::get_key(*before), key) &&
            key_comp_(key, value_traits::get_key(*hint)))
        { 
            // before < node < hint
            if (bnp->right == nullptr)
            {
                return insert_node_at(bnp, node, false);
            }
            else if (np->left == nullptr)
            {
                return insert_node_at(np, node, true);
            }
        }
        auto pos = get_insert_unique_pos(key);
        if (!pos.second)
        {
            destroy_node(node);
            return pos.first.first;
        }
        return insert_node_at(pos.first.first, node, pos.first.second);
    }

    // Recursively copy a tree, the node starts from x, and p is the parent node of x
    template <class T, class Compare>
    typename rb_tree<T, Compare>::base_ptr
    rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p)
    {
        auto top = clone_node(x);
        top->parent = p;
        try
        {
            if (x->right)
                top->right = copy_from(x->right, top);
            p = top;
            x = x->left;
            while (x != nullptr)
            {
                auto y = clone_node(x);
                p->left = y;
                y->parent = p;
                if (x->right)
                    y->right = copy_from(x->right, y);
                p = y;
                x = x->left;
            }
        }
        catch (...)
        {
            erase_since(top);
            throw;
        }
        return top;
    }

    // erase_since
    // deleting the node and its subtrees starts at node x 
    template <class T, class Compare>
    void rb_tree<T, Compare>::erase_since(base_ptr x)
    {
        while (x != nullptr)
        {
            erase_since(x->right);
            auto y = x->left;
            destroy_node(x->get_node_ptr());
            x = y;
        }
    }

    // overload comparison operations
    template <class T, class Compare>
    bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
    {
        return lhs.size() == rhs.size() && tinystl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class T, class Compare>
    bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
    {
        return tinystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class T, class Compare>
    bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T, class Compare>
    bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
    {
        return rhs < lhs;
    }

    template <class T, class Compare>
    bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T, class Compare>
    bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
    {
        return !(lhs < rhs);
    }

    // swap
    template <class T, class Compare>
    void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

} // namespace tinystl
#endif 