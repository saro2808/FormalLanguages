# Формальные языки
## Практикум 1
### Арутюнян Саро, Б05-951
`id = 2`

**Задача.** Даны регулярное выражение `α` и натуральные числа `k, l`, такие что `0` ≤ `l` < `k`. Вывести минимальное число `n`, равное `l` по модулю `k`, такое что язык `L` содержит слова длины `n`.

**Решение.** Определим так называемое состояние, под которой хранится хеш-таблица, ключи которой — остатки от деления на `k`, а значения — длины кратчайших слов из языка, равные соответствующим ключам по модулю `k`.

Обновление состояний происходит следующим образом. Мы обрабатываем очередной символ регулярного выражения, и если он
* буква, получаем состояние `{1, 1}`;
* 1, получаем состояние `{0, 0}`;
* +, берем минимальное из значений, соответствующих данному ключу;
* ., сложим остатки первого и второго состояний по модулю `k` и сложим длины;
* \*, сведем обработку звезды к решению задачи о рюкзаке, поскольку у нас но более, чем `k` чисел, причем остатки — весы предметов, а значения — стоимости.

В конце концов мы получаем минимальные длины для всех остатков в терминальном состоянии, поскольку мы рассматриваем кратчайшие слова для каждого остатка, и рассматриваем все такие слова.

Сложность алгоритма — `O(nk^2)`, поскольку у нас `n` символов и на обработку звезды тратим `O(k^2)` из-за использования динамического программирования.
