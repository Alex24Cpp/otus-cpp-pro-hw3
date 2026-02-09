#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <new>
#include <utility>

#include "allocator.hpp"
#include "unidir_list-type_container.hpp"

// Тип ключа и значения
using KeyType = int;
using ValueType = int;

constexpr std::size_t POOL_SIZE = 10;
constexpr int ELEMENTS_NUMBER = 10;

// Аллокатор на 10 элементов
using MyAllocator =
    FixedAllocator<std::pair<const KeyType, ValueType>, POOL_SIZE>;

// Тип map с моим аллокатором
using MyMap = std::map<KeyType, ValueType, std::less<KeyType>, MyAllocator>;

// map с аллокатором по умолчанию
using StdMap = std::map<KeyType, ValueType>;

// Мой контейнер спискового типа (однонаправленный список)
using MyListStd = MyUniDirListTypeContainer<int>;

// Мой контейнер спискового типа c моим аллокатром
using MyListAlloc = MyUniDirListTypeContainer<int, MyAllocator>;

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

// Функция вывода содержимого моего list контейнера
template <typename T>
void printMyContainer(const T& my_container) {
    for (size_t i = 0; i < my_container.size(); i++) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << my_container[i];
    }
    std::cout << "\n";
}

template <typename T>
void printMyContainerIt(const T& my_container) {
    for (const auto& elemet : my_container) {
        std::cout << elemet << " ";
    }
    std::cout << "\n";
}

int main() {
    try {
        StdMap std_map;
        // Заполнение 10 элементами: ключ — число от 0 до 9, значение —
        // факториал ключа
        for (int i = 0; i < ELEMENTS_NUMBER; ++i) {
            std_map[i] = factorial(i);
        }

        MyMap my_map;
        // Заполнение 10 элементами: ключ — число от 0 до 9, значение —
        // факториал ключа
        for (int i = 0; i < ELEMENTS_NUMBER; ++i) {
            my_map[i] = factorial(i);
        }

        MyListStd my_list;
        // Заполнение 10 элементами — int
        for (int i = 0; i < ELEMENTS_NUMBER; ++i) {
            my_list.push_back(i);
        }

        MyListAlloc my_list_alloc;
        // Заполнение 10 элементами — int
        for (int i = 0; i < ELEMENTS_NUMBER; ++i) {
            my_list_alloc.push_back(i);
        }

        // Выводим содержимое контейнеров
        std::cout << "StdMap (std::map со стандартным аллокатором):\n";
        printMapContents(std_map);

        std::cout << "\nMyMap (std::map с моим аллокатором):\n";
        printMapContents(my_map);

        std::cout << "\nMyList (мой контейнер со стандартным аллокатором):\n";
        printMyContainer(my_list);

        std::cout << "\nMyListAlloc (мой контейнер с моим аллокатором):\n";
        printMyContainerIt(my_list_alloc);

    } catch (const std::bad_alloc& e) {
        std::cerr << "Ошибка выделения памяти, превышено количество элементов: "
                  << e.what() << "\n";
        return 1;
    }

    return 0;
}