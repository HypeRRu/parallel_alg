#ifndef THREADS_TASKS_H
#define THREADS_TASKS_H

#include <thread>
#include <atomic>
#include <vector>

#include "common/matrix.h"


namespace threads
{

class Tasks
{
public:
    /// @brief Конструктор менеджера потоков.
    /// @param[in] parallelMultiply Использовать параллельный алгоритм умножения.
    Tasks( bool parallelMultiply = false );

    /// @brief Валидность программы.
    bool isValid() const;

    /// @brief Поток чтения матриц.
    /// @param[out] lhs Первая считываемая матрица.
    /// @param[out] rhs Вторая считываемая матрица.
    void readMatrices( common::Matrix& lhs, common::Matrix& rhs );

    /// @brief Поток умножения матриц. Умножение последовательное (в одном потоке).
    /// @param[in] lhs Левая умножаемая матрица.
    /// @param[in] rhs Правая умножаемая матрица.
    /// @param[out] result Результирующая матрица.
    void multiplyMatricesSerial( const common::Matrix& lhs, const common::Matrix& rhs, common::Matrix& result );

    /// @brief Поток умножения матриц. Умножение параллельное (создается поток для каждого результирующего элемента).
    /// @param[in] lhs Левая умножаемая матрица.
    /// @param[in] rhs Правая умножаемая матрица.
    /// @param[out] result Результирующая матрица.
    void multiplyMatricesParallel( const common::Matrix& lhs, const common::Matrix& rhs, common::Matrix& result );

    /// @brief Поток записи результирующей матрицы.
    /// @param[in] result Результирующая матрица.
    void writeResultMatrix( const common::Matrix& result );

private:
    /// @brief Атомарное изменение isValid_ на false
    void invalidate();

    /// @brief Запись в атомарную bool переменную значения.
    /// @param[in] atomic Атомарная переменная.
    /// @param[in] value Записываемое значение.
    void storeAtomicBool( std::atomic< bool >& atomic, bool value );

    /// @brief Ожидание значения в атомарной переменной.
    /// @param[in] atomic Атомарная переменная.
    /// @param[in] value Ожидаемое значение.
    void waitForAtomicBool( const std::atomic< bool >& atomic, bool value );

private:
    common::Matrix              lhs_;               ///< Левая умножаемая матрица.
    common::Matrix              rhs_;               ///< Правая умножаемая матрица.
    common::Matrix              result_;            ///< Результирующая матрица.

    std::vector< std::thread >  threads_;           ///< Рабочие потоки.

    std::atomic< bool > isValid_;                   ///< Валидность программы.
    std::atomic< bool > calcAllowed_;               ///< Разрешение на чтение данных для второго потока.
    std::atomic< bool > writeAllowed_;              ///< Разрешение на чтение данных для третьего потока.

}; // class Tasks

}; // namespace threads

#endif // THREADS_TASKS_H
