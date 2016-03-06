#include "Utility.hpp"
#include "Animation.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <random>
#include <cmath>
#include <ctime>
#include <cassert>


namespace {
    std::default_random_engine createRandomEngine() {
        auto seed = static_cast<unsigned long>(std::time(nullptr));
        return std::default_random_engine(seed);
    }

    auto RandomEngine = createRandomEngine();
}

std::string toString(sf::Keyboard::Key key) {
#define BOOK_KEYTOSTRING_CASE(KEY) case sf::Keyboard::KEY: return #KEY;

    switch (key) {
        BOOK_KEYTOSTRING_CASE(Unknown)
        BOOK_KEYTOSTRING_CASE(A)
        BOOK_KEYTOSTRING_CASE(B)
        BOOK_KEYTOSTRING_CASE(C)
        BOOK_KEYTOSTRING_CASE(D)
        BOOK_KEYTOSTRING_CASE(E)
        BOOK_KEYTOSTRING_CASE(F)
        BOOK_KEYTOSTRING_CASE(G)
        BOOK_KEYTOSTRING_CASE(H)
        BOOK_KEYTOSTRING_CASE(I)
        BOOK_KEYTOSTRING_CASE(J)
        BOOK_KEYTOSTRING_CASE(K)
        BOOK_KEYTOSTRING_CASE(L)
        BOOK_KEYTOSTRING_CASE(M)
        BOOK_KEYTOSTRING_CASE(N)
        BOOK_KEYTOSTRING_CASE(O)
        BOOK_KEYTOSTRING_CASE(P)
        BOOK_KEYTOSTRING_CASE(Q)
        BOOK_KEYTOSTRING_CASE(R)
        BOOK_KEYTOSTRING_CASE(S)
        BOOK_KEYTOSTRING_CASE(T)
        BOOK_KEYTOSTRING_CASE(U)
        BOOK_KEYTOSTRING_CASE(V)
        BOOK_KEYTOSTRING_CASE(W)
        BOOK_KEYTOSTRING_CASE(X)
        BOOK_KEYTOSTRING_CASE(Y)
        BOOK_KEYTOSTRING_CASE(Z)
        BOOK_KEYTOSTRING_CASE(Num0)
        BOOK_KEYTOSTRING_CASE(Num1)
        BOOK_KEYTOSTRING_CASE(Num2)
        BOOK_KEYTOSTRING_CASE(Num3)
        BOOK_KEYTOSTRING_CASE(Num4)
        BOOK_KEYTOSTRING_CASE(Num5)
        BOOK_KEYTOSTRING_CASE(Num6)
        BOOK_KEYTOSTRING_CASE(Num7)
        BOOK_KEYTOSTRING_CASE(Num8)
        BOOK_KEYTOSTRING_CASE(Num9)
        BOOK_KEYTOSTRING_CASE(Escape)
        BOOK_KEYTOSTRING_CASE(LControl)
        BOOK_KEYTOSTRING_CASE(LShift)
        BOOK_KEYTOSTRING_CASE(LAlt)
        BOOK_KEYTOSTRING_CASE(LSystem)
        BOOK_KEYTOSTRING_CASE(RControl)
        BOOK_KEYTOSTRING_CASE(RShift)
        BOOK_KEYTOSTRING_CASE(RAlt)
        BOOK_KEYTOSTRING_CASE(RSystem)
        BOOK_KEYTOSTRING_CASE(Menu)
        BOOK_KEYTOSTRING_CASE(LBracket)
        BOOK_KEYTOSTRING_CASE(RBracket)
        BOOK_KEYTOSTRING_CASE(SemiColon)
        BOOK_KEYTOSTRING_CASE(Comma)
        BOOK_KEYTOSTRING_CASE(Period)
        BOOK_KEYTOSTRING_CASE(Quote)
        BOOK_KEYTOSTRING_CASE(Slash)
        BOOK_KEYTOSTRING_CASE(BackSlash)
        BOOK_KEYTOSTRING_CASE(Tilde)
        BOOK_KEYTOSTRING_CASE(Equal)
        BOOK_KEYTOSTRING_CASE(Dash)
        BOOK_KEYTOSTRING_CASE(Space)
        BOOK_KEYTOSTRING_CASE(Return)
        BOOK_KEYTOSTRING_CASE(BackSpace)
        BOOK_KEYTOSTRING_CASE(Tab)
        BOOK_KEYTOSTRING_CASE(PageUp)
        BOOK_KEYTOSTRING_CASE(PageDown)
        BOOK_KEYTOSTRING_CASE(End)
        BOOK_KEYTOSTRING_CASE(Home)
        BOOK_KEYTOSTRING_CASE(Insert)
        BOOK_KEYTOSTRING_CASE(Delete)
        BOOK_KEYTOSTRING_CASE(Add)
        BOOK_KEYTOSTRING_CASE(Subtract)
        BOOK_KEYTOSTRING_CASE(Multiply)
        BOOK_KEYTOSTRING_CASE(Divide)
        BOOK_KEYTOSTRING_CASE(Left)
        BOOK_KEYTOSTRING_CASE(Right)
        BOOK_KEYTOSTRING_CASE(Up)
        BOOK_KEYTOSTRING_CASE(Down)
        BOOK_KEYTOSTRING_CASE(Numpad0)
        BOOK_KEYTOSTRING_CASE(Numpad1)
        BOOK_KEYTOSTRING_CASE(Numpad2)
        BOOK_KEYTOSTRING_CASE(Numpad3)
        BOOK_KEYTOSTRING_CASE(Numpad4)
        BOOK_KEYTOSTRING_CASE(Numpad5)
        BOOK_KEYTOSTRING_CASE(Numpad6)
        BOOK_KEYTOSTRING_CASE(Numpad7)
        BOOK_KEYTOSTRING_CASE(Numpad8)
        BOOK_KEYTOSTRING_CASE(Numpad9)
        BOOK_KEYTOSTRING_CASE(F1)
        BOOK_KEYTOSTRING_CASE(F2)
        BOOK_KEYTOSTRING_CASE(F3)
        BOOK_KEYTOSTRING_CASE(F4)
        BOOK_KEYTOSTRING_CASE(F5)
        BOOK_KEYTOSTRING_CASE(F6)
        BOOK_KEYTOSTRING_CASE(F7)
        BOOK_KEYTOSTRING_CASE(F8)
        BOOK_KEYTOSTRING_CASE(F9)
        BOOK_KEYTOSTRING_CASE(F10)
        BOOK_KEYTOSTRING_CASE(F11)
        BOOK_KEYTOSTRING_CASE(F12)
        BOOK_KEYTOSTRING_CASE(F13)
        BOOK_KEYTOSTRING_CASE(F14)
        BOOK_KEYTOSTRING_CASE(F15)
        BOOK_KEYTOSTRING_CASE(Pause)
    }

    return "";
}

void centerOrigin(sf::Sprite &sprite) {
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

void centerOrigin(sf::Text &text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

void centerOrigin(Animation &animation) {
    sf::FloatRect bounds = animation.getLocalBounds();
    animation.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

float toDegree(float radian) {
    return 180.f / 3.141592653589793238462643383f * radian;
}

float toRadian(float degree) {
    return 3.141592653589793238462643383f / 180.f * degree;
}

int randomInt(int exclusiveMax) {
    std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
    return distr(RandomEngine);
}

float length(sf::Vector2f vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f unitVector(sf::Vector2f vector) {
    //assert(vector != sf::Vector2f(0.f, 0.f));
    if (vector != sf::Vector2f(0.f, 0.f)) {
        return vector / length(vector);
    } else {
        return vector;
    }
}

sf::Event transformEvent(const sf::Event &event, sf::Vector2f position) {
    sf::Event *evt = new sf::Event(event);

    if (event.type == sf::Event::MouseMoved) {
        evt->mouseMove.x = event.mouseMove.x - (int) position.x;
        evt->mouseMove.y = event.mouseMove.y - (int) position.y;
    }

    else if (event.type == sf::Event::MouseButtonPressed) {
        evt->mouseButton.x = event.mouseButton.x - (int) position.x;
        evt->mouseButton.y = event.mouseButton.y - (int) position.y;
    }

    else if (event.type == sf::Event::MouseWheelMoved) {
        evt->mouseWheel.x = event.mouseWheel.x - (int) position.x;
        evt->mouseWheel.y = event.mouseWheel.y - (int) position.y;
    }

    else if (event.type == sf::Event::MouseWheelScrolled) {
        evt->mouseWheelScroll.delta = event.mouseWheelScroll.delta;
        evt->mouseWheelScroll.wheel = event.mouseWheelScroll.wheel;
        evt->mouseWheelScroll.x = event.mouseWheelScroll.x - (int) position.x;
        evt->mouseWheelScroll.y = event.mouseWheelScroll.y - (int) position.y;
    }

    else if (event.type == sf::Event::TouchMoved || event.type == sf::Event::TouchBegan || event.type == sf::Event::TouchEnded) {
        evt->touch.x = event.touch.x - (int) position.x;
        evt->touch.y = event.touch.y - (int) position.y;
    }

    return *evt;
}

char keyToChar(sf::Keyboard::Key key) {

    switch (key){
        case sf::Keyboard::Key::A:
            return 'A';
        case sf::Keyboard::Key::B:
            return 'B';
        case sf::Keyboard::Key::C:
            return 'C';
        case sf::Keyboard::Key::D:
            return 'D';
        case sf::Keyboard::Key::E:
            return 'E';
        case sf::Keyboard::Key::F:
            return 'F';
        case sf::Keyboard::Key::G:
            return 'G';
        case sf::Keyboard::Key::H:
            return 'H';
        case sf::Keyboard::Key::I:
            return 'I';
        case sf::Keyboard::Key::J:
            return 'J';
        case sf::Keyboard::Key::K:
            return 'K';
        case sf::Keyboard::Key::L:
            return 'L';
        case sf::Keyboard::Key::M:
            return 'M';
        case sf::Keyboard::Key::N:
            return 'N';
        case sf::Keyboard::Key::O:
            return 'O';
        case sf::Keyboard::Key::P:
            return 'P';
        case sf::Keyboard::Key::Q:
            return 'Q';
        case sf::Keyboard::Key::R:
            return 'R';
        case sf::Keyboard::Key::S:
            return 'S';
        case sf::Keyboard::Key::T:
            return 'T';
        case sf::Keyboard::Key::U:
            return 'U';
        case sf::Keyboard::Key::V:
            return 'V';
        case sf::Keyboard::Key::W:
            return 'W';
        case sf::Keyboard::Key::X:
            return 'X';
        case sf::Keyboard::Key::Y:
            return 'Y';
        case sf::Keyboard::Key::Z:
            return 'Z';
        case sf::Keyboard::Key::Num0:
            return '0';
        case sf::Keyboard::Key::Num1:
            return '1';
        case sf::Keyboard::Key::Num2:
            return '2';
        case sf::Keyboard::Key::Num3:
            return '3';
        case sf::Keyboard::Key::Num4:
            return '4';
        case sf::Keyboard::Key::Num5:
            return '5';
        case sf::Keyboard::Key::Num6:
            return '6';
        case sf::Keyboard::Key::Num7:
            return '7';
        case sf::Keyboard::Key::Num8:
            return '8';
        case sf::Keyboard::Key::Num9:
            return '9';
        case sf::Keyboard::Key::Numpad0:
            return '0';
        case sf::Keyboard::Key::Numpad1:
            return '1';
        case sf::Keyboard::Key::Numpad2:
            return '2';
        case sf::Keyboard::Key::Numpad3:
            return '3';
        case sf::Keyboard::Key::Numpad4:
            return '4';
        case sf::Keyboard::Key::Numpad5:
            return '5';
        case sf::Keyboard::Key::Numpad6:
            return '6';
        case sf::Keyboard::Key::Numpad7:
            return '7';
        case sf::Keyboard::Key::Numpad8:
            return '8';
        case sf::Keyboard::Key::Numpad9:
            return '9';
        case sf::Keyboard::Key::Space:
            return ' ';
        case sf::Keyboard::Key::Tab:
            return '\t';
        case sf::Keyboard::Key::LBracket:
            return '[';
        case sf::Keyboard::Key::RBracket:
            return ']';
        case sf::Keyboard::Key::SemiColon:
            return ';';
        case sf::Keyboard::Key::Comma:
            return ',';
        case sf::Keyboard::Key::Period:
            return '.';
        case sf::Keyboard::Key::Quote:
            return '\'';
        case sf::Keyboard::Key::Slash:
            return '/';
        case sf::Keyboard::Key::BackSlash:
            return '\\';
        case sf::Keyboard::Key::Tilde:
            return '~';
        case sf::Keyboard::Key::Equal:
            return '=';
        case sf::Keyboard::Key::Dash:
            return '-';
        case sf::Keyboard::Key::Add:
            return '+';
        case sf::Keyboard::Key::Subtract:
            return '-';
        case sf::Keyboard::Key::Multiply:
            return '*';
        case sf::Keyboard::Key::Divide:
            return '/';
        default:
            return '\0';
    }

}
