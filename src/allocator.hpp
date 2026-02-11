#pragma once

#include <cstddef>
#include <new>
#include <stdexcept>

template <typename T, std::size_t N>
class FixedAllocator {
public:
    using value_type = T;
    using pointer = T*;              // (deprecated in C++17)(removed in C++20)
    using const_pointer = const T*;  // (deprecated in C++17)(removed in C++20)
    using reference = T&;            // (deprecated in C++17)(removed in C++20)
    using const_reference =
        const T&;  // (deprecated in C++17)(removed in C++20)
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

    using is_always_equal = std::false_type;

    // rebind для STL-совместимости
    template <class U>
    struct rebind {
        using other = FixedAllocator<U, N>;
        using value_type = U;  // rebind меняет value_type
    };

    // Конструкторы/деструктор
    FixedAllocator() noexcept = default;
    FixedAllocator(const FixedAllocator&) noexcept = default;
    template <class U>
    FixedAllocator(const FixedAllocator<U, N>&) noexcept {
    }

    ~FixedAllocator() {
        // Освобождаем память при уничтожении аллокатора
        if (memory_block) {
            ::operator delete(memory_block);
        }
    }

    // Выделение памяти
    [[nodiscard]]
    auto allocate(size_type count) -> pointer {
        if (count == 0) {
            return nullptr;
        }
        if (count != 1) {  // аллокатор выделяет только по одному элементу
            throw std::bad_alloc();
        }

        if (!memory_block) {
            // Выделить единый блок на N элементов c выравниванием памяти
            memory_block = reinterpret_cast<pointer>(::operator new(
                N * sizeof(value_type),
                static_cast<std::align_val_t>(alignof(value_type))));

            allocated_count = 0;

            // Создать индексный free‑list
            // next_index[i] = i+1, последний = -1
            for (size_type i = 0; i + 1 < N; ++i) {
                next_index[i] = static_cast<int>(i + 1);
            }
            next_index[N - 1] = -1;  // конец списка
            free_head = 0;  // первый свободный индекс
        }

        // Проверка свободных слотов
        if (free_head == -1) {
            throw std::bad_alloc();  // свободных слотов нет
        }

        // Выделение слота по индексу
        const int index = free_head;
        free_head = next_index[index];  // сдвиг головы списка

        ++allocated_count;

        return memory_block + index;
    }

    // Память освобождается только в деструкторе
    // слот вернуть во free‑list
    void deallocate(pointer ptr, size_type count) noexcept {
        if (!ptr || count == 0) {
            return;
        }

        // вычисление индекса слота
        const size_type index = static_cast<size_type>(ptr - memory_block);

        // возврат индекса в начало списка свободных
        next_index[index] = free_head;
        free_head = static_cast<int>(index);

        if (allocated_count > 0) {
            --allocated_count;
        }
    }

    // Конструктор/деструктор
    template <class U, class... Args>
    void construct(U* ptr, Args&&... args) {
        ::new (ptr) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* ptr) {
        ptr->~U();
    }

    // Размер блока (для STL-совместимости)
    // максимальное число объектов типа T, которые может разместить аллокатор
    [[nodiscard]]
    auto max_size() const noexcept -> size_type {
        return N;
    }

    template <typename U, std::size_t M>
    friend class FixedAllocator;

    template <typename U, std::size_t M>
    friend auto operator==(const FixedAllocator&,
                           const FixedAllocator<U, M>&) noexcept -> bool;

    template <typename U, std::size_t M>
    friend auto operator!=(const FixedAllocator&,
                           const FixedAllocator<U, M>&) noexcept -> bool;
private:
    pointer memory_block = nullptr;  // Единый блок памяти
    size_type allocated_count{0};    // Сколько уже выделено

    // Индексный free‑list:
    // next_index[i] = индекс следующего свободного слота
    // next_index[i] = i+1, последний = -1
    int next_index[N]{};
    // Голова списка индексов свободных слотов
    int free_head{-1};
};

template <typename T, std::size_t N, typename U, std::size_t M>
auto operator==(const FixedAllocator<T, N>&,
                const FixedAllocator<U, M>&) noexcept -> bool {
    return N == M;
}

template <typename T, std::size_t N, typename U, std::size_t M>
auto operator!=(const FixedAllocator<T, N>& lhs,
                const FixedAllocator<U, M>& rhs) noexcept -> bool {
    return !(lhs == rhs);
}
