#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <new>
#include <utility>

#include "allocator.hpp"

// Тип ключа и значения
using KeyType = int;
using ValueType = int;

constexpr std::size_t POOL_SIZE = 10;

// Аллокатор на 10 элементов
using MyAllocator =
    FixedAllocator<std::pair<const KeyType, ValueType>, POOL_SIZE>;

// Тип map с моим аллокатором
using MyMap = std::map<KeyType, ValueType, std::less<KeyType>, MyAllocator>;

// map с аллокатором по умолчанию
using StdMap = std::map<KeyType, ValueType>;

// --- Вспомогательные функции ---

// Функция вычисления факториала
constexpr ValueType factorial(int number) {
    ValueType result = 1;
    for (int i = 2; i <= number; ++i) {
        result *= i;
    }
    return result;
}

// Функция вывода содержимого map
template <typename MapType>
void printMapContents(const MapType& map) {
    for (const auto& pair : map) {
        std::cout << pair.first << " " << pair.second << "\n";
    }
}

int main() {
    try {
        StdMap std_map;
        // Заполнение 10 элементами: ключ — число от 0 до 9, значение —
        // факториал ключа
        for (int i = 0; i < 10; ++i) {
            std_map[i] = factorial(i);
        }

        MyMap my_map;
        // Заполнение 10 элементами: ключ — число от 0 до 9, значение —
        // факториал ключа
        for (int i = 0; i < 10; ++i) {
            my_map[i] = factorial(i);
        }

        // Выводим содержимое контейнеров
        std::cout << "StdMap (std::map со стандартным аллокатором):\n";
        printMapContents(std_map);

        std::cout << "\nMyMap (std::map с моим аллокатором):\n";
        printMapContents(my_map);

    } catch (const std::bad_alloc& e) {
        std::cerr << "Ошибка выделения памяти, превышено количество элементов: "
                  << e.what() << "\n";
        return 1;
    }

    return 0;
}