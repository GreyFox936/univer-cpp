# Прайс-лист (вариант 10)

## Состав:
README.md - описание и инструкции
price_list - бинарник
main.cpp - исходный код
price.csv - файл с вводными данными 


## Сборка
```bash
g++ -std=c++17 -O2 main.cpp -o price_list
```

## Запуск
```bash
./price_list
```

Исходные данные читаются из `products.csv` в том же каталоге.

Формат файла:  
`id;name;type;basePrice;discount`
