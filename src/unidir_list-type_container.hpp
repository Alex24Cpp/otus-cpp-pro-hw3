#pragma once

#include <cstddef>
#include <memory>

template <typename T>
struct MyUniDirNode {
    MyUniDirNode() = default;
    MyUniDirNode(T v) : m_data{v} {
    }
    MyUniDirNode* m_next{nullptr};
    T m_data{0};
};

template <typename T, typename Allocator = std::allocator<T>>
class MyUniDirListTypeContainer {
public:
    MyUniDirListTypeContainer() = default;
    MyUniDirListTypeContainer(const MyUniDirListTypeContainer& mlc);
    MyUniDirListTypeContainer(MyUniDirListTypeContainer&& mlc);
    ~MyUniDirListTypeContainer();
    MyUniDirListTypeContainer& operator=(const MyUniDirListTypeContainer& mlc);
    MyUniDirListTypeContainer& operator=(MyUniDirListTypeContainer&& mlc);
    void push_back(T value);
    void push_front(T value);
    int insert(T value, size_t index);
    int erase(size_t first, size_t last);
    int erase(size_t index);
    size_t size() const;
    T operator[](size_t index) const;
    void clear();
    // Добавлено для параметризации аллокатором
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;

    using node_type = MyUniDirNode<T>;
    using node_allocator_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<node_type>;
    using node_allocator_traits = std::allocator_traits<node_allocator_type>;
private:
    node_type* m_head{nullptr};
    node_type* m_tail{nullptr};
    size_type m_size{0};
    node_allocator_type
        m_node_allocator{};  // Добавлено для параметризации аллокатором
    void free_up_memory();
    // Добавлено для параметризации аллокатором
    node_type* createNode(const T& value) {
        node_type* node = node_allocator_traits::allocate(m_node_allocator, 1U);
        try {
            node_allocator_traits::construct(m_node_allocator, node, value);
        } catch (...) {
            node_allocator_traits::deallocate(m_node_allocator, node, 1U);
            throw;
        }
        return node;
    }

    void destroyNode(node_type* node) {
        node_allocator_traits::destroy(m_node_allocator, node);
        node_allocator_traits::deallocate(m_node_allocator, node, 1U);
    }
};

template <typename T, typename Allocator>
MyUniDirListTypeContainer<T, Allocator>::MyUniDirListTypeContainer(
    const MyUniDirListTypeContainer& mlc)
    : m_node_allocator(mlc.m_node_allocator) {
    if (mlc.m_size != 0) {
        node_type* prev_new_node = nullptr;
        for (node_type* temp = mlc.m_head; temp != nullptr;
             temp = temp->m_next) {
            node_type* new_node = createNode(temp->m_data);
            if (temp == mlc.m_head) {
                m_head = new_node;
            }
            if (temp->m_next == nullptr) {
                m_tail = new_node;
            }
            if (prev_new_node != nullptr) {
                prev_new_node->m_next = new_node;
            }
            prev_new_node = new_node;
            m_size++;
        }
    }
}

template <typename T, typename Allocator>
MyUniDirListTypeContainer<T, Allocator>::MyUniDirListTypeContainer(
    MyUniDirListTypeContainer&& mlc)
    : m_head(mlc.m_head),
      m_tail(mlc.m_tail),
      m_size(mlc.m_size),
      m_node_allocator(std::move(mlc.m_node_allocator)) {
    mlc.m_head = nullptr;
    mlc.m_tail = nullptr;
    mlc.m_size = 0;
}

template <typename T, typename Allocator>
MyUniDirListTypeContainer<T, Allocator>::~MyUniDirListTypeContainer() {
    free_up_memory();
}

template <typename T, typename Allocator>
MyUniDirListTypeContainer<T, Allocator>&
MyUniDirListTypeContainer<T, Allocator>::operator=(
    const MyUniDirListTypeContainer& mlc) {
    free_up_memory();
    m_size = 0;
    if (mlc.m_size != 0) {
        node_type* prev_new_node = nullptr;
        for (node_type* temp = mlc.m_head; temp != nullptr;
             temp = temp->m_next) {
            node_type* new_node = createNode(temp->m_data);
            if (temp == mlc.m_head) {
                m_head = new_node;
            }
            if (temp->m_next == nullptr) {
                m_tail = new_node;
            }
            if (prev_new_node != nullptr) {
                prev_new_node->m_next = new_node;
            }
            prev_new_node = new_node;
            m_size++;
        }
    } else {
        m_head = nullptr;
        m_tail = nullptr;
    }
    return *this;
}

template <typename T, typename Allocator>
MyUniDirListTypeContainer<T, Allocator>&
MyUniDirListTypeContainer<T, Allocator>::operator=(
    MyUniDirListTypeContainer&& mlc) {
    free_up_memory();
    m_head = mlc.m_head;
    m_tail = mlc.m_tail;
    m_size = mlc.m_size;
    m_node_allocator = std::move(mlc.m_node_allocator);
    mlc.m_head = nullptr;
    mlc.m_tail = nullptr;
    mlc.m_size = 0;
    return *this;
}

template <typename T, typename Allocator>
void MyUniDirListTypeContainer<T, Allocator>::push_back(T value) {
    node_type* new_node = createNode(value);
    if (m_head == nullptr) {
        m_head = new_node;
    }
    if (m_tail != nullptr) {
        m_tail->m_next = new_node;
    }
    m_tail = new_node;
    ++m_size;
}

template <typename T, typename Allocator>
void MyUniDirListTypeContainer<T, Allocator>::push_front(T value) {
    node_type* new_node = createNode(value);
    if (m_tail == nullptr) {
        m_tail = new_node;
    } else {
        new_node->m_next = m_head;
    }
    m_head = new_node;
    ++m_size;
}

template <typename T, typename Allocator>
int MyUniDirListTypeContainer<T, Allocator>::insert(T value, size_t index) {
    if (index >= m_size) {
        return -1;
    }
    node_type* new_node = createNode(value);
    node_type* node = m_head;
    if (index == 0) {
        m_head = new_node;
        new_node->m_next = node;
    } else {
        for (size_t i = 0; i < index - 1 || i == 1; ++i) {
            node = node->m_next;
        }
        new_node->m_next = node->m_next;
        node->m_next = new_node;
    }

    ++m_size;
    return 0;
}

template <typename T, typename Allocator>
int MyUniDirListTypeContainer<T, Allocator>::erase(size_t index) {
    if (index >= m_size) {
        return -1;
    } else {
        node_type* nodeDel{nullptr};
        node_type* preNode{nullptr};
        nodeDel = m_head;
        if (index == 0) {
            m_head = m_head->m_next;
        } else {
            for (size_t i = 0; i <= index - 1; ++i) {
                preNode = nodeDel;
                nodeDel = nodeDel->m_next;
            }
            preNode->m_next = nodeDel->m_next;
        }
        destroyNode(nodeDel);
    }
    --m_size;
    return 0;
}

template <typename T, typename Allocator>
int MyUniDirListTypeContainer<T, Allocator>::erase(size_t first, size_t last) {
    if (first >= m_size || last >= m_size || first > last) {
        return -1;
    } else {
        node_type* nodeDelStart{nullptr};
        node_type* nodeDelEnd = m_head;
        node_type* preNode = m_head;
        node_type* afterNode{nullptr};
        if (first == 0) {
            if (m_head == m_tail) {
                delete m_head;
                m_head = m_tail = {nullptr};
                return 0;
            }
            nodeDelStart = m_head;
        } else {
            for (size_t i = 0; i < first - 1 || i == 1; ++i) {
                preNode = preNode->m_next;
            }
            nodeDelStart = preNode->m_next;
        }

        if (last == m_size - 1) {
            for (size_t i = 0; i < last || i == 1; ++i) {
                nodeDelEnd = nodeDelEnd->m_next;
            }
            nodeDelEnd = m_tail;
            afterNode = nullptr;
            m_tail = preNode;
        } else {
            for (size_t i = 0; i < last || i == 1; ++i) {
                nodeDelEnd = nodeDelEnd->m_next;
            }
            afterNode = nodeDelEnd->m_next;
        }
        if (first == 0) {
            m_head = afterNode;
        } else {
            preNode->m_next = afterNode;
        }
        for (size_t i = 0; i <= last - first; ++i) {
            nodeDelEnd = nodeDelStart->m_next;
            destroyNode(nodeDelStart);
            nodeDelStart = nodeDelEnd;
        }
        m_size -= last - first + 1;
    }
    return 0;
}

template <typename T, typename Allocator>
size_t MyUniDirListTypeContainer<T, Allocator>::size() const {
    return m_size;
}

template <typename T, typename Allocator>
T MyUniDirListTypeContainer<T, Allocator>::operator[](size_t index) const {
    node_type* node = m_head;
    if (index != 0) {
        for (size_t i = 0; i < index; ++i) {
            node = node->m_next;
        }
    }
    return node->m_data;
}

template <typename T, typename Allocator>
void MyUniDirListTypeContainer<T, Allocator>::clear() {
    free_up_memory();
    m_head = nullptr;
    m_tail = nullptr;
    m_size = 0;
}

template <typename T, typename Allocator>
void MyUniDirListTypeContainer<T, Allocator>::free_up_memory() {
    if (m_head != nullptr && m_tail != nullptr) {
        for (node_type* temp; m_head != nullptr; m_head = temp) {
            temp = m_head->m_next;
            destroyNode(m_head);
        }
    }
}
