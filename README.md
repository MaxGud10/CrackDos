# Взлом программы

### Описание взламываемой программы

Программа написана на языке ассемблера под архитектуру x86 и предназначается под использование на ПК под управлением DOS

### Подготовка к взлому

В первую очередь необходимо установить среду операционной системы DOS.
Мы будем использовать программу [DosBox](https://ru.wikipedia.org/wiki/DOSBox).

> [!Note]
> Если вы используете ОС Linux и у вас не получилось установить версию DosBox для вашей системы, то скачайте и установите [wine](https://wiki.archlinux.org/title/Wine_(%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9)).
Это нужно для корректного запуска Windows приложений на Linux

##### Версия для Windows
Для этого нужно перейти по ссылке http://ded32.net.ru/

В правой части сайта найти ссылку: `Скачать DosBox + Asm`

![image](https://github.com/user-attachments/assets/a5c8ea6d-595a-4f55-b323-83f5cf5ba810)


Затем следовать инструкциям установщика.

### Взлом через переполнение буфера ввода

В первую очередь стоит отметить, что будет являться корректным взломом без потери функциональности программы. 
- Программа должна вывести приветственное сообщение `Enter password:`
- Запросить ввод пароля (пользователь должен ввести комбинацию с нажатием Enter на конце)
- Программа должна вывести сообщение `Password correct`


Проанализировав код программы можно прийти к выводу, что количество символв, вводимое с клавиатуры никак не контролируется, а значит можно выйти за пределы буфера ввода и переписать данные, которые располагаются на ним.

Рассмотрим побайтную структуру буфера ввода:
![image](https://github.com/user-attachments/assets/1cb550a2-1b5d-41b9-b1dd-75f7bde96e7d)

Как видно на картинке буфер (подчеркнутый красный) 16 байт, а значит можно ввести 15 символов и один под Enter


![image](https://github.com/user-attachments/assets/f89ecef4-cde1-416c-b46d-38d843789cdd)



Сразу после буфера у нас идут байты под хэш пароля с которым потом будет сраниваться хэш введный пользопателем (выделено красным цветом). Так же заметим, что "функциональная длина" пароля равна 5, то есть только у 5 первых чисел  булет высчитываться хэш (выделено жёлтым цветом)

Попробуем перезаписать хэш. Для этого напишем программу с таким же алгоритмом подсчета хэша, только для нашего пароля 
![image](https://github.com/user-attachments/assets/64ce4f93-f9df-45dc-8a7d-c7c532978b58)

Где ```hello``` наш пароль, а ```214``` наш хэш который надо будет вставить после перезаполнения фубера

Введем в консоль пароль
```
hello0000000000214
```

![image](https://github.com/user-attachments/assets/a37a7e18-7c73-4683-ae75-2b04668ce48d)
Первая уязвимость найдена 

### Колизия хэша

Попытаемся понять каквысчитывается хэш введенного пароля
    ![image](https://github.com/user-attachments/assets/fb442f04-db93-45d3-be29-2b58fdff73d7)

Нетрудно понять, что хэш считается как сумма аски кодов символов, также не турдно понять, что хэш равен 20A, если посмотреть что лежит после буфера
Найдем такое слов которое имеет такой же хэш, как 20A
к примеру ```dedfw```

 ![image](https://github.com/user-attachments/assets/0a779e56-93c1-45b1-868f-68d5e5551a11)
 Вторая уязвимость найдена 


### Взлом через модификацию бинарного файла


![image](https://github.com/user-attachments/assets/af28d2d3-ceb1-4a87-a489-e7c2f16e0c2b)

Проанализировав graph dump можем заметить, что данный jnz решает куда дальше пойжет прога -> либо выведет ```Well done, Max```, либо ```Wrong password, try again!```
Поэтому напишем patcher, который поменяет в исхожном файле это место на ```nop```


> [!Important] 
> Нужно установить библиотеку SFML 2.6

![gif](crackme/animation)
