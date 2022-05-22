#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <vector>
#include <random>

#define X 2560
#define Y 1440
#define START_POINTS 50

using namespace sf;

std::vector<CircleShape> initColor(std::vector<CircleShape> initPlayer){
    for (int i = 0; i < initPlayer.size(); ++i) initPlayer[i].setFillColor(Color(i + 20,255,i + 20));
    return initPlayer;
}

std::vector<CircleShape> init(std::vector<CircleShape> initPlayer){
    for (int i = 0; i < initPlayer.size(); ++i) {
        initPlayer[i].setFillColor(Color(0 + i,255,0 + i));
        initPlayer[i].setRadius(20);
        if(i == 0)initPlayer[i].setPosition(X/2, Y/2);
        else initPlayer[i].setPosition(X/2, initPlayer[i-1].getPosition().y + 20);
    }
    return initPlayer;
}

std::vector<CircleShape> moveSnake(std::vector<CircleShape> player, int speed, char direction){
    Vector2f Buff[2];
    for (int i = 0; i < player.size(); i++) {
        if(i == 0) {
            Buff[0] = player[i].getPosition();
            switch (direction) {
                case 'u':
                    player[i].setPosition(player[i].getPosition().x, player[i].getPosition().y - speed);
                    break;
                case 'd':
                    player[i].setPosition(player[i].getPosition().x, player[i].getPosition().y + speed);
                    break;
                case 'r':
                    player[i].setPosition(player[i].getPosition().x + speed, player[i].getPosition().y);
                    break;
                case 'l':
                    player[i].setPosition(player[i].getPosition().x - speed, player[i].getPosition().y);
                    break;
            }
        }else{
            Buff[1] = player[i].getPosition();
            player[i].setPosition(Buff[0]);
            Buff[0] = Buff[1];
        }
    }
    return player;
}

bool isColide(std::vector<CircleShape> player){
    for (int i = 3; i < player.size(); ++i) if(player[0].getGlobalBounds().intersects(player[i].getGlobalBounds())) return true;
    return false;
}

void drawSnake(std::vector<CircleShape> player, RenderWindow* window){
    for (int i = 0; i < player.size(); ++i) window->draw(player[i]);
}

int main() {
    srand(time(NULL));
    std::mt19937 random(rand());
    RenderWindow window(VideoMode(X, Y), "Suck!", Style::Fullscreen);

    std::vector<CircleShape> snake(START_POINTS);
    snake = init(snake);

    CircleShape apple;
    apple.setFillColor(Color::Red);
    apple.setRadius(18);
    apple.setPosition(random()%X,random()%Y);

    Font font;
    font.loadFromFile("CyrilicOld.ttf");
    Text framePerSecond;
    framePerSecond.setFillColor(Color::White);
    framePerSecond.setPosition(20, 20);
    framePerSecond.setCharacterSize(20);
    framePerSecond.setFont(font);
    framePerSecond.setStyle(sf::Text::Bold);

    Text Score;
    Score.setFillColor(Color::White);
    Score.setPosition(X/2, 50);
    Score.setCharacterSize(20);
    Score.setFont(font);
    Score.setStyle(sf::Text::Bold);
    int score = START_POINTS;

    Text Apple;
    Apple.setFillColor(Color::Red);
    Apple.setString("Apple!");
    Apple.setCharacterSize(20);
    Apple.setPosition(apple.getPosition().x - 10, apple.getPosition().y - 30);
    Apple.setFont(font);
    Apple.setStyle(sf::Text::Bold);

    Text Build;
    Build.setFillColor(Color::White);
    Build.setString("Build 1.0");
    Build.setCharacterSize(20);
    Build.setPosition(X - 20, Y - 30);
    Build.setFont(font);
    Build.setStyle(sf::Text::Bold);

    Clock clock;

    SoundBuffer buffer;
    buffer.loadFromFile("point.wav");
    Sound sound;

    window.setFramerateLimit(30);
    char derection = 'u';
    while(window.isOpen()){
        Time time = clock.getElapsedTime();
        framePerSecond.setString("FPS: " + std::to_string(1.f / time.asSeconds()));
        clock.restart();
        Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed)
                window.close();
        }
        if(Keyboard::isKeyPressed(Keyboard::Up) && derection != 'd') derection = 'u';
        else if(Keyboard::isKeyPressed(Keyboard::Right) && derection != 'l') derection = 'r';
        else if(Keyboard::isKeyPressed(Keyboard::Down) && derection != 'u') derection = 'd';
        else if(Keyboard::isKeyPressed(Keyboard::Left) && derection != 'r') derection = 'l';

        if(derection == 'r' && snake[0].getPosition().x > X) snake[0].setPosition(0, snake[0].getPosition().y);
        else if(derection == 'l' && snake[0].getPosition().x  < 0) snake[0].setPosition(X, snake[0].getPosition().y);
        else if(derection == 'u' && snake[0].getPosition().y < 0) snake[0].setPosition(snake[0].getPosition().x, Y);
        else if(derection == 'd' && snake[0].getPosition().y > Y) snake[0].setPosition(snake[0].getPosition().x, 0);

        snake = moveSnake(snake, 20, derection);

        if(apple.getGlobalBounds().intersects(snake[0].getGlobalBounds())){
            apple.setPosition(random()%X,random()%Y);
            Apple.setPosition(apple.getPosition().x - 10, apple.getPosition().y - 30);
            snake.push_back(snake[snake.size() - 1]);
            snake = initColor(snake);
            score++;
            sound.setBuffer(buffer);
            sound.play();
        }

        Score.setString("Score: " + std::to_string(score));

        window.clear();

        window.draw(framePerSecond);
        drawSnake(snake, &window);
        window.draw(apple);
        window.draw(Apple);
        window.draw(Score);
        window.draw(Build);

        if(isColide(snake)) {
            if(score > 5 && score - 5 != 0) {
                score -= 5;
                for (int i = 0; i < 5; ++i) snake.pop_back();
            }else window.close();
        }

        window.display();
    }
}