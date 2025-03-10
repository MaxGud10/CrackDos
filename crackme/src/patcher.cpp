#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdint> 

// Функция патчера
bool applyPatch(const std::string& filename) 
{
    FILE* file = fopen(filename.c_str(), "rb+");
    if (!file) 
    {
        std::cerr << "Error when opening a file: " << filename << std::endl;
        return false;
    }

    uint8_t buffer[2];
    size_t pos = 0;

    // Ищем последовательность 75 06
    while (fread(buffer, 1, 2, file) == 2) 
    {
        if (buffer[0] == 0x75 && buffer[1] == 0x06) 
        {
            // Нашли нужные байты, заменяем на 90 90
            fseek(file, pos, SEEK_SET);
            uint8_t nop[2] = {0x90, 0x90};
            fwrite(nop, 1, 2, file);
            fclose(file);
            return true; // Патч успешно применен
        }
        pos++;
        fseek(file, pos, SEEK_SET); // Передвигаем указатель для следующего чтения
    }

    fclose(file);
    std::cerr << "75 06 Not found in the file: " << filename << std::endl;
    return false; // Последовательность не найдена
}

int main() 
{
    // Создаем окно с увеличенной высотой
    sf::RenderWindow window(sf::VideoMode(600, 800), "Crack.NET", sf::Style::Close);

    // Загружаем шрифт
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) 
    {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
        return -1;
    }

    // Загружаем спрайт-лист для анимации
    sf::Texture spriteSheet;
    if (!spriteSheet.loadFromFile("dog.png")) 
    {
        std::cerr << "Ошибка загрузки спрайт-листа!" << std::endl;
        return -1;
    }

    // Параметры спрайт-листа
    const int columns = 6;               // Количество столбцов в спрайт-листе
    const int rows = 6;                  // Количество строк в спрайт-листе
    const int totalFrames = (columns - 1) * rows + 5; // Общее количество кадров (5x6 + 5 = 35)

    // Размеры одного кадра
    const int frameWidth = spriteSheet.getSize().x / columns;  // Ширина одного кадра
    const int frameHeight = spriteSheet.getSize().y / rows;    // Высота одного кадра

    // Создаем спрайт и устанавливаем текстуру
    sf::Sprite sprite(spriteSheet);
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight)); // Устанавливаем первый кадр

    // Поднимаем анимацию еще выше
    sprite.setPosition((window.getSize().x - frameWidth) / 2, 30);

    // Переменные для анимации
    int currentFrame = 0; // Текущий кадр
    sf::Clock animationClock; // Таймер для переключения кадров
    float frameDuration = 0.1f; // Длительность одного кадра в секундах (0.1 секунды)

    // Элементы интерфейса
    sf::RectangleShape inputBox(sf::Vector2f(400, 40)); // Уменьшил ширину рамки
    inputBox.setPosition((window.getSize().x - inputBox.getSize().x) / 2, 600); // Опустил еще ниже
    inputBox.setFillColor(sf::Color(30, 30, 30));
    inputBox.setOutlineThickness(2);

    sf::Text placeholder("Enter a path to file to crack...", font, 20);
    placeholder.setFillColor(sf::Color(150, 150, 150));
    placeholder.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);

    sf::RectangleShape crackButton(sf::Vector2f(100, 40));
    crackButton.setPosition((window.getSize().x - crackButton.getSize().x) / 2, 660); // Опустил еще ниже
    crackButton.setFillColor(sf::Color(30, 30, 30));
    crackButton.setOutlineThickness(2);

    sf::Text crackText("CRACK!", font, 20);
    crackText.setFillColor(sf::Color::White);
    crackText.setPosition(crackButton.getPosition().x + 20, crackButton.getPosition().y + 10);

    std::string inputText;
    sf::Text userInputText("", font, 20);
    userInputText.setFillColor(sf::Color::White);
    userInputText.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);

    sf::Text statusText("", font, 20); // Текст для отображения статуса
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition((window.getSize().x - 300) / 2, 710); // Опустил еще ниже

    bool inputActive = false;
    bool buttonHovered = false;
    float time = 0;

    // Основной цикл программы
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) 
            {
                if (inputBox.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) 
                {
                    inputActive = true;
                    statusText.setString(""); // Очищаем сообщение об ошибке при активации поля ввода
                } 
                else 
                {
                    inputActive = false;
                }

                if (crackButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) 
                {
                    // Проверяем, существует ли файл
                    std::string filename = inputText;
                    std::ifstream file(filename);
                    if (file.good()) 
                    {
                        // Файл найден, применяем патч
                        if (applyPatch(filename)) 
                        {
                            statusText.setString("[SUCCESS] file was cracked");
                            statusText.setFillColor(sf::Color::Green);
                        } 
                        else 
                        {
                            statusText.setString("[ERROR] patcher failed");
                            statusText.setFillColor(sf::Color::Red);
                        }
                    } 
                    else 
                    {
                        // Файл не найден, выводим ошибку
                        statusText.setString("[ERROR] file not found");
                        statusText.setFillColor(sf::Color::Red);
                    }
                    file.close();
                }
            }
            if (event.type == sf::Event::TextEntered && inputActive) 
            {
                if (event.text.unicode == 8 && !inputText.empty()) // Backspace
                { 
                    inputText.pop_back();
                    statusText.setString(""); // Очищаем сообщение об ошибке при удалении текста
                } 
                else if (event.text.unicode >= 32 && event.text.unicode < 128) // Печатные символы
                { 
                    inputText += static_cast<char>(event.text.unicode);
                    statusText.setString(""); // Очищаем сообщение об ошибке при вводе текста
                }
                userInputText.setString(inputText);
            }
        }

        // Переключение кадров анимации
        if (animationClock.getElapsedTime().asSeconds() > frameDuration) 
        {
            int col = currentFrame % columns; // Столбец
            int row = currentFrame / columns; // Строка

            // Если это последний столбец и кадров в нем меньше, чем rows, останавливаемся на последнем кадре
            if (col == columns - 1 && row >= 5) 
            {
                row = 4; // Останавливаемся на последнем кадре последнего столбца
            }

            sprite.setTextureRect(sf::IntRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight));

            // Переход к следующему кадру
            currentFrame = (currentFrame + 1) % totalFrames;
            animationClock.restart();
        }

        // Обработка наведения на кнопку
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        buttonHovered = crackButton.getGlobalBounds().contains(mousePos.x, mousePos.y);

        // Анимация свечения кнопки
        time += 0.01;
        int redGlow = static_cast<int>(std::abs(std::sin(time)) * 30 + 180);

        if (buttonHovered) 
        {
            crackButton.setFillColor(sf::Color(redGlow, 50, 50));
        } 
        else 
        {
            crackButton.setFillColor(sf::Color(30, 30, 30));
        }

        inputBox.setOutlineColor(buttonHovered ? sf::Color(80, 80, 180) : sf::Color(50, 50, 180));
        crackButton.setOutlineColor(buttonHovered ? sf::Color(redGlow, 80, 80) : sf::Color(redGlow, 50, 50));

        // Очистка окна и отрисовка
        window.clear(sf::Color(10, 10, 20));

        // Отрисовка анимации
        window.draw(sprite);

        // Отрисовка интерфейса
        window.draw(inputBox);
        window.draw(crackButton);
        window.draw(crackText);
        if (inputText.empty() && !inputActive)
            window.draw(placeholder);
        else
            window.draw(userInputText);

        // Отрисовка статуса
        window.draw(statusText);

        window.display();
    }

    return 0;
}

// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>
// #include <SFML/System.hpp>
// #include <iostream>
// #include <cmath>
// #include <fstream>

// int main() 
// {
//     // Создаем окно с увеличенной высотой
//     sf::RenderWindow window(sf::VideoMode(600, 800), "Crack.NET", sf::Style::Close);

//     // Загружаем шрифт
//     sf::Font font;
//     if (!font.loadFromFile("arial.ttf")) 
//     {
//         std::cerr << "Ошибка загрузки шрифта!" << std::endl;
//         return -1;
//     }

//     // Загружаем спрайт-лист для анимации
//     sf::Texture spriteSheet;
//     if (!spriteSheet.loadFromFile("dog.png")) 
//     {
//         std::cerr << "Ошибка загрузки спрайт-листа!" << std::endl;
//         return -1;
//     }

//     // Параметры спрайт-листа
//     const int columns = 6;               // Количество столбцов в спрайт-листе
//     const int rows = 6;                  // Количество строк в спрайт-листе
//     const int totalFrames = (columns - 1) * rows + 5; // Общее количество кадров (5x6 + 5 = 35)

//     // Размеры одного кадра
//     const int frameWidth = spriteSheet.getSize().x / columns;  // Ширина одного кадра
//     const int frameHeight = spriteSheet.getSize().y / rows;    // Высота одного кадра

//     // Создаем спрайт и устанавливаем текстуру
//     sf::Sprite sprite(spriteSheet);
//     sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight)); // Устанавливаем первый кадр

//     // Поднимаем анимацию еще выше
//     sprite.setPosition((window.getSize().x - frameWidth) / 2, 30);

//     // Переменные для анимации
//     int currentFrame = 0; // Текущий кадр
//     sf::Clock animationClock; // Таймер для переключения кадров
//     float frameDuration = 0.1f; // Длительность одного кадра в секундах (0.1 секунды)

//     // Элементы интерфейса
//     sf::RectangleShape inputBox(sf::Vector2f(400, 40)); // Уменьшил ширину рамки
//     inputBox.setPosition((window.getSize().x - inputBox.getSize().x) / 2, 600); // Опустил еще ниже
//     inputBox.setFillColor(sf::Color(30, 30, 30));
//     inputBox.setOutlineThickness(2);

//     sf::Text placeholder("Enter a path to file to crack...", font, 20);
//     placeholder.setFillColor(sf::Color(150, 150, 150));
//     placeholder.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);

//     sf::RectangleShape crackButton(sf::Vector2f(100, 40));
//     crackButton.setPosition((window.getSize().x - crackButton.getSize().x) / 2, 660); // Опустил еще ниже
//     crackButton.setFillColor(sf::Color(30, 30, 30));
//     crackButton.setOutlineThickness(2);

//     sf::Text crackText("CRACK!", font, 20);
//     crackText.setFillColor(sf::Color::White);
//     crackText.setPosition(crackButton.getPosition().x + 20, crackButton.getPosition().y + 10);

//     std::string inputText;
//     sf::Text userInputText("", font, 20);
//     userInputText.setFillColor(sf::Color::White);
//     userInputText.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);

//     sf::Text statusText("", font, 20); // Текст для отображения статуса
//     statusText.setFillColor(sf::Color::White);
//     statusText.setPosition((window.getSize().x - 300) / 2, 710); // Опустил еще ниже

//     bool inputActive = false;
//     bool buttonHovered = false;
//     float time = 0;

//     // Основной цикл программы
//     while (window.isOpen()) 
//     {
//         sf::Event event;
//         while (window.pollEvent(event)) 
//         {
//             if (event.type == sf::Event::Closed)
//                 window.close();
//             if (event.type == sf::Event::MouseButtonPressed) 
//             {
//                 if (inputBox.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
//                     inputActive = true;
//                 else
//                     inputActive = false;

//                 if (crackButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) 
//                 {
//                     // Проверяем, существует ли файл
//                     std::ifstream file(inputText);
//                     if (file.good()) 
//                     {
//                         statusText.setString("SUCCESS: file was cracked");
//                         statusText.setFillColor(sf::Color::Green);
//                     } 
//                     else 
//                     {
//                         statusText.setString("ERROR: couldn't open file");
//                         statusText.setFillColor(sf::Color::Red);
//                     }
//                     file.close();
//                 }
//             }
//             if (event.type == sf::Event::TextEntered && inputActive) 
//             {
//                 if (event.text.unicode == 8 && !inputText.empty()) // Backspace
//                     inputText.pop_back();
//                 else if (event.text.unicode >= 32 && event.text.unicode < 128) // Печатные символы
//                     inputText += static_cast<char>(event.text.unicode);
//                 userInputText.setString(inputText);
//             }
//         }

//         // Переключение кадров анимации
//         if (animationClock.getElapsedTime().asSeconds() > frameDuration) 
//         {
//             int col = currentFrame % columns; // Столбец
//             int row = currentFrame / columns; // Строка

//             // Если это последний столбец и кадров в нем меньше, чем rows, останавливаемся на последнем кадре
//             if (col == columns - 1 && row >= 5) 
//             {
//                 row = 4; // Останавливаемся на последнем кадре последнего столбца
//             }

//             sprite.setTextureRect(sf::IntRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight));

//             // Переход к следующему кадру
//             currentFrame = (currentFrame + 1) % totalFrames;
//             animationClock.restart();
//         }

//         // Обработка наведения на кнопку
//         sf::Vector2i mousePos = sf::Mouse::getPosition(window);
//         buttonHovered = crackButton.getGlobalBounds().contains(mousePos.x, mousePos.y);

//         // Анимация свечения кнопки
//         time += 0.01;
//         int redGlow = static_cast<int>(std::abs(std::sin(time)) * 30 + 180);

//         if (buttonHovered) 
//         {
//             crackButton.setFillColor(sf::Color(redGlow, 50, 50));
//         } 
//         else 
//         {
//             crackButton.setFillColor(sf::Color(30, 30, 30));
//         }

//         inputBox.setOutlineColor(buttonHovered ? sf::Color(80, 80, 180) : sf::Color(50, 50, 180));
//         crackButton.setOutlineColor(buttonHovered ? sf::Color(redGlow, 80, 80) : sf::Color(redGlow, 50, 50));

//         // Очистка окна и отрисовка
//         window.clear(sf::Color(10, 10, 20));

//         // Отрисовка анимации
//         window.draw(sprite);

//         // Отрисовка интерфейса
//         window.draw(inputBox);
//         window.draw(crackButton);
//         window.draw(crackText);
//         if (inputText.empty() && !inputActive)
//             window.draw(placeholder);
//         else
//             window.draw(userInputText);

//         // Отрисовка статуса
//         window.draw(statusText);

//         window.display();
//     }

//     return 0;
// }
