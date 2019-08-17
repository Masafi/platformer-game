#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace std;

//SETTINGS
float GAMESPEED = 50;
const int GRAVITY = 500;
const int XMAXSPEED = 1000;
const int YMAXSPEED = 1000;
const int JUMPSPEED = 0.7 * GRAVITY;
const int XACCEL = 150;
const int PLATSIZE = 42;
const int SCREENWIDTH = 30 * PLATSIZE;
const int SCREENHEIGHT = 16 * PLATSIZE;
const bool EASYMODE = 1;
//SETTINGS

//TEXTURES
sf::Texture PLAT_NORMAL;
sf::Texture PLAT_FALLING;
sf::Texture PLAT_JUMPING;
sf::Texture PLAT_FLAT;
sf::Texture PLAT_DAMAGE;
sf::Texture PLAYER;
sf::Texture BACKGROUND;
sf::Texture HEART;
sf::Texture GOLDENHEART;

enum class PlatformType {
	Air,
	Flat,
	Normal,
	Damage,
	Falling,
	Jumping
};

class RenderObject {
protected:
	float x;
	float y;
	int width;
	int height;
	int row;

	sf::Sprite sprite;

	bool animated;
	bool started;
    vector<float> timing;
    float currentTime;
    float lastTime;
	int lastFrame;
	bool cycled;
public:
	RenderObject() {
		x = 0;
		y = 0;
		sprite.scale(2, 2);
		started = 0;
		animated = 0;
		timing.resize(0);
		currentTime = 0;
		lastFrame = 0;
		row = 0;
		cycled = 0;
	}

	float getX() {return x;}
	float getY() {return y;}
	int getWidth() {return width;}
	int getHeight() {return height;}

	void setX(float v) {x = v;}
	void setY(float v) {y = v;}
	void setWidth(int v) {width = v;}
	void setHeight(int v) {height = v;}

	virtual sf::Sprite& render() {
		sprite.setPosition((int)x, (int)y);
		return sprite;
	}

	void startAnimation() {
		if(!animated || started) {
            return;
		}
		started = 1;
		lastFrame = 0;
		currentTime = 0;
		lastTime = 0;
	}

	void stopAnimation() {
		if(!animated || !started) {
            return;
		}
		started = 0;
		sprite.setTextureRect(sf::IntRect(0, row * height / 2, width / 2, height / 2));
	}


	void updateAnimation(float dt) {
		if(!animated || !started) {
            return;
		}
		currentTime += dt;
        for(int i = lastFrame; i < timing.size(); i++) {
            if(lastTime + timing[i] < currentTime) {
				lastTime += timing[i];
				if(i == timing.size() - 1 && !cycled || i < timing.size()) {
					sprite.setTextureRect(sf::IntRect(((i + 1) % timing.size()) * width / 2, row * height / 2, width / 2, height / 2));
                }
                lastFrame = i + 1;
            }
            else {
				break;
            }
        }
        if(lastFrame >= timing.size()) {
			started = 0;
        }
	}
};

class Platform : public RenderObject {
protected:
	PlatformType type;

	bool falling;
public:
	Platform() : RenderObject() {
		type = PlatformType::Air;
		width = PLATSIZE;
		height = PLATSIZE;
		falling = 0;
	}

	Platform(PlatformType t, int spriteT = 0) : Platform() {
		type = t;
		row = spriteT;
		switch(type) {
		case PlatformType::Air:
			break;
		case PlatformType::Flat:
			sprite.setTexture(PLAT_FLAT);
			break;
		case PlatformType::Normal:
			sprite.setTexture(PLAT_NORMAL);
			break;
		case PlatformType::Damage:
			sprite.setTexture(PLAT_DAMAGE);
			break;
		case PlatformType::Jumping:
			sprite.setTexture(PLAT_JUMPING);
			animated = 1;
			timing.push_back(0.0);
			timing.push_back(0.5);
			cycled = 1;
			break;
		case PlatformType::Falling:
			sprite.setTexture(PLAT_FALLING);
			animated = 1;
			const float FALLINGTIME = 0.2;
			timing.push_back(FALLINGTIME);
			timing.push_back(FALLINGTIME);
			timing.push_back(FALLINGTIME);
			timing.push_back(FALLINGTIME);
			timing.push_back(FALLINGTIME);
			cycled = 0;
			break;
		}
		sprite.setTextureRect(sf::IntRect(0, row * height / 2, width / 2, height / 2));
	}

	PlatformType getType() {
		return type;
	}

	bool update(float dt) {
		x -= dt * GAMESPEED;
		if(type != PlatformType::Falling) {
			return x + width >= 0;
		}
		else {
			if(x + width < 0) {
				return false;
			}
            if(started) {
				falling = 1;
				return true;
            }
            else {
				return !falling;
            }
		}
	}
};

typedef set<Platform*> Platforms;
typedef pair<float, float> point;

class Player : public RenderObject {
	int hp;
	float xSp;
	float ySp;

	bool isJumping;
	float timer;
	bool dir;
public:
	Player() : RenderObject() {
		hp = 3;
		xSp = 0;
		ySp = 0;
		timer = 0;
		width = 36;
		height = 42;
		isJumping = 0;
		sprite.setTexture(PLAYER);
		sprite.setTextureRect(sf::IntRect(0, 0, width / 2, height / 2));
		sprite.scale(0.5, 0.5);
		sprite.setOrigin((float)width / 4, (float)height / 4);
		sprite.scale(2, 2);
		animated = 1;
		const float PLAYERTIMING = 0.1F;
		timing.push_back(PLAYERTIMING);
		timing.push_back(PLAYERTIMING);
		timing.push_back(PLAYERTIMING);
		timing.push_back(PLAYERTIMING);
		timing.push_back(PLAYERTIMING);
		dir = 0;
	}

    int getHP() {return hp;}

	bool update(float dt, Platforms &platforms) {
		PlatformType type = PlatformType::Air;
		Platform *pl;
		for(auto i : platforms) {
			sf::FloatRect cur(x + 4, y + height - 1, width - 8, 2);
			sf::FloatRect plat(i->getX(), i->getY(), i->getWidth(), 2);
			if(cur.intersects(plat)) {
				if(i->getType() == PlatformType::Falling) {
                    i->startAnimation();
				}
				if((int)i->getType() > (int)type) {
					pl = i;
				}
				type = PlatformType(max((int)type, (int)i->getType()));

			}
		}
		switch(type) {
		case PlatformType::Air:
			ySp += GRAVITY * dt;
			break;
		case PlatformType::Falling:
		case PlatformType::Flat:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				y += 2;
				break;
			}
		case PlatformType::Jumping:
		case PlatformType::Normal:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				if(!isJumping) {
					y -= 2;
					if(type == PlatformType::Jumping) {
						ySp = -1.5 * JUMPSPEED;
						pl->startAnimation();
					}
					else {
						ySp = -JUMPSPEED;
					}
					isJumping = 1;
				}
			}
			else {
				isJumping = 0;
				ySp = min(ySp, 0.0F);
			}
			break;
		case PlatformType::Damage:
			if(!EASYMODE) {
				hp--;
			}
			ySp = -JUMPSPEED;
			y -= 2;
			timer = 1;
			break;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			xSp = -XACCEL;
			if(!dir) {
                sprite.scale(-1, 1);
			}
			dir = 1;
			startAnimation();
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			xSp = XACCEL;
			if(dir) {
                sprite.scale(-1, 1);
			}
			dir = 0;
			startAnimation();
		}
		xSp = min(xSp, (float)XMAXSPEED);
		xSp = max(xSp, -(float)XMAXSPEED);
		ySp = min(ySp, (float)YMAXSPEED);
		ySp = max(ySp, -(float)YMAXSPEED);
		timer -= dt;
		timer = max(0.0F, timer);
		if(((int)(timer / 0.5)) % 2) {
			sprite.setColor(sf::Color(255, 50, 50));
		}
		else {
			sprite.setColor(sf::Color::White);
		}
		float nx = x + xSp * dt;
		nx = max(nx, 0.0F);
		nx = min(nx, (float)SCREENWIDTH - width);
		float ny = y + ySp * dt;
		if(ny < 0) {
            ny = 0;
            ySp = 0;
		}
		for(auto i : platforms) {
			sf::FloatRect rect(nx, ny, width, height);
			sf::FloatRect plat(i->getX(), i->getY(), i->getWidth(), i->getHeight());
			if(rect.intersects(plat) && i->getType() != PlatformType::Flat) {
				sf::FloatRect bottom(nx + 5, ny + height - 1, width - 10, 1);
				if(bottom.intersects(plat)) {
					ny = plat.top - height;
					ySp = min(ySp, 0.0F);
				}
				sf::FloatRect left(nx, ny + 5, 1, height - 10);
				if(left.intersects(plat)) {
					nx = plat.left + plat.width;
                    xSp = max(xSp, 0.0F);
				}
				sf::FloatRect right(nx + width - 1, ny + 5, 1, height - 10);
				if(right.intersects(plat)) {
					nx = plat.left - width;
					xSp = min(xSp, 0.0F);
				}
				sf::FloatRect top(nx + 5, ny, width - 10, 1);
				if(top.intersects(plat)) {
					ny = plat.top + plat.height;
					ySp = max(ySp, 0.0F);
				}
			}
		}
		x = nx;
		y = ny;
		if(y > SCREENHEIGHT) {
			if(EASYMODE) {
				y = 0;
			}
			else {
				hp = 0;
			}
		}
		return hp > 0;
	}

	sf::Sprite& render() {
		sprite.setPosition(x + (float)width / 2, y + (float)height / 2);
		return sprite;
	}
};

class Heart : public RenderObject {
	bool enabled;
public:
    Heart() {
    	enabled = 0;
    	if(EASYMODE) {
			sprite.setTexture(GOLDENHEART);
    	}
    	else {
			sprite.setTexture(HEART);
		}
		sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
		enable();
	}

	void enable() {
		if(enabled) return;
		enabled = 1;
		sprite.setColor(sf::Color::White);
	}

	void disable() {
		if(!enabled) return;
		enabled = 0;
		sprite.setColor(sf::Color::Transparent);
	}
};

struct PlatINFO {
	int x;
	int y;
	PlatformType type;
	int spriteType;
};

class Game {
	//Render
	sf::RenderWindow window;

	//Game objects
	Player player;
	Platforms platforms;
	bool isGameActive;
	float curX;
	queue<PlatINFO*> platmap;
	sf::Sprite bg;
	array<Heart*, 3> hearts;

	//FPS counting
	clock_t lastFrame;
	int frames;
	float time;

	float fps() {
		clock_t current = clock();
		float dt = (float)(current - lastFrame) / CLOCKS_PER_SEC;
		lastFrame = current;
		time += dt;
		frames++;
		if(time >= 1) {
			cout << (int)(frames / time) << endl;
			frames = 0;
			time = 0;
		}
		return dt;
	}

	void windowUpdate() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				isGameActive = 0;
			}
		}
		if(!isGameActive) {
			window.close();
		}
		isGameActive = isGameActive && window.isOpen();
	}

	void frame() {
		float dt = fps();
		update(dt);
		windowUpdate();
		render();
	}

	void update(float dt) {
		if(!EASYMODE) {
			GAMESPEED += dt;
		}
		curX += dt * GAMESPEED;
		isGameActive = isGameActive && player.update(dt, platforms);
		player.updateAnimation(dt);
		int hp = player.getHP();
        for(int i = 0; i < hp; i++) {
            hearts[i]->enable();
        }
        for(int i = hp; i < 3; i++) {
            hearts[i]->disable();
        }
		for(auto it = platforms.begin(); it != platforms.end();) {
			(*it)->updateAnimation(dt);
			if(!((*it)->update(dt))) {
				delete(*it);
				it = platforms.erase(it);
			}
			else {
				it++;
			}
		}
	}

	void render() {
		window.clear((sf::Color::Black));
		window.draw(bg);
		for(auto i : platforms) {
			window.draw(i->render());
		}
		window.draw(player.render());
		for(int i = 0; i < 3; i++) {
            window.draw(hearts[i]->render());
		}
		window.display();
	}
public:
	Game() : window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Runner") {
		isGameActive = 0;
		lastFrame = clock();
		frames = 0;
		time = 0;
		curX = SCREENWIDTH;

		PLAT_NORMAL.loadFromFile("normal.png");
		PLAT_FLAT.loadFromFile("flat.png");
		PLAT_DAMAGE.loadFromFile("damage.png");
		PLAT_JUMPING.loadFromFile("jump.png");
		PLAT_FALLING.loadFromFile("falling.png");
		PLAYER.loadFromFile("player.png");
		BACKGROUND.loadFromFile("background.png");
		HEART.loadFromFile("heart.png");
		GOLDENHEART.loadFromFile("goldenheart.png");
		bg.setTexture(BACKGROUND);
		bg.setTextureRect(sf::IntRect(0, 0, SCREENWIDTH, SCREENHEIGHT));
		bg.setColor(sf::Color(255, 255, 255, 150));
        for(int i = 0; i < 3; i++) {
			hearts[i] = new Heart();
			hearts[i]->setX(5 + 40 * i);
			hearts[i]->setY(5);
        }
		player.setX(40);
		player.setY(200);
		ifstream mapname("mapname.txt");
		string s;
		mapname >> s;
		ifstream inmap(s);
		while(!inmap.eof() && inmap.good()) {
			PlatINFO *infp = new PlatINFO();
			PlatINFO &inf = *infp;
			int t;
			inmap >> inf.x >> inf.y >> t >> inf.spriteType;
			inf.type = PlatformType(t);
			platmap.push(infp);
		}
		while(platmap.size()) {
			PlatINFO &inf = *platmap.front();
			Platform *pl = new Platform(inf.type, inf.spriteType);
			pl->setX(inf.x * PLATSIZE);
			pl->setY(inf.y * PLATSIZE);
			platforms.insert(pl);
			delete(platmap.front());
			platmap.pop();
		}
	}

	void play() {
		isGameActive = 1;
		lastFrame = clock();
		while(isGameActive) {
			frame();
		}
	}
};
