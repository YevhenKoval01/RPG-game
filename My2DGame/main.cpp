#include <iostream>
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <cmath>
#include <array>
#include <random>

struct EnemyAttack {
    sf::CircleShape shape;
    sf::Vector2f direction;
};
enum class Items {
    Empty,
    Sword,
    Keys,
    HealthEssention,
    HealPotion
};
std::string itemToString(Items item) {
    switch (item) {
        case Items::Empty: return "Empty";
        case Items::Sword: return "Sword";
        case Items::HealthEssention: return "Health Essention";
        case Items::HealPotion: return "Heal Potion";
        case Items::Keys: return "Keys";
        default: return "Unknown Item";
    }
}

class Animation {
public:
    Animation() : currentFrame(0), animationSpeed(0.1f), time(0) {}

    void addFrame(const sf::Texture& texture) {
        frames.push_back(texture);
    }

    void update(float deltaTime) {
        time += deltaTime;
        if (time >= animationSpeed) {
            currentFrame = (currentFrame + 1) % framesOrder.size();
            time = 0;
        }
    }

    const sf::Texture& getCurrentFrame() const {
        return frames[framesOrder[currentFrame]];
    }

    void setAnimationSpeed(float speed) {
        animationSpeed = speed;
    }

    void setFramesOrder(const std::vector<int>& order) {
        framesOrder = order;
    }

private:
    std::vector<sf::Texture> frames;
    std::vector<int> framesOrder;
    size_t currentFrame;
    float animationSpeed;
    float time;
};

class Player {
public:

    int lvl = 1;
    int money = 10;
    bool keysFound = false;
    Player() {
        loadTextures();
        sprite.setTexture(defaultTextures["down"]);
        sprite.setScale(0.5f, 0.5f);
        previousPosition = sprite.getPosition();
        if (!font.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\determination-extended\\determination-extended.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
        inventory = std::vector<Items>(inventoryCapacity);
    }


    const std::vector<Items> &getInventory() const {
        return inventory;
    }


    void setInventory(const std::vector<Items> &inventory) {
        Player::inventory = inventory;
    }

    size_t getInventoryCapacity() const {
        return inventoryCapacity;
    }
    size_t getInventorySize() const {
        return std::count_if(inventory.begin(), inventory.end(), [](Items item) {
            return item != Items::Empty;
        });
    }
    int basehp = 80;



    void loadTextures() {
        loadAnimation("left", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Left_", {1, 0, 1, 2});
        loadAnimation("right", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Right_", {1, 0, 1, 2});
        loadAnimation("up", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Up_", {1, 2});
        loadAnimation("down", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Down_", {1, 2});

        loadDefaultTexture("left", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Left_2.png");
        loadDefaultTexture("right", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Right_2.png");
        loadDefaultTexture("up", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Up_1.png");
        loadDefaultTexture("down", "C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\Down_1.png");
    }

    void loadAnimation(const std::string& direction, const std::string& basePath, const std::vector<int>& order) {
        Animation animation;
        for (int i = 0; i < 3; ++i) {
            sf::Texture texture;
            if (!texture.loadFromFile(basePath + std::to_string(i + 1) + ".png")) {
                std::cerr << "Error loading texture: " << basePath + std::to_string(i + 1) + ".png" << std::endl;
            } else {
                animation.addFrame(texture);
            }
        }
        animation.setFramesOrder(order);
        animations[direction] = animation;
    }

    void setPosition(const sf::Vector2f& position) {
        sprite.setPosition(position);
        updateFeetHitbox();
        previousPosition = position;
    }

    void revertPosition() {
        sprite.setPosition(previousPosition);
        updateFeetHitbox();
    }

    const sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    const sf::FloatRect getFeetBounds() const {
        return feetHitbox;
    }

    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }
    void addItem(const Items& item) {
        auto it = std::find(inventory.begin(), inventory.end(), Items::Empty);

        if (it != inventory.end()) {
            *it = item;
        } else if (inventory.size() < inventoryCapacity) {
            inventory.push_back(item);
        } else {
            std::cerr << "Inventory is full!" << std::endl;
        }
    }

    void removeItem(const Items& item) {
        auto it = std::find(inventory.begin(), inventory.end(), item);
        if (it != inventory.end()) {
            inventory.erase(it);
        } else {
            std::cerr << "Item not found in inventory!" << std::endl;
        }
    }


    void loadDefaultTexture(const std::string& direction, const std::string& filePath) {
        sf::Texture texture;
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Error loading default texture: " << filePath << std::endl;
        } else {
            defaultTextures[direction] = texture;
        }
    }

    void update(float deltaTime) {
        if (isMoving) {
            animations[currentDirection].update(deltaTime / 5);
            sprite.setTexture(animations[currentDirection].getCurrentFrame());
        } else {
            sprite.setTexture(defaultTextures[currentDirection]);
        }
        updateFeetHitbox();  // Оновлюємо хітбокс ніг при кожному оновленні
    }

    void handleInput() {
        isMoving = false;
        bool movingLeft = false, movingRight = false, movingUp = false, movingDown = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            movingLeft = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            movingRight = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            movingUp = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            movingDown = true;
        }

        float diagonalSpeed = movementSpeed / sqrt(2.0f);

        previousPosition = sprite.getPosition();  // Запам'ятовуємо поточну позицію

        if (movingLeft && !movingRight) {
            if (movingUp && !movingDown) {
                move(-diagonalSpeed, -diagonalSpeed);
                currentDirection = "left";
                isMoving = true;
            } else if (movingDown && !movingUp) {
                move(-diagonalSpeed, diagonalSpeed);
                currentDirection = "left";
                isMoving = true;
            } else {
                move(-movementSpeed, 0);
                currentDirection = "left";
                isMoving = true;
            }
        } else if (movingRight && !movingLeft) {
            if (movingUp && !movingDown) {
                move(diagonalSpeed, -diagonalSpeed);
                currentDirection = "right";
                isMoving = true;
            } else if (movingDown && !movingUp) {
                move(diagonalSpeed, diagonalSpeed);
                currentDirection = "right";
                isMoving = true;
            } else {
                move(movementSpeed, 0);
                currentDirection = "right";
                isMoving = true;
            }
        } else if (movingUp && !movingDown) {
            move(0, -movementSpeed);
            currentDirection = "up";
            isMoving = true;
        } else if (movingDown && !movingUp) {
            move(0, movementSpeed);
            currentDirection = "down";
            isMoving = true;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        if (interactionActive) {
            drawExclamationMark(window);
        }
    }

    void setIsInteracting(bool interacting) {
        isInteracting = interacting;
    }

    bool getIsInteracting() const {
        return isInteracting;
    }

    bool checkItem(Items item) const {
        return std::find(inventory.begin(), inventory.end(), item) != inventory.end();
    }
    void setInteractionActive(bool active) {
        interactionActive = active;
    }
    bool getInteractionActive() const {
        return interactionActive;
    }
    int getMaxHp() const {
        return maxHp;
    }

    void setMaxHp(int maxHp) {
        Player::maxHp = maxHp;
    }

    int getCurrentHp() const {
        return currentHp;
    }

    void setCurrentHp(int currentHp) {
        Player::currentHp = currentHp;
    }
    int getCurrentdamage() const {
        return currentdamage;
    }

    void setCurrentdamage(int currentdamage) {
        Player::currentdamage = currentdamage;
    }

    int getCurrentXp() const {
        return currentXp;
    }

    void setCurrentXp(int currentXp) {
        Player::currentXp = currentXp;
    }

    int getXpToNextLvl() const {
        return xpToNextLvl;
    }

    void setXpToNextLvl(int xpToNextLvl) {
        Player::xpToNextLvl = xpToNextLvl;
    }

    int getBaseDamage() const {
        return baseDamage;
    }

    void setBaseDamage(int basedamage) {
        Player::baseDamage = baseDamage;
    }
    void setMovementSpeed(float movementSpeed) {
        Player::movementSpeed = movementSpeed;
    }



private:
    void move(float offsetX, float offsetY) {
        sprite.move(offsetX, offsetY);
        updateFeetHitbox();
    }

    void updateFeetHitbox() {
        sf::FloatRect globalBounds = sprite.getGlobalBounds();
        feetHitbox.left = globalBounds.left +39;
        feetHitbox.top = globalBounds.top +140;
        feetHitbox.width = globalBounds.width - 80;
        feetHitbox.height = 27;
    }
    void drawExclamationMark(sf::RenderWindow& window) {

        sf::Text exclamation;

        exclamation.setFont(font);
        exclamation.setCharacterSize(50);
        exclamation.setString("!");
        exclamation.setPosition(this->getPosition().x, this->getPosition().y - 50);
        window.draw(exclamation);

    }

public:
    sf::Sprite &getSprite() {
        return sprite;
    }
private:

    sf::Sprite sprite;
    sf::FloatRect feetHitbox;
    std::vector<Items> inventory;
    size_t inventoryCapacity = 10;
    sf::Vector2f previousPosition;
    std::map<std::string, Animation> animations;
    std::map<std::string, sf::Texture> defaultTextures;
    std::string currentDirection = "down";
    bool interactionActive = false;

    bool isInteracting = false;
    bool isMoving = false;
    int maxHp = 100;
    int currentHp = 100; // narazie tu
    int currentXp = 0;
    int xpToNextLvl = 200;
    int baseDamage = 6;
    int currentdamage = 6; //narazie tu

    float movementSpeed = 0.20f;
    sf::Font font;

};

class Location {
public:

    struct transitionBox {
        sf::FloatRect box;
        std::string toLoc;
        int id;
    };
    struct InteractiveBox {
        sf::FloatRect box;
        std::string loc;
        int id;
    };
    struct InnitiateBox {
        sf::FloatRect box;
        bool enabled;
        int id;
    };
    std::string texturePath;
    sf::Vector2f spawnPosition;

    Location() : texturePath(""), spawnPosition(0.0f, 0.0f) {}

    Location(const std::string& texturePath, const sf::Vector2f& spawnPosition)
            : spawnPosition(spawnPosition), texturePath(texturePath) {
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Error loading texture: " << texturePath << std::endl;
        } else {
            std::cout << "Texture loaded successfully: " << texturePath << std::endl;
            sprite.setTexture(texture);
        }
    }

    void addInteractiveBox(const sf::FloatRect& box,const std::string& loc ,const int& id) {
        interactiveBoxes.push_back({box, loc,id});
    }

    const std::vector<InteractiveBox>& getInteractiveBoxes() const {
        return interactiveBoxes;
    }

    void addCollisionBox(const sf::FloatRect& box) {
        collisionBoxes.push_back(box);
    }
    void addInnitiateBox(const sf::FloatRect& box,const bool& enabled, const int& id) {
        innitiateBoxes.push_back({box, enabled,id});
    }

    void addTransitionBox(const sf::FloatRect& box, const std::string& targetLocation,const int& id) {
        transitionBoxes.push_back({box, targetLocation,id});
    }

    sf::Sprite& getSprite()  {
        return sprite;
    }

    const sf::Vector2f& getSpawnPosition() const {
        return spawnPosition;
    }

    const std::vector<sf::FloatRect>& getCollisionBoxes() const {
        return collisionBoxes;
    }
    std::vector<InnitiateBox>& getInnitiateBoxes() {
        return innitiateBoxes;
    }



    const std::vector<transitionBox>& getTransitionBoxes() const {
        return transitionBoxes;
    }
    sf::Sprite sprite; ////////////
private:
    sf::Texture texture;
public:
    const sf::Texture &getTexture() const {
        return texture;
    }

    void setTexture(const sf::Texture &texture) {
        Location::texture = texture;
    }

private:

    std::vector<sf::FloatRect> collisionBoxes;
    std::vector<InnitiateBox> innitiateBoxes;
    std::vector<transitionBox> transitionBoxes;
    std::vector<InteractiveBox> interactiveBoxes;
};
Player player;                             //Zmienne globalne
std::map<std::string, Location> locations;
std::string currentLocName = "Loc1";
Location& currentLoc = locations[currentLocName];
sf::Font font;
bool locToChange(false);
sf::Texture carTexture;
sf::Sprite carSprite;
bool carEnabled = false;
sf::Sprite locSp;
bool dis(true);



void displayLocation(sf::RenderWindow& window, Location& location) {



    sf::RectangleShape hitboxShape;
    hitboxShape.setFillColor(sf::Color(1, 0, 0, 1));

    // Collision boxes For Test
    for (const auto& box : location.getCollisionBoxes()) {
        hitboxShape.setPosition(box.left, box.top);
        hitboxShape.setSize(sf::Vector2f(box.width, box.height));
        window.draw(hitboxShape);
    }

    // Trasition boxes For Test
    for (const auto& transition : location.getTransitionBoxes()) {
        hitboxShape.setPosition(transition.box.left, transition.box.top);
        hitboxShape.setSize(sf::Vector2f(transition.box.width, transition.box.height));
        window.draw(hitboxShape);
    }


   // hitboxShape.setFillColor(sf::Color(1, 0, 0, 100));

    // Interactive boxes For Test
    for (const auto& interactiveBox : location.getInteractiveBoxes()) {
        hitboxShape.setPosition(interactiveBox.box.left, interactiveBox.box.top);
        hitboxShape.setSize(sf::Vector2f(interactiveBox.box.width, interactiveBox.box.height));
        window.draw(hitboxShape);
    }

    // Innitiate boxes
    for (auto& innitiateBox : location.getInnitiateBoxes()) {
        hitboxShape.setPosition(innitiateBox.box.left, innitiateBox.box.top);
        hitboxShape.setSize(sf::Vector2f(innitiateBox.box.width, innitiateBox.box.height));
        window.draw(hitboxShape);
    }
}

void displayTextBox(sf::RenderWindow& window, const std::vector<std::string>& messages, const int& delayM ) {
    sf::RectangleShape textBox(sf::Vector2f(600, 140));
    textBox.setFillColor(sf::Color::Black);
    textBox.setOutlineColor(sf::Color::White);
    textBox.setOutlineThickness(5);
    textBox.setPosition(600, 920);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    text.setPosition(650, 940);

    size_t currentMessageIndex = 0;
    size_t currentCharIndex = 0;
    sf::Clock clock;
    sf::Time delay = sf::milliseconds(delayM);

    bool enterPressed = false;
    sf::Clock enterClock;
    sf::Time enterDelay = sf::milliseconds(200);


    if (!messages.empty()) {
        std::string initialMessage = "* " + messages[0];
        text.setString(initialMessage.substr(0, 1));
    }

    while (currentMessageIndex < messages.size() && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (!enterPressed && enterClock.getElapsedTime() > enterDelay) {
                    enterPressed = true;
                    enterClock.restart();
                }
            }
        }

        if (enterPressed && enterClock.getElapsedTime() > enterDelay) {
            currentMessageIndex++;
            currentCharIndex = 0;
            clock.restart();
            enterPressed = false;
            if (currentMessageIndex < messages.size()) {
                std::string nextMessage = "* " + messages[currentMessageIndex];
                text.setString(nextMessage.substr(0, 1));
            }
        }

        if (currentMessageIndex < messages.size()) {
            std::string currentMessage = "* " + messages[currentMessageIndex];
            if (currentCharIndex < currentMessage.length() && clock.getElapsedTime() > delay) {
                currentCharIndex++;
                std::string displayText = currentMessage.substr(0, currentCharIndex);

                size_t lineLength = 30;
                for (size_t i = lineLength; i < displayText.length(); i += lineLength + 1) {
                    if (i < displayText.length()) {
                        displayText.insert(i, "\n");
                    }
                }

                text.setString(displayText);
                clock.restart();
            }
        }

        window.clear();
        window.draw(locSp);
        displayLocation(window, currentLoc);
        player.draw(window);
        window.draw(textBox);
        window.draw(text);
        window.display();
    }
}

void displayTextBox(sf::RenderWindow& window, const std::vector<std::string>& messages) {
    displayTextBox(window, messages,50);
}

void runMainMenu(sf::RenderWindow& window, bool& startGame, Player& player) {
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\determination-extended\\determination-extended.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return;
    }

    std::array<sf::Text, 3> menuOptions;
    menuOptions[0].setFont(font);
    menuOptions[0].setCharacterSize(50);
    menuOptions[0].setString("New Game");
    menuOptions[0].setPosition(150, 400);

    menuOptions[1].setFont(font);
    menuOptions[1].setCharacterSize(50);
    menuOptions[1].setString("Load Game");
    menuOptions[1].setPosition(150, 470);

    menuOptions[2].setFont(font);
    menuOptions[2].setCharacterSize(50);
    menuOptions[2].setString("Quit");
    menuOptions[2].setPosition(150, 540);

    sf::Text info;
    info.setFont(font);
    info.setCharacterSize(30);
    info.setString("PJC Project");
    info.setPosition(450, 920);
    info.setFillColor(sf::Color::White);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\Main_menu.png")) {
        std::cerr << "Error loading background texture" << std::endl;
        return;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    int selectedOption = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            selectedOption = (selectedOption + menuOptions.size() - 1) % menuOptions.size();
            sf::sleep(sf::milliseconds(200));  // Проти швидкої зміни опцій
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            selectedOption = (selectedOption + 1) % menuOptions.size();
            sf::sleep(sf::milliseconds(200));  // Проти швидкої зміни опцій
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            if (selectedOption == 0) { // New Game
                startGame = true;
                return;
            } else if (selectedOption == 1) { // Load Game
                //
            } else if (selectedOption == 2) { // Quit
                window.close();
                return;
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(info);

        for (int i = 0; i < menuOptions.size(); ++i) {
            if (i == selectedOption) {
                menuOptions[i].setFillColor(sf::Color::Yellow);
            } else {
                menuOptions[i].setFillColor(sf::Color::White);
            }
            window.draw(menuOptions[i]);
        }

        window.display();
    }
}

 void handleInnitiation(Location::InnitiateBox& box,sf::RenderWindow& window) {
    if(currentLocName=="Loc2"){
        switch (box.id) {
            case 0:


                if (box.enabled) {
                    if (!carTexture.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\Car.png")) {
                        std::cerr << "Error loading car texture!" << std::endl;
                        return;
                    }
                    carSprite.scale(6,6);
                    carSprite.setTexture(carTexture);
                    carSprite.setPosition(1980, 250);

                    carEnabled = true;
                    player.getSprite().rotate(90);
                    player.setMovementSpeed(0);
                    sf::sleep(sf::milliseconds(500));
                    player.setPosition({player.getPosition().x,(player.getPosition().y+60)});



                }


                box.enabled = false;
                break;
            case 1:
                if(box.enabled){
                    player.setPosition({player.getPosition().x,(player.getPosition().y+25)});
                    displayTextBox(window, {"Cant cross on red."});
                    displayTextBox(window, {"I'll wait."});
                    displayTextBox(window, {"........................................................"},200);
                    sf::sleep(sf::seconds(1));
                    displayTextBox(window, {"You know what?","F*ck it."});
                }
                box.enabled = false;

    }
    }else if(currentLocName=="Loc3") {
        switch (box.id) {
            case 0:
                if(box.enabled)
                displayTextBox(window, {"WTF!!!", "Where am I?"}, 80);
                box.enabled = false;
                break;
        }
    }

}


void interactLocation(Player& player, Location& currentLoc,std::map<std::string, Location>& locations,
                      std::string& currentLocName, std::string& currentInteractiveBoxLoc, int& currentInteractiveBoxID,sf::RenderWindow& window) {
    // Feet bounds
    for (const auto& box : currentLoc.getCollisionBoxes()) {
        if (player.getFeetBounds().intersects(box)) {
            player.revertPosition();
        }
    }

    // Transition boxes
    for (const auto& transition : currentLoc.getTransitionBoxes()) {
        if (player.getFeetBounds().intersects(transition.box)) {
            if(transition.id ==0 && transition.toLoc=="Loc3"){
                locToChange = true;
                currentLocName = transition.toLoc;
                currentLoc = locations[currentLocName];
                player.setPosition({1800,250});
            }else {
                locToChange = true;
                currentLocName = transition.toLoc;
                currentLoc = locations[currentLocName];
                player.setPosition(currentLoc.getSpawnPosition());
                break;
            }
        }
    }

    // Interactive boxes
    currentInteractiveBoxLoc = "";
    bool isInteractingBox = false;
    for (const auto& interactiveBox : currentLoc.getInteractiveBoxes()) {
        if (player.getFeetBounds().intersects(interactiveBox.box)) {
            currentInteractiveBoxLoc = interactiveBox.loc;
            currentInteractiveBoxID = interactiveBox.id;
            isInteractingBox = true;
            break;
        }
    }
    if (!isInteractingBox) {
        currentInteractiveBoxLoc = "";
    }

    // Innitiate boxes
    for (auto& innitiateBox : currentLoc.getInnitiateBoxes()) {
        if (player.getFeetBounds().intersects(innitiateBox.box)) {
            handleInnitiation(innitiateBox,window);
        }
    }




}


void startBattle(sf::RenderWindow& window,const int&monstrMaxHp,int& monstrCurrentHp) {
    sf::Clock battleClock;

    auto movementSpeed = 0.20f;
    sf::RectangleShape battleBox(sf::Vector2f(300, 300));
    battleBox.setFillColor(sf::Color::Black);
    battleBox.setOutlineThickness(5);
    battleBox.setOutlineColor(sf::Color::White);
    battleBox.setPosition((window.getSize().x - battleBox.getSize().x) / 2,
                          (window.getSize().y - battleBox.getSize().y) / 2 + 200); // Зміщення на 200 пікселів вниз

    sf::Texture heartTexture;

    if (!heartTexture.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\Animations\\serce6.PNG")) {
        std::cerr << "Error loading heart texture!" << std::endl;
        return;
    }

    sf::Text playerHpText;
    playerHpText.setFont(font);
    playerHpText.setCharacterSize(30);
    playerHpText.setFillColor(sf::Color::White);
    playerHpText.setPosition(600, window.getSize().y - 100);

    sf::Text enemyHpText;
    enemyHpText.setFont(font);
    enemyHpText.setCharacterSize(30);
    enemyHpText.setFillColor(sf::Color::White);
    enemyHpText.setPosition(1000, window.getSize().y - 100);


    sf::Sprite heartSprite;
    heartSprite.setTexture(heartTexture);
    heartSprite.setPosition(window.getSize().x / 2, window.getSize().y / 2 + 200); // Початкове положення враховує зміщення battleBox
    heartSprite.scale({0.15f, 0.15f});
    std::vector<EnemyAttack> enemyAttacks;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> sideDist(0, 3);
    std::uniform_int_distribution<int> posDistX(0, battleBox.getSize().x);
    std::uniform_int_distribution<int> posDistY(0, battleBox.getSize().y);

    sf::FloatRect heartBounds = heartSprite.getGlobalBounds();
    heartBounds.width /= 5.0f;
    heartBounds.height /= 5.0f;

    sf::Clock clock;
    while (window.isOpen()) {
        if(player.getCurrentHp()<=0){
            std::cout<<"Game Over";
            exit(0);
        }
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        for (auto it = enemyAttacks.begin(); it != enemyAttacks.end();) {
            sf::FloatRect attackBounds = it->shape.getGlobalBounds();
            if (heartBounds.intersects(attackBounds)) {

                it = enemyAttacks.erase(it);
                player.setCurrentHp(player.getCurrentHp()-2);
            } else {
                ++it;
            }
        }
        sf::Time battleTime = battleClock.getElapsedTime();
        if (battleTime.asSeconds() >=8.0f) {
            battleClock.restart();
            break;
        }
        playerHpText.setString("Your HP: " + std::to_string(player.getCurrentHp()) + "/" + std::to_string(player.getMaxHp()));
        enemyHpText.setString("Enemy HP: " + std::to_string(monstrCurrentHp) + "/" + std::to_string(monstrMaxHp));



        sf::Vector2f movement(0.f, 0.f);
        bool movingLeft = false, movingRight = false, movingUp = false, movingDown = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            movingLeft = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            movingRight = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            movingUp = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            movingDown = true;
        }

        float diagonalSpeed = movementSpeed / sqrt(2.0f);


        if (movingLeft && !movingRight) {
            if (movingUp && !movingDown) {
                heartSprite.move(-diagonalSpeed, -diagonalSpeed);

            } else if (movingDown && !movingUp) {
                heartSprite.move(-diagonalSpeed, diagonalSpeed);

            } else {
                heartSprite.move(-movementSpeed, 0);

            }
        } else if (movingRight && !movingLeft) {
            if (movingUp && !movingDown) {
                heartSprite.move(diagonalSpeed, -diagonalSpeed);

            } else if (movingDown && !movingUp) {
                heartSprite.move(diagonalSpeed, diagonalSpeed);

            } else {
                heartSprite.move(movementSpeed, 0);
            }
        } else if (movingUp && !movingDown) {
            heartSprite.move(0, -movementSpeed);

        } else if (movingDown && !movingUp) {
            heartSprite.move(0, movementSpeed);
        }
        sf::Vector2f heartPos = heartSprite.getPosition();
        if (heartPos.x < battleBox.getPosition().x) {
            heartPos.x = battleBox.getPosition().x;
        }
        if (heartPos.x + heartSprite.getGlobalBounds().width > battleBox.getPosition().x + battleBox.getSize().x) {
            heartPos.x = battleBox.getPosition().x + battleBox.getSize().x - heartSprite.getGlobalBounds().width;
        }
        if (heartPos.y < battleBox.getPosition().y) {
            heartPos.y = battleBox.getPosition().y;
        }
        if (heartPos.y + heartSprite.getGlobalBounds().height > battleBox.getPosition().y + battleBox.getSize().y) {
            heartPos.y = battleBox.getPosition().y + battleBox.getSize().y - heartSprite.getGlobalBounds().height;
        }
        heartSprite.setPosition(heartPos);


        if (clock.getElapsedTime().asSeconds() > 0.6f) {
            clock.restart();
            sf::CircleShape attackShape(5);
            attackShape.setFillColor(sf::Color::White);
            int side = sideDist(rng);
            sf::Vector2f startPosition;
            switch (side) {
                case 0:
                    startPosition = {battleBox.getPosition().x - attackShape.getRadius() * 2,
                                     battleBox.getPosition().y + posDistY(rng)};
                    break;
                case 1:
                    startPosition = {battleBox.getPosition().x + battleBox.getSize().x,
                                     battleBox.getPosition().y + posDistY(rng)};
                    break;
                case 2:
                    startPosition = {battleBox.getPosition().x + posDistX(rng),
                                     battleBox.getPosition().y - attackShape.getRadius() * 2};
                    break;
                case 3:
                    startPosition = {battleBox.getPosition().x + posDistX(rng),
                                     battleBox.getPosition().y + battleBox.getSize().y};
                    break;
            }
            attackShape.setPosition(startPosition);

            sf::Vector2f targetPosition = {battleBox.getPosition().x + posDistX(rng),
                                           battleBox.getPosition().y + posDistY(rng)};
            sf::Vector2f direction = targetPosition - startPosition;
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length != 0) {
                direction /= length;
            }


            enemyAttacks.push_back({attackShape, direction});
        }

        for (auto& attack : enemyAttacks) {
            attack.shape.move(attack.direction * 0.1f);
        }

        window.clear();
        window.draw(battleBox);
        window.draw(enemyHpText);
        window.draw(playerHpText);
        window.draw(heartSprite);

        for (const auto& attack : enemyAttacks) {
            window.draw(attack.shape);
        }

        window.display();
    }
}

void battleWindow(sf::RenderWindow& window) {
    bool leave(false);
    const auto monstrMaxHp = 12;
    auto monstrCurrentHP = monstrMaxHp;
    sf::RectangleShape battleBox(sf::Vector2f(300, 300));
    battleBox.setFillColor(sf::Color::Black);
    battleBox.setOutlineThickness(5);
    battleBox.setOutlineColor(sf::Color::White);
    battleBox.setPosition(790,
                          600);


    std::vector<std::string> options = {"Escape", "Use Heal", "Attack"};

    sf::Texture monstrTexture;
    sf::Sprite monstrSp;
    if(monstrTexture.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\Monstr"+std::to_string(std::rand()%2+1)+".png")){

    }
    monstrSp.setTexture(monstrTexture);
    monstrSp.setPosition(0,-38);


    std::vector<sf::Text> optionTexts;
    int selectedOption = 0;

    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(options[i]);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);
        text.setPosition(600 + i * 250, window.getSize().y - 50);
        optionTexts.push_back(text);
    }
    sf::Text playerHpText;
    playerHpText.setFont(font);
    playerHpText.setCharacterSize(30);
    playerHpText.setFillColor(sf::Color::White);
    playerHpText.setPosition(600, window.getSize().y - 100);

    sf::Text enemyHpText;
    enemyHpText.setFont(font);
    enemyHpText.setCharacterSize(30);
    enemyHpText.setFillColor(sf::Color::White);
    enemyHpText.setPosition(1000, window.getSize().y - 100);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Right) {
                    selectedOption = (selectedOption + 1) % options.size();
                } else if (event.key.code == sf::Keyboard::Left) {
                    selectedOption = (selectedOption - 1 + options.size()) % options.size();
                } else if (event.key.code == sf::Keyboard::Enter) {
                    if (options[selectedOption] == "Attack") {
                        monstrCurrentHP-=player.getCurrentdamage();
                        if(monstrCurrentHP>0) {
                            startBattle(window, monstrMaxHp, monstrCurrentHP);
                        }
                        else{
                            player.setCurrentXp(player.getCurrentXp()+90);
                        if(player.getCurrentXp()>=player.getXpToNextLvl())
                        {
                            player.setCurrentXp(0);
                            player.setXpToNextLvl(player.getXpToNextLvl()+20);
                            player.lvl +=1;
                        }
                            player.money+=20;
                            leave=true;
                            break;
                        }



                    }else if(options[selectedOption] == "Escape"){
                        leave=true;
                    }else if(options[selectedOption] == "Use Heal"){
                        if(player.checkItem(Items::HealPotion)){
                            player.setCurrentHp(player.getCurrentHp()+50);
                            if(player.getCurrentHp()>=player.getMaxHp()){
                                player.setCurrentHp(player.getMaxHp());
                            }
                            player.removeItem(Items::HealPotion);
                        }
                    }

                }
            }
        }

        for (size_t i = 0; i < optionTexts.size(); ++i) {
            if (i == selectedOption) {
                optionTexts[i].setFillColor(sf::Color::Yellow);
            } else {
                optionTexts[i].setFillColor(sf::Color::White);
            }
        }
        playerHpText.setString("Your HP: " + std::to_string(player.getCurrentHp()) + "/" + std::to_string(player.getMaxHp()));
        enemyHpText.setString("Enemy HP: " + std::to_string(monstrCurrentHP) + "/" + std::to_string(monstrMaxHp));



        window.clear();
        window.draw(battleBox);
        window.draw(monstrSp);
        window.draw(playerHpText);
        window.draw(enemyHpText);
        for (const auto& text : optionTexts) {
            window.draw(text);
        }
        if(leave)
            break;


        window.display();
    }
}

void openInventoryMenu(sf::RenderWindow& window, Player& player, const sf::RectangleShape& quickmenu, const sf::RectangleShape& infoMenu, const std::vector<sf::Text*>& menuOptions, const std::vector<sf::Text*>& infoPanel) {
    sf::RectangleShape inventoryMenu(sf::Vector2f(220, 350));
    inventoryMenu.setFillColor(sf::Color::Black);
    inventoryMenu.setOutlineColor(sf::Color::White);
    inventoryMenu.setOutlineThickness(5);
    inventoryMenu.setPosition(420, 100);

    sf::Text inventoryInfo("Inventory: " + std::to_string(player.getInventorySize()) + " / " + std::to_string(player.getInventoryCapacity()), font, 20);
    inventoryInfo.setPosition(440, 110);

    std::vector<Items> items = player.getInventory();
    std::vector<sf::Text> itemTexts;

    for (size_t i = 0; i < items.size(); ++i) {
        sf::Text itemText(itemToString(items[i]), font, 20);
        itemText.setPosition(440, 140 + i * 30);
        itemTexts.push_back(itemText);
    }

    int selectedItem = 0;
    itemTexts[selectedItem].setFillColor(sf::Color::Yellow);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    return;
                }
                if (event.key.code == sf::Keyboard::Up) {
                    itemTexts[selectedItem].setFillColor(sf::Color::White);
                    selectedItem = (selectedItem - 1 + itemTexts.size()) % itemTexts.size();
                    itemTexts[selectedItem].setFillColor(sf::Color::Yellow);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    itemTexts[selectedItem].setFillColor(sf::Color::White);
                    selectedItem = (selectedItem + 1) % itemTexts.size();
                    itemTexts[selectedItem].setFillColor(sf::Color::Yellow);
                }
            }
        }

        window.clear();
        displayLocation(window, currentLoc);
        player.draw(window);
        window.draw(quickmenu);
        window.draw(locSp);
        window.draw(infoMenu);
        for (const auto& info : infoPanel) {
            window.draw(*info);
        }


        window.draw(inventoryMenu);
        window.draw(inventoryInfo);
        for (const auto& itemText : itemTexts) {
            window.draw(itemText);
        }
        window.display();
    }
}

void displayQuickMenu(sf::RenderWindow& window) {
    sf::RectangleShape infoMenu(sf::Vector2f(200, 150));
    sf::RectangleShape quickmenu(sf::Vector2f(200, 300));
    infoMenu.setFillColor(sf::Color::Black);
    infoMenu.setOutlineColor(sf::Color::White);
    infoMenu.setOutlineThickness(5);
    infoMenu.setPosition(200, 50);

    quickmenu.setFillColor(sf::Color::Black);
    quickmenu.setOutlineColor(sf::Color::White);
    quickmenu.setOutlineThickness(5);
    quickmenu.setPosition(200, 200);


    sf::Text monInfo("Money: " + std::to_string(player.money), font, 20);
    sf::Text hpInfo("Hp: " + std::to_string(player.getCurrentHp()) + " / " + std::to_string(player.getMaxHp()), font, 20);
    sf::Text dmgInfo("Damage: " + std::to_string(player.getCurrentdamage()), font, 20);
    sf::Text xpInfo("Xp: " + std::to_string(player.getCurrentXp()) + " / " + std::to_string(player.getXpToNextLvl())+", Lvl: "+std::to_string(player.lvl), font, 20);

    monInfo.setPosition(220,70);
    hpInfo.setPosition(220,105);
    dmgInfo.setPosition(220,135);
    xpInfo.setPosition(220,165);

    sf::Text resumeText("Resume", font, 24);
    sf::Text inventoryText("Inventory", font, 24);
    sf::Text quitGameText("Quit Game", font, 24);

    resumeText.setPosition(220, 220);
    inventoryText.setPosition(220, 260);
    quitGameText.setPosition(220, 300);

    std::vector<sf::Text*> infoPanel = {&hpInfo, &dmgInfo, &xpInfo,&monInfo};
    std::vector<sf::Text*> menuOptions = {&resumeText, &inventoryText, &quitGameText};
    int selectedOption = 0;
    menuOptions[selectedOption]->setFillColor(sf::Color::Yellow);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    menuOptions[selectedOption]->setFillColor(sf::Color::White);
                    selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
                    menuOptions[selectedOption]->setFillColor(sf::Color::Yellow);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    menuOptions[selectedOption]->setFillColor(sf::Color::White);
                    selectedOption = (selectedOption + 1) % menuOptions.size();
                    menuOptions[selectedOption]->setFillColor(sf::Color::Yellow);
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedOption == 0) {
                        // Resume the game
                        return;
                    } else if (selectedOption == 1) {
                        openInventoryMenu(window, player,quickmenu,infoMenu,menuOptions,infoPanel);
                    } else if (selectedOption == 2) {
                        window.close();
                        return;
                    }
                }
            }
        }

        window.clear();
        window.draw(locSp);
        displayLocation(window, currentLoc);
        player.draw(window);
        window.draw(quickmenu);
        window.draw(infoMenu);
        for(const auto& info : infoPanel){
            window.draw(*info);
        }
        for (const auto& option : menuOptions) {
            window.draw(*option);
        }
        window.display();
    }
}
void displayShop(sf::RenderWindow& window) {
    sf::RectangleShape quickmenu(sf::Vector2f(300, 300));

    quickmenu.setFillColor(sf::Color::Black);
    quickmenu.setOutlineColor(sf::Color::White);
    quickmenu.setOutlineThickness(5);
    quickmenu.setPosition(200, 200);


    sf::Text monInfo("Money: " + std::to_string(player.money), font, 20);

    monInfo.setPosition(220,70);


    sf::Text Pos1("Sword (30)", font, 24);
    sf::Text Pos2("Health Essention (35)", font, 24);
    sf::Text Pos3("Heal Potion", font, 24);

    Pos1.setPosition(220, 220);
    Pos2.setPosition(220, 260);
    Pos3.setPosition(220, 300);


    std::vector<sf::Text*> Options = {&Pos1, &Pos2, &Pos3};
    int selectedOption = 0;
    Options[selectedOption]->setFillColor(sf::Color::Yellow);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    Options[selectedOption]->setFillColor(sf::Color::White);
                    selectedOption = (selectedOption - 1 + Options.size()) % Options.size();
                    Options[selectedOption]->setFillColor(sf::Color::Yellow);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    Options[selectedOption]->setFillColor(sf::Color::White);
                    selectedOption = (selectedOption + 1) % Options.size();
                    Options[selectedOption]->setFillColor(sf::Color::Yellow);
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedOption == 0) {
                        //
                        return;
                    } else if (selectedOption == 1) {
                        //
                    } else if (selectedOption == 2) {
                        //

                    }
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    return;
                }
            }
        }

        window.clear();
        window.draw(locSp);
        displayLocation(window, currentLoc);
        player.draw(window);
        window.draw(quickmenu);

        for (const auto& option : Options) {
            window.draw(*option);
        }
        window.display();
    }
}

void handleInteraction(const std::string& loc ,const int& interactiveBoxId, sf::RenderWindow& window) {


    if(loc == "Loc1"){
        int randomKeyID;
        do {
            randomKeyID = 0 + (rand() % 6);
        } while(randomKeyID == 2 || randomKeyID == 4);

        std::string objWithoutKeys = "Nothing.";
        std::string objWithKeys = "You found keys.";



        switch (interactiveBoxId) {
            case 0:
                if(!player.keysFound && randomKeyID == 0) {
                    displayTextBox(window, {"Just my old closet.","I dont use it anyway.","Looked inside*", objWithKeys});
                    player.addItem(Items::Keys);
                    player.keysFound = true;
                } else {
                    displayTextBox(window, {"Just my old closet.","I dont use it anyway.","Looked inside*", objWithoutKeys});
                }
                break;
            case 1:
                if(!player.keysFound && randomKeyID == 0) {
                    displayTextBox(window, {"My beer throne.","Searching*", objWithKeys});
                    player.addItem(Items::Keys);
                    player.keysFound = true;
                } else {
                    displayTextBox(window, {"My beer throne.","Searching*", objWithoutKeys});

                }
                break;
            case 2:

                displayTextBox(window, {"My TV.","Cool to have one."});
                battleWindow(window);
                break;
            case 3:
                if(!player.keysFound && randomKeyID == 0) {
                    displayTextBox(window, {"The bed I've been sleeping on for the past 20 years","Searching*", objWithKeys});
                    player.addItem(Items::Keys);
                    player.keysFound = true;
                } else {
                    displayTextBox(window, {"The bed I've been sleeping on for the past 20 years","Searching*", objWithoutKeys});
                }
                break;
            case 4:
                displayTextBox(window, {"Just my phone on the table"});
                break;
            case 5:
                if(!player.keysFound && randomKeyID == 0) {
                    displayTextBox(window, {"My flip-flops.","Those are cool", "seeking*", objWithKeys});
                    player.addItem(Items::Keys);
                    player.keysFound = true;
                } else {
                    displayTextBox(window, {"My flip-flops.","Those are cool", "seeking*", objWithoutKeys});

                }
                break;
            case 6:
                if(player.checkItem(Items::Keys)) {
                    displayTextBox(window, {"Yeah"});
                    currentLocName = "Loc2";
                    player.setPosition(locations[currentLocName].spawnPosition);
                    locToChange = true;

                } else {
                    displayTextBox(window, {"I have no keys.", "I should find 'em."});
                }
                break;
        }
    }else if(loc == "Loc5"){
        if(interactiveBoxId==1){
            displayShop(window);
        }

    }
    sf::sleep(sf::milliseconds(300));
}


void playLoc(sf::RenderWindow& window){
    sf::Clock battleClock;
    sf::Clock clock;
    player.setPosition(currentLoc.getSpawnPosition());

    std::string currentInteractiveBoxLoc;
    int currentInteractiveBoxId;

    while (window.isOpen()) {


        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }


        sf::Texture tex = sf::Texture();
        if (tex.loadFromFile(currentLoc.texturePath)) {
            currentLoc.setTexture(tex);
        }
        locSp.setTexture(currentLoc.getTexture());


        if (dis) {
            displayTextBox(window,{"Terrible nightmare...", "I have to buy some beer."});
            dis = false;
        }

        while (window.isOpen()) {
                if(locToChange) {
                    currentLoc = locations[currentLocName];
                    tex = sf::Texture();
                    locToChange = false;
                    break;
                }
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        displayQuickMenu(window);
                    }


                }


            if(currentLocName=="Loc3"||currentLocName=="Loc4"||currentLocName=="Loc5"){
                sf::Time battleTime = battleClock.getElapsedTime();
                if (battleTime.asSeconds() >=30.0f) {
                    battleClock.restart();
                    break;
                }
            }

            player.setCurrentdamage(player.getBaseDamage()+2*player.lvl);
            player.setMaxHp(player.basehp+20*player.lvl);

                float deltaTime = clock.restart().asSeconds();

                player.handleInput();
                player.update(deltaTime);

                player.setIsInteracting(true);
                interactLocation(player, currentLoc, locations, currentLocName, currentInteractiveBoxLoc, currentInteractiveBoxId, window);

                window.clear();
                if(currentLocName=="Loc2") {
                    if (carEnabled) {
                        carSprite.move(-2000 * deltaTime, 0);

                        if (carSprite.getPosition().x <= 0) {
                            carEnabled = false;
                            currentLocName = "Loc3";
                            player.setPosition(locations[currentLocName].getSpawnPosition());

                            locToChange = true;
                            player.getSprite().rotate(-90);
                            player.setMovementSpeed(0.20f);
                        }
                    }
                }
                window.draw(locSp);
                displayLocation(window, currentLoc);
                if (carEnabled) {
                    window.draw(carSprite);
                }


                player.draw(window);




                if (!currentInteractiveBoxLoc.empty()) {
                    player.setInteractionActive(true);
                } else {
                    player.setInteractionActive(false);
                }

                if (player.getInteractionActive() && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    handleInteraction(currentInteractiveBoxLoc, currentInteractiveBoxId, window);
                }
                window.display();
            }
        }

}
auto main()->int {
    sf::RenderWindow window(sf::VideoMode(1980, 1080), "Hearted", sf::Style::Fullscreen,
                            sf::ContextSettings(0, 0, 8));
    if (!font.loadFromFile("C:\\Users\\Yevhen\\Desktop\\Project_C++\\determination-extended\\determination-extended.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    player.addItem(Items::HealPotion);
    bool startGame = false;

    // Tworzenie hitboxów
    locations["Loc1"] = Location("C:\\Users\\Yevhen\\Desktop\\Project_C++\\locations\\Home.png", {90, 550});
    locations["Loc2"] = Location("C:\\Users\\Yevhen\\Desktop\\Project_C++\\locations\\Ulica.png", {10, 850});
    locations["Loc3"] = Location("C:\\Users\\Yevhen\\Desktop\\Project_C++\\locations\\Loc3.png", {450, 350});
    locations["Loc4"] = Location("C:\\Users\\Yevhen\\Desktop\\Project_C++\\locations\\Loc4.png", {30, 350});
    locations["Loc5"] = Location("C:\\Users\\Yevhen\\Desktop\\Project_C++\\locations\\Loc5.png", {50, 350});

    locations["Loc1"].addCollisionBox(sf::FloatRect(30,75,1200,100));
    locations["Loc1"].addCollisionBox(sf::FloatRect(155,105,105,100));
    locations["Loc1"].addCollisionBox(sf::FloatRect(15,75,10,1200));
    locations["Loc1"].addCollisionBox(sf::FloatRect(30,1040,2000,10));
    locations["Loc1"].addCollisionBox(sf::FloatRect(1130,75,10,425));
    locations["Loc1"].addCollisionBox(sf::FloatRect(1130,500,780,10));
    locations["Loc1"].addCollisionBox(sf::FloatRect(1550,515,220,10));
    locations["Loc1"].addCollisionBox(sf::FloatRect(1875,500,10,600));
    locations["Loc1"].addCollisionBox(sf::FloatRect(80,777,145,200));
    locations["Loc1"].addCollisionBox(sf::FloatRect(225,968,145,200));
    locations["Loc1"].addCollisionBox(sf::FloatRect(1100,225,60,230));
    locations["Loc1"].addCollisionBox(sf::FloatRect(620,200,175,205));



    locations["Loc1"].addInteractiveBox(sf::FloatRect(120, 100, 200, 130), "Loc1",0);
    locations["Loc1"].addInteractiveBox(sf::FloatRect(610, 200, 200, 250), "Loc1",1);
    locations["Loc1"].addInteractiveBox(sf::FloatRect(1080, 210, 200, 280), "Loc1",2);
    locations["Loc1"].addInteractiveBox(sf::FloatRect(50, 750, 200, 150), "Loc1",3);
    locations["Loc1"].addInteractiveBox(sf::FloatRect(200, 940, 200, 150), "Loc1",4);
    locations["Loc1"].addInteractiveBox(sf::FloatRect(1520, 430, 300, 150), "Loc1",5);
    locations["Loc1"].addInteractiveBox(sf::FloatRect(1790, 700, 300, 150), "Loc1",6);

    locations["Loc2"].addInnitiateBox(sf::FloatRect(1530,130,200,300), true,0);
    locations["Loc2"].addCollisionBox(sf::FloatRect(0,850,1510,10));
    locations["Loc2"].addCollisionBox(sf::FloatRect(0,1060,2000,10));
    locations["Loc2"].addCollisionBox(sf::FloatRect(1505,100,5,750));
    locations["Loc2"].addCollisionBox(sf::FloatRect(1755,100,5,750));
    locations["Loc2"].addCollisionBox(sf::FloatRect(1760,850,200,10));
    locations["Loc2"].addInnitiateBox(sf::FloatRect(1530,850,200,5), true,1);

    locations["Loc3"].addInnitiateBox(sf::FloatRect(450,490,200,100), true,0);

    locations["Loc3"].addCollisionBox(sf::FloatRect(0,830,2000,10));
    locations["Loc3"].addCollisionBox(sf::FloatRect(310,35,2000,5));
    locations["Loc3"].addCollisionBox(sf::FloatRect(1840,530,5,400));
    locations["Loc3"].addCollisionBox(sf::FloatRect(58,180,5,800));
    locations["Loc3"].addCollisionBox(sf::FloatRect(305,50,5,60));
    locations["Loc3"].addCollisionBox(sf::FloatRect(182,110,5,60));
    locations["Loc3"].addCollisionBox(sf::FloatRect(182,110,120,5));
    locations["Loc3"].addCollisionBox(sf::FloatRect(182,110,120,5));
    locations["Loc3"].addCollisionBox(sf::FloatRect(55,175,125,5));
    locations["Loc3"].addCollisionBox(sf::FloatRect(1865,0,5,255));
    locations["Loc3"].addCollisionBox(sf::FloatRect(1860,260,300,3));
    locations["Loc3"].addCollisionBox(sf::FloatRect(1860,523,300,3));

    locations["Loc3"].addTransitionBox(sf::FloatRect(1920,270,300,250),"Loc4",0);

    locations["Loc4"].addTransitionBox(sf::FloatRect(-100,270,2,250),"Loc3",0);

    locations["Loc4"].addTransitionBox(sf::FloatRect(1980,270,3,250),"Loc5",1);

    locations["Loc4"].addCollisionBox(sf::FloatRect(0,260,2000,3));
    locations["Loc4"].addCollisionBox(sf::FloatRect(0,520,2000,3));

    locations["Loc5"].addTransitionBox(sf::FloatRect(-100,270,2,250),"Loc3",0);
    locations["Loc5"].addCollisionBox(sf::FloatRect(50,0,2,270));
    locations["Loc5"].addCollisionBox(sf::FloatRect(50,520,2,450));
    locations["Loc5"].addCollisionBox(sf::FloatRect(50,970,1820,450));
    locations["Loc5"].addCollisionBox(sf::FloatRect(1850,980,3,-975));
    locations["Loc5"].addCollisionBox(sf::FloatRect(50,0,1800,2));
    locations["Loc5"].addCollisionBox(sf::FloatRect(720,145,570,3));
    locations["Loc5"].addInteractiveBox(sf::FloatRect(830,150,300,300),"Loc5",0);

    runMainMenu(window, startGame, player);

    playLoc(window);

    return 0;
}
