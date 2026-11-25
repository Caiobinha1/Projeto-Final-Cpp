#ifndef UTILITARIOS_TEMPLATE_H
#define UTILITARIOS_TEMPLATE_H

#include <vector>
#include <numeric>

/**
 * @brief Funções Template utilitárias do sistema (Semana 7 - EEL7323).
 *
 * Complementam a classe template FilaEncadeada<T> demonstrando programação
 * genérica de funções com dedução automática de tipo em tempo de compilação.
 */

/**
 * @brief Restringe um valor numérico ao intervalo fechado [minimo, maximo].
 * @tparam T Tipo numérico comparável (int, double, float, etc.).
 */
template <typename T>
T limitarIntervalo(T valor, T minimo, T maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

/**
 * @brief Calcula a média aritmética de uma coleção genérica (STL std::vector<T>).
 * @tparam T Tipo numérico armazenado no vetor.
 */
template <typename T>
double calcularMedia(const std::vector<T>& valores) {
    if (valores.empty()) {
        return 0.0;
    }
    double soma = std::accumulate(valores.begin(), valores.end(), 0.0);
    return soma / static_cast<double>(valores.size());
}

/**
 * @brief Retorna o maior entre dois valores de tipo genérico T.
 */
template <typename T>
T encontrarMaximo(const T& a, const T& b) {
    return (a > b) ? a : b;
}

#endif // UTILITARIOS_TEMPLATE_H
