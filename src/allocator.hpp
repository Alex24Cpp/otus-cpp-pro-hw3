#pragma once

#include <cstddef>
#include <new>
#include <stdexcept>

template <typename T, std::size_t N>
class FixedAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

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
        if (count > N || allocated_count + count > N) {
            throw std::bad_alloc();  // Превышение лимита
        }
        if (!memory_block) {
            // Выделить единый блок на N элементов c выравниванием памяти
            memory_block = reinterpret_cast<pointer>(::operator new(
                N * sizeof(value_type),
                static_cast<std::align_val_t>(alignof(value_type))));
            allocated_count = 0;
        }
        pointer result = memory_block + allocated_count;
        allocated_count += count;
        return result;
    }

    // Освобождение (не делаать ничего для отдельных элементов)
    // память освобождается только в деструкторе
    void deallocate(pointer, size_type) noexcept {
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
private:
    pointer memory_block = nullptr;  // Единый блок памяти
    size_type allocated_count{0};    // Сколько уже выделено
};
