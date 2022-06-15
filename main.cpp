#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <vector>
#include <random>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
#include <windows.h>

#define X 2560
#define Y 1440
#define START_POINTS 50
#define LOCALHOST "25.73.9.134"
#define PORT 8000

using namespace sf;

std::vector<CircleShape> initColor(std::vector<CircleShape> initPlayer){
    for (int i = 0; i < initPlayer.size(); ++i) initPlayer[i].setFillColor(Color(i + 20,255,i + 20));
    return initPlayer;
}

std::vector<CircleShape> init(std::vector<CircleShape> initPlayer){
    for (int i = 1; i < initPlayer.size(); ++i) {
        initPlayer[i].setFillColor(Color(0 + i,255,0 + i));
        initPlayer[i].setRadius(20);
        initPlayer[i].setPosition(X/2, initPlayer[i-1].getPosition().y + 20);
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
    int mode;

    std::cout << "1 or 2?"; std::cin >> mode;

    WSADATA WSA;
    WSAStartup(WINSOCK_VERSION, &WSA);
    sockaddr_in addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    SOCKET inoutstream = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in client;
    int sizeOfClient = sizeof(client);

    srand(time(NULL));
    std::mt19937 random(rand());
    RenderWindow window(VideoMode(X, Y), "Suck!", Style::Fullscreen);

    std::vector<CircleShape> snakefirst(START_POINTS);
    snakefirst[0].setPosition(X / 2 - 50, Y / 2);
    snakefirst = init(snakefirst);

    std::vector<CircleShape> snakesecond(START_POINTS);
    snakesecond[0].setPosition(X / 2 + 50, Y / 2);
    snakesecond = init(snakesecond);

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

    Text Score1;
    Score1.setFillColor(Color::White);
    Score1.setPosition(X / 2 - 50, 50);
    Score1.setCharacterSize(20);
    Score1.setFont(font);
    Score1.setStyle(sf::Text::Bold);
    int score1 = START_POINTS;

    Text Score2;
    Score2.setFillColor(Color::White);
    Score2.setPosition(X / 2 + 50, 50);
    Score2.setCharacterSize(20);
    Score2.setFont(font);
    Score2.setStyle(sf::Text::Bold);
    int score2 = START_POINTS;

    Text Apple;
    Apple.setFillColor(Color::Red);
    Apple.setString("Apple!");
    Apple.setCharacterSize(20);
    Apple.setPosition(apple.getPosition().x - 10, apple.getPosition().y - 30);
    Apple.setFont(font);
    Apple.setStyle(sf::Text::Bold);

    Text Build;
    Build.setFillColor(Color::White);
    Build.setString("Build 1.3");
    Build.setCharacterSize(20);
    Build.setPosition(X - 20, Y - 30);
    Build.setFont(font);
    Build.setStyle(sf::Text::Bold);

    Clock clock;

    SoundBuffer buffer;
    buffer.loadFromFile("point.wav");
    Sound sound;

    window.setFramerateLimit(30);
    char derection1 = 'u';
    char derection2 = 'u';

    Vector2f buff = apple.getPosition();
    bool needBuff = false;

    if(mode == 1){
        bind(inoutstream, (SOCKADDR*)&addr, sizeof(addr));
        recvfrom(inoutstream, 0, 0, 0, (sockaddr*)&client, &sizeOfClient);
        std::cout << "Connected!" << std::endl;
        recvfrom(inoutstream, (char*)&buff, sizeof(buff), 0, (sockaddr*)&client, &sizeOfClient);
        std::cout << "2Connected!" << std::endl;
    }else{
        sendto(inoutstream, 0, 0, 0, (sockaddr*)&addr, sizeof(addr));
        sendto(inoutstream, (char*)&buff, sizeof(buff), 0, (sockaddr*)&addr, sizeof(addr));
    }

    while(window.isOpen()){
        Time time = clock.getElapsedTime();
        framePerSecond.setString("FPS: " + std::to_string(1.f / time.asSeconds()));
        clock.restart();
        Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed)
                window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Up) && derection1 != 'd') derection1 = 'u';
        else if(Keyboard::isKeyPressed(Keyboard::Right) && derection1 != 'l') derection1 = 'r';
        else if(Keyboard::isKeyPressed(Keyboard::Down) && derection1 != 'u') derection1 = 'd';
        else if(Keyboard::isKeyPressed(Keyboard::Left) && derection1 != 'r') derection1 = 'l';

        if(mode == 1){
            sendto(inoutstream, (char*)&derection1, sizeof(derection1), 0, (sockaddr*)&client, sizeof(client));
            recvfrom(inoutstream, (char*)&derection2, sizeof(derection2), 0, (sockaddr*)&client, &sizeOfClient);
            Vector2f pos = apple.getPosition();
            if(buff != pos) {
                needBuff = true;
                sendto(inoutstream, (char*)&needBuff, sizeof(needBuff), 0, (sockaddr*)&client, sizeof(client));
                sendto(inoutstream, (char *) &pos, sizeof(pos), 0, (sockaddr *) &client, sizeof(client));
                needBuff = false;
            }else{
                sendto(inoutstream, (char*)&needBuff, sizeof(needBuff), 0, (sockaddr*)&client, sizeof(client));
            }
        }else{
            recvfrom(inoutstream, (char*)&derection2, sizeof(derection2), 0, (sockaddr*)&addr, &sizeofaddr);
            sendto(inoutstream, (char*)&derection1, sizeof(derection1), 0, (sockaddr*)&addr, sizeof(addr));
            recvfrom(inoutstream, (char*)&needBuff, sizeof(needBuff), 0, (sockaddr*)&addr, &sizeofaddr);
            if(needBuff) {
                Vector2f pos;
                recvfrom(inoutstream, (char *) &pos, sizeof(pos), 0, (sockaddr *) &addr, &sizeofaddr);
                apple.setPosition(pos);
            }
        }

        if(derection1 == 'r' && snakefirst[0].getPosition().x > X) snakefirst[0].setPosition(0, snakefirst[0].getPosition().y);
        else if(derection1 == 'l' && snakefirst[0].getPosition().x < 0) snakefirst[0].setPosition(X, snakefirst[0].getPosition().y);
        else if(derection1 == 'u' && snakefirst[0].getPosition().y < 0) snakefirst[0].setPosition(snakefirst[0].getPosition().x, Y);
        else if(derection1 == 'd' && snakefirst[0].getPosition().y > Y) snakefirst[0].setPosition(snakefirst[0].getPosition().x, 0);

        if(derection2 == 'r' && snakesecond[0].getPosition().x > X) snakesecond[0].setPosition(0, snakesecond[0].getPosition().y);
        else if(derection2 == 'l' && snakesecond[0].getPosition().x < 0) snakesecond[0].setPosition(X, snakesecond[0].getPosition().y);
        else if(derection2 == 'u' && snakesecond[0].getPosition().y < 0) snakesecond[0].setPosition(snakesecond[0].getPosition().x, Y);
        else if(derection2 == 'd' && snakesecond[0].getPosition().y > Y) snakesecond[0].setPosition(snakesecond[0].getPosition().x, 0);

        snakefirst = moveSnake(snakefirst, 20, derection1);
        snakesecond = moveSnake(snakesecond, 20, derection2);
        Vector2f snPos = snakefirst[0].getPosition();
        Vector2f snsPos = snakesecond[0].getPosition();
        Vector2f apPos = apple.getPosition();
        if(abs(snPos.x - apPos.x) <= 100 && abs(snPos.y - apPos.y) <= 100){
            apple.setPosition(random()%X,random()%Y);
            Apple.setPosition(apple.getPosition().x - 10, apple.getPosition().y - 30);
            snakefirst.push_back(snakefirst[snakefirst.size() - 1]);
            snakefirst = initColor(snakefirst);
            score1++;
            sound.setBuffer(buffer);
            sound.play();
        }

        if(abs(snsPos.x - apPos.x) <= 100 && abs(snsPos.y - apPos.y) <= 100){
            apple.setPosition(random()%X,random()%Y);
            Apple.setPosition(apple.getPosition().x - 10, apple.getPosition().y - 30);
            snakesecond.push_back(snakesecond[snakesecond.size() - 1]);
            snakesecond = initColor(snakesecond);
            score2++;
            sound.setBuffer(buffer);
            sound.play();
        }

        Score1.setString("Score1: " + std::to_string(score1));
        Score2.setString("Score2: " + std::to_string(score2));

        window.clear();

        window.draw(framePerSecond);
        drawSnake(snakefirst, &window);
        drawSnake(snakesecond, &window);
        window.draw(apple);
        window.draw(Apple);
        window.draw(Score1);
        window.draw(Score2);
        window.draw(Build);

        if(isColide(snakefirst)) {
            if(score1 > 5 && score1 - 5 != 0) {
                score1 -= 5;
                for (int i = 0; i < 5; ++i) snakefirst.pop_back();
            }else window.close();
        }
        if(isColide(snakesecond)) {
            if(score1 > 5 && score1 - 5 != 0) {
                score1 -= 5;
                for (int i = 0; i < 5; ++i) snakesecond.pop_back();
            }else window.close();
        }

        window.display();
    }
}
