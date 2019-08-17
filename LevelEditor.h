#include "Game.h"

class EditorPlatform : public Platform {
public:
	EditorPlatform(PlatformType t, int spriteT = 0) : Platform(t, spriteT) {}
	int id;

	sf::Sprite& render(int _x) {
        sprite.setPosition(x - _x * PLATSIZE, y);
        return sprite;
	}
};

sf::Texture HIGHLIGHT;

class Highlight : public RenderObject {
public:
    Highlight() {
        x = 7 * PLATSIZE;
        y = 7 * PLATSIZE;
        sprite.setTexture(HIGHLIGHT);
        sprite.setTextureRect(sf::IntRect(0, 0, 21, 21));
    }
};

const int PLATFORMS[][2] = {{1, 0}, {1, 1}, {2, 0}, {2, 1}, {2, 2}, {3, 0}, {4, 0}, {5, 0}};

class LevelEditor {
	sf::RenderWindow window;

	bool isEditorActive;
    set<EditorPlatform*> platforms;
    Highlight border;
    int curx;
    int cury;

	void windowUpdate() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				isEditorActive = 0;
			}
		}
		if(!isEditorActive) {
			window.close();
		}
		isEditorActive = isEditorActive && window.isOpen();
	}

	void frame() {
		windowUpdate();
        update();
        render();
	}
	void add(int id) {
		id = (id + 9) % 9;
		if(id < 8) {
			EditorPlatform* pl = new EditorPlatform((PlatformType)PLATFORMS[id][0], PLATFORMS[id][1]);
			pl->id = id;
			pl->setX(curx * PLATSIZE);
			pl->setY(cury * PLATSIZE);
			platforms.insert(pl);
		}
	}

	void getPL(int x, int y, bool change) {
		EditorPlatform* pl = nullptr;
		EditorPlatform* cur = nullptr;
		for(auto i : platforms) {
			if(i->getX() / PLATSIZE == x && i->getY() / PLATSIZE == y) {
				pl = i;
			}
			if(i->getX() / PLATSIZE == curx && i->getY() / PLATSIZE == cury) {
				cur = i;
			}
		}
		int id = 8;
		if(pl) {
			id = pl->id;
			if(change) {
				platforms.erase(pl);
				delete(pl);
			}
			else if(cur) {
				platforms.erase(cur);
				delete(cur);
			}
		}
		if(change) {
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				id--;
			}
			else {
				id++;
			}
		}
		add(id);
	}

	void update() {
        bool isKeyPressed = 0;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			isKeyPressed = 1;
			cury--;
			if(cury < 0) {
				cury = 14;
			}
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			isKeyPressed = 1;
			cury++;
			if(cury > 14) {
				cury = 0;
			}
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			isKeyPressed = 1;
			curx--;
			curx = max(0, curx);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			isKeyPressed = 1;
			curx++;
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
			isKeyPressed = 1;
			getPL(curx, cury, 1);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            string name;
            cin >> name;
            ofstream out(name);
            for(auto i : platforms) {
                out << i->getX() / PLATSIZE << " " << i->getY() / PLATSIZE << " " << PLATFORMS[i->id][0] << " " << PLATFORMS[i->id][1] << endl;
            }
            cout << "Saved" << endl;
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			isKeyPressed = 1;
			getPL(curx, cury - 1, 0);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			isKeyPressed = 1;
			getPL(curx - 1, cury, 0);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			isKeyPressed = 1;
			getPL(curx + 1, cury, 0);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			isKeyPressed = 1;
			getPL(curx, cury + 1, 0);
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
			string s;
			cin >> s;
			ifstream inmap(s);
			while(!inmap.eof() && inmap.good()) {
				PlatINFO inf;
				int t;
				inmap >> inf.x >> inf.y >> t >> inf.spriteType;
				inf.type = PlatformType(t);
				EditorPlatform *pl = new EditorPlatform(inf.type, inf.spriteType);
				pl->setX(inf.x * PLATSIZE);
				pl->setY(inf.y * PLATSIZE);
				platforms.insert(pl);
			}
		}
		if(isKeyPressed) {
			sf::Time t = sf::milliseconds(200);
			sf::sleep(t);
		}
	}

	void render() {
        int cur = curx - 7;
        cur = max(0, cur);
        window.clear(sf::Color::White);
        for(auto i : platforms) {
            window.draw(i->render(cur));
        }
        border.setX(min(curx, 7) * PLATSIZE);
        border.setY(cury * PLATSIZE);
        window.draw(border.render());
        window.display();
	}
public:
	LevelEditor() : window(sf::VideoMode(42 * 15, 42 * 15), "Editor") {
		PLAT_NORMAL.loadFromFile("normal.png");
		PLAT_FLAT.loadFromFile("flat.png");
		PLAT_DAMAGE.loadFromFile("damage.png");
		PLAT_JUMPING.loadFromFile("jump.png");
		PLAT_FALLING.loadFromFile("falling.png");
		PLAYER.loadFromFile("player.png");
		BACKGROUND.loadFromFile("background.png");
		HEART.loadFromFile("heart.png");
		GOLDENHEART.loadFromFile("goldenheart.png");
		HIGHLIGHT.loadFromFile("border.png");
		isEditorActive = 1;
		curx = 7;
	}

	void run() {
        while(isEditorActive) {
			frame();
        }
	}
};
