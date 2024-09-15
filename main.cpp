///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Editor: Bella Conrad
// Date: 4/22/2024
// Name: hw7\main.cpp
// Description: Comment Asteroids game to better understand code and ufo class while improving other aspects of
//              the game.
//
// EXTRA CREDIT: added intro, play, and game over states with different display screens and instructions,
// added six different bullet colors, red, orange, yellow, green, and kept original blue, then purple
// Also added explosion sounds.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <cmath>
#include <SFML/Audio.hpp>

using namespace sf;

const int W = 1200; // width of window
const int H = 800; // height of window

float DEGTORAD = 0.017453f; // used to change angle variable from degrees to radians

class Animation // handles animation of sprites
{
public:
	float Frame, speed;
	Sprite sprite;
    std::vector<IntRect> frames;

	Animation(){} // constructor

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed) //
	{   // constructor initializes an animation with a texture (t), starting position (x, y), frame width (w),
        // frame height (h), number of frames (count), and animation speed (Speed).
	    Frame = 0; // initializes frame of animation to zero
        speed = Speed; // set speed of animation

		for (int i=0;i<count;i++) // creates and adds an IntRect object to the frames vector, loop generates
            // different rectangles incrementing x coordinate for each frame
         frames.push_back( IntRect(x+i*w, y, w, h)  );

		sprite.setTexture(t); // set the texture of the sprite associated with this animation
		sprite.setOrigin(w/2,h/2); // sets the origin of the sprite to its center
        sprite.setTextureRect(frames[0]); // sets the texture rectangle of the sprite to the first frame
        // of the animation. This initializes the sprite with the first frame of the animation
	}


	void update()  // advances the animation by incrementing the Frame variable
	{
    	Frame += speed; // moves animation forward in time by the specified speed
		int n = frames.size(); // get number of frames for animation
		if (Frame >= n) Frame -= n; // ensures that animation wraps around when it reaches the end
		if (n>0) sprite.setTextureRect( frames[int(Frame)] ); // sets texture rectangle of the sprite to
        // the rectangle of the current frame. Accesses frame from the current value of Frame variable
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size();  // returns true if the animation has ended
	}
};


class Entity // base class for each type of sprite in the game
{
public:
float x,y,dx,dy,R,angle; // x and y correspond to x and y coordinates of sprite,
// dx and dy are rate of change and direction to increment x and y coordinates with,
// R is the radius of collision for sprites, angle is the angle of rotation of sprites (how fast they rotate)
bool life; // if false, take entity of out of entity pointer list
std::string name; // saves the name of the type of entity as a string
Animation anim; // animation corresponding to sprite

Entity() 
{
  life = 1; // constructor that sets its life variable to true
}

void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
{
  anim = a; // initialize the sprite animation type associated with the type of entity:spaceship, asteroid, bullet, etc.
  x=X; y=Y; // initialize the sprites x and y coordinates
  angle = Angle; // initialize the sprite's angle of rotation
  R = radius; // initialize the radius of the entity
}

virtual void update(){}; // virtual function that's expected to be overridden
// used to update a sprite's motion using derived classes

void draw(RenderWindow &app) // draws sprites onto the game screen and is able to render collision circles for debugging
{
  anim.sprite.setPosition(x,y); // sets the position of the sprite associated with the entity
  // to the current coordinates of the entity
  anim.sprite.setRotation(angle+90); // sets rotation of the sprite +90 to ensure faster rotation
  app.draw(anim.sprite); // draw the sprite associated with the entity

  CircleShape circle(R); // create circle with radius R
  circle.setFillColor(Color(255,0,0,170)); // Fill the circle with red to visualize collision
  circle.setPosition(x,y); // set the collision circle with the current coordinates of the entity/sprite
  circle.setOrigin(R,R); // sets the origin of the collision circle to its center
  //app.draw(circle); // don't draw the collision circle unless debugging the program
}

virtual ~Entity(){}; // virtual entity destructor that calls the destructor of its derived classes (prevents memory leaks)
};
///////////////////////////////////////////////// added UFO CLASS ////////////////////////////////////////
class ufo: public Entity
{
public:
    ufo() {
        name = "ufo";
        x = 20; y = 20;
        dx = 1.25; dy = 0;
    }
    void update() {
        x+=dx;
        if (x>W) x=0;  if (x<0) x=W; // horizontal wrap-around behavior
    }
};


class asteroid: public Entity // asteroid class derived from entity base class
{
public:
  asteroid() // constructor that initializes direction of the asteroid and its name as the type of entity (asteroid)
  {
    dx=rand()%8-4; // make asteroids move in random x direction
    dy=rand()%8-4; // make asteroids move  in random y direction
    name = "asteroid";
    ++count; // count number of asteroids
  }

  ~asteroid() {
      --count; // decrement number asteroids when they're destroyed
  }

void  update() // update the position of the asteroid as an entity
  {
   x+=dx; // move the asteroid in the x direction on the screen
   y+=dy; // move the asteroid in the y direction on the screen

    // define wrap-around behavior
   if (x>W) x=0;  if (x<0) x=W; // if asteroid moves off-screen horizontally, move to other side of screen
   if (y>H) y=0;  if (y<0) y=H; // vertical wrap-around
  }

  static unsigned int getCount() { // get number of asteroids in game
      return count;
  }

private:
    static unsigned int count; // counts number of asteroids in game

};

unsigned int asteroid::count = 0; // initialize count only once

class bullet: public Entity // bullet derived class of entity, shoots out of the spaceship on screen
{
public:
  bullet() // constructor: sets the name of the entity
  {
    name = "bullet";
  }

void  update() // updates the motion of bullet entities
  {
      // update each bullets speed and direction using the angle and speed variables
   dx=cos(angle*DEGTORAD)*6;
   dy=sin(angle*DEGTORAD)*6;
  // angle+=rand()%6-3;
   x+=dx; // move bullet in x direction according to its change in angle and speed from dx
   y+=dy; // move bullet in y direction according to its change in angle and speed from dy

   if (x>W || x<0 || y>H || y<0) life=0; // if bullet goes off the screen, it disappears and entity deletes in main
  }

};


class player: public Entity
{
public:
   bool thrust; // variable that is true if the user presses up arrow

   player() // constructor that saves name of entity type
   {
     name = "player";
   }

   void update() // updates motion of the player
   {
     if (thrust) // if up arrow is pressed
      { dx+=cos(angle*DEGTORAD)*0.2; // accelerate by 0.2 coordinates per pixel speed
        dy+=sin(angle*DEGTORAD)*0.2; } // same as above but in the y direction
     else
      { dx*=0.99; // deceleration: slow down by 1% each frame if the thrust isn't pressed anymore
        dy*=0.99; } // y direction

    int maxSpeed=15; // set max speed to 15 coordinates per pixel
    float speed = sqrt(dx*dx+dy*dy); // get magnitude of player velocity
    if (speed>maxSpeed) // if magnitude of speed is greater than the max speed
     { dx *= maxSpeed/speed; // keep speed at +15 pixels per frame in x coordinates
       dy *= maxSpeed/speed; } // keep speed at +15 pixels per frame in y coordinates

    x+=dx; // move player in x direction
    y+=dy; // ^^ y direction
    // define screen wrapping
    if (x>W) x=0; if (x<0) x=W; // if player goes off-screen, move to other side of screen horizontally
    if (y>H) y=0; if (y<0) y=H; // ^^ vertically
   }

};


bool isCollide(Entity *a,Entity *b) // set static variable
{
  return (b->x - a->x)*(b->x - a->x)+ // calculates the difference in the x coordinates between two entity objects
         (b->y - a->y)*(b->y - a->y)< // calculates difference in y coordinates between two entity objects
         (a->R + b->R)*(a->R + b->R); // returns true if the entities are colliding
}

// check if a ufo exists to ensure only one ufo is in game at a time
bool ufoExists(std::list<Entity*> &entities) {
    for (auto e: entities) {
        if (e->name == "ufo") {
            return true; // return true UFO exists in list of entities
        }
    }
    return false; // false if UFO is not in list of entities
}


int main()
{
    srand(time(0)); // seed time to zero
    enum screentype {INTRO, PLAY, GAMEOVER}; // create gamestate variables
    screentype gameState;
    gameState = INTRO;

    enum bulletColors {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE}; // set different bullet color variables
    bulletColors color;
    color = RED;

    RenderWindow app(VideoMode(W, H), "Asteroids"); // create game window
    app.setFramerateLimit(60); // set max framerate to 60

    Texture t1,t2,t3,t4,t5,t6,t7; // create and initialize textures for sprites and background of game
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png"); // BLUE BULLET
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");

    // load extra bullet textures
    Texture redBullet, orangeBullet, yellowBullet, greenBullet, purpleBullet;
    // blue bullet was already loaded in original program
    redBullet.loadFromFile("images/fire_red.png");
    orangeBullet.loadFromFile("images/fire_orange.png");
    yellowBullet.loadFromFile("images/fire_yellow.png");
    greenBullet.loadFromFile("images/fire_green.png");
    purpleBullet.loadFromFile("images/fire_purple.png");


    // create texture for UFO and load from file. UFO png used from flaticon.com
    // license: free for personal and commercial use
    Texture UFOtexture; UFOtexture.loadFromFile("images/UFO128pix.png");

    t1.setSmooth(true); // make the spaceship look less pixelated
    t2.setSmooth(true); // make the galaxy background look less pixelated

    Sprite background(t2); // creates a sprite named background
    // to be displayed as the background picture of the game for every frame

    // set animation inputs for each type of entity
    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);

    // set animations for each bullet color
    Animation sBulletRed(redBullet, 0,0,32,64, 16, 0.8);
    Animation sBulletOrange(orangeBullet, 0,0,32,64, 16, 0.8);
    Animation sBulletYellow(yellowBullet, 0,0,32,64, 16, 0.8);
    Animation sBulletGreen(greenBullet, 0,0,32,64, 16, 0.8);
    Animation sBulletBlue(t5, 0,0,32,64, 16, 0.8);
    Animation sBulletPurple(purpleBullet, 0,0,32,64, 16, 0.8);

    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);
    Animation sUFO(UFOtexture, 0, 0, 128, 128, 1, 0); // set ufo texture into anim class

    //Create ufo alarm sound, sound from freesound.org, license: free for personal and commercial use
    SoundBuffer UFObuffer; // create sound buffer
    if (!UFObuffer.loadFromFile("sounds/UFOalarm.ogg")) {return EXIT_FAILURE;}
    Sound UFOsound; UFOsound.setBuffer(UFObuffer); UFOsound.setVolume(10);

    // create explosion sound, sound from freesound.org, license: free for personal and commercial use
    SoundBuffer explosionBuffer;
    if (!explosionBuffer.loadFromFile("sounds/explosionSound.ogg")) {return EXIT_FAILURE;}
    Sound explosionSound; explosionSound.setBuffer(explosionBuffer); explosionSound.setVolume(20);

    // load texture from 1001freefonts.com, license: free for personal use
    Font regularFont;
    if (!regularFont.loadFromFile("fonts/Orbitron-Regular.ttf")) {return EXIT_FAILURE;}
    Font mediumFont;
    if (!mediumFont.loadFromFile("fonts/Orbitron-Medium.ttf")) {return EXIT_FAILURE;}
    Font boldFont;
    if (!boldFont.loadFromFile("fonts/Orbitron-Bold.ttf")) {return EXIT_FAILURE;}


    std::list<Entity*> entities; // create list of entity pointers called entities to keep track of game entities

    for(int i=0;i<15;i++) // create 15 asteroid entities in the game
    {
      asteroid *a = new asteroid(); // allocate memory for current asteroid object
      a->settings(sRock, rand()%W, rand()%H, rand()%360, 25); //
      // set current asteroids animation, start coordinates (random location), angle of rotation, and radius
      entities.push_back(a); // add each asteroid to list of game entities
    }

    player *p = new player(); // allocate memory for the player object
    p->settings(sPlayer,200,200,0,20); // start the player (spaceship) in the middle of the screen
    // pointing upwards with a collision radius of 20 pixels
    entities.push_back(p); // add the player to the list of game entities


    /////main loop/////
    while (app.isOpen()) // while game is played check if buttons are pressed
    {
        Event event; // create event to be be polled with
        while (app.pollEvent(event)) // poll events
        {
            if (event.type == Event::Closed) // close game
                app.close();

            // Handle key events for different game states
            if (gameState == INTRO) {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                    gameState = PLAY; // Switch to gameplay state when enter is pressed
                }
            }
            else if (gameState == PLAY) {
                // check if the user changed the bullet color
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Num1) { // Change bullet color to RED
                        color = RED;
                    } else if (event.key.code == Keyboard::Num2) { // Change bullet color to ORANGE
                        color = ORANGE;
                    } else if (event.key.code == Keyboard::Num3) { // Change bullet color to YELLOW
                        color = YELLOW;
                    } else if (event.key.code == Keyboard::Num4) { // Change bullet color to GREEN
                        color = GREEN;
                    } else if (event.key.code == Keyboard::Num5) { // Change bullet color to BLUE
                        color = BLUE;
                    } else if (event.key.code == Keyboard::Num6) { // Change bullet color to PURPLE
                        color = PURPLE;
                    }
                }

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
                    // Handle shooting logic
                    bullet *b = new bullet(); // allocate memory for a bullet object
                    // set animation to current bullet color
                    if (color == RED){
                        b->settings(sBulletRed,p->x,p->y,p->angle,10); // initialize bullet variables to make the bullet
                    }
                    else if (color == ORANGE) {
                        b->settings(sBulletOrange,p->x,p->y,p->angle,10); // initialize bullet variables to make the bullet
                    }
                    else if (color == YELLOW) {
                        b->settings(sBulletYellow,p->x,p->y,p->angle,10); // initialize bullet variables to make the bullet
                    }
                    else if (color == GREEN) {
                        b->settings(sBulletGreen,p->x,p->y,p->angle,10); // initialize bullet variables to make the bullet
                    }
                    else if (color == BLUE) {
                        b->settings(sBulletBlue,p->x,p->y,p->angle,10); // initialize bullet variables to make the bullet
                    }
                    else if (color == PURPLE) {
                        b->settings(sBulletPurple,p->x,p->y,p->angle,10); // initialize bullet variables to make the bullet
                    }
                    entities.push_back(b); // add the bullet to list of game entities
                }
            } else if (gameState == GAMEOVER) {
                // Handle game over screen events
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
                    gameState = PLAY; // Switch to gameplay state when enter is pressed
                }
            }

        }


        if (gameState == INTRO) {
            // Display intro screen
            // Create Text objects for title and instructions
            Text titleText("Asteroids", boldFont, 60); // Title text
            titleText.setFillColor(Color::White); // Set text color
            titleText.setPosition(W / 2 - titleText.getGlobalBounds().width / 2, H / 4); // Position title text

            Text instructionsText("Controls:\n\n- Use arrow keys to move\n- Press Spacebar to shoot\n"
                                  "- Press numbers 1-6 to toggle between bullet colors",
                                  mediumFont, 30); // Instructions text
            instructionsText.setFillColor(Color::White); // Set text color
            instructionsText.setPosition(W / 2 - instructionsText.getGlobalBounds().width / 2, H / 2);

            Text enterText("Press enter to start", boldFont, 30);
            enterText.setFillColor(Color::White); // Set text color
            enterText.setPosition(W / 2 - enterText.getGlobalBounds().width / 2, H - 200);

            // Draw title and instructions texts
            app.clear(); // Clear the window
            app.draw(background); // Draw background
            app.draw(titleText); // Draw title text
            app.draw(instructionsText); // Draw instructions text
            app.draw(enterText); // press enter to start

        }


        if (gameState == PLAY) { // gameplay logic:

            if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle+=3; // change players angle of rotation
            // to the right side of the screen
            if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle-=3; // change rotation to the left side of the screen
            if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust=true; // move player up and play thrust sound if
                // up arrow is pressed, change thrust variable true to animate fire shooting out of the spaceship
            else p->thrust=false; // don't display fire shooting out of the spaceship


            // check if objects are colliding
            for(auto a:entities) // iterate through all entities twice simultaneously
                for(auto b:entities)
                {
                    if (a->name == "asteroid" && b->name == "bullet")  // check if asteroid and bullet entities are colliding
                        if ( isCollide(a,b) ){
                            explosionSound.play();
                            a->life=false; // if an asteroid and bullet collide, set their life to false and delete them off-screen
                            b->life=false;

                            Entity *e = new Entity(); // allocate memory for new entity
                            e->settings(sExplosion,a->x,a->y); // set x and y coordinates of explosion animation to be
                            // where the asteroid and bullet collided
                            e->name="explosion"; // name the explosion entity
                            entities.push_back(e); // add explosion to entity pointer list


                            for(int i=0;i<2;i++) // iterate through loop twice
                            {
                                if (a->R == 15) continue; // checks if the asteroid is large by checking if the radius is 15 pixels,
                                // if true, then continues to next iteration without executing code below
                                Entity *e = new asteroid();
                                // if the entity a is a large asteroid iterate through loop twice to create two smaller asteroids
                                e->settings(sRock_small,a->x,a->y,rand()%360,15);
                                entities.push_back(e); // add two small asteroids to list of game entities
                            }

                        }

                    ////////////////////// add collision check for the ufo and player ///////////////////////////////////////////
                    if (a->name == "player" && b->name == "ufo")  // check if ufo and bullet entities are colliding
                        if ( isCollide(a,b) ){
                            explosionSound.play();
                            a->life=false; // if an asteroid and bullet collide, set their life to false and delete them off-screen
                            b->life=false;

                            UFOsound.stop();

                            Entity *e = new Entity(); // allocate memory for new entity
                            e->settings(sExplosion,a->x,a->y); // set x and y coordinates of explosion animation to be
                            // where the asteroid and bullet collided
                            e->name="explosion"; // name the explosion entity
                            entities.push_back(e); // add explosion to entity pointer list

                            p->settings(sPlayer,W/2,H/2,0,20); // return player to center of screen
                            p->dx = 0; p->dy = 0;
                            gameState = GAMEOVER;
                        }

                    ////////////////////// add collision check for the ufo and bullet ///////////////////////////////////////////
                    if (a->name == "ufo" && b->name == "bullet")  // check if ufo and bullet entities are colliding
                        if ( isCollide(a,b) ){
                            explosionSound.play();
                            a->life=false; // if an asteroid and bullet collide, set their life to false and delete them off-screen
                            b->life=false;

                            UFOsound.stop();

                            Entity *e = new Entity(); // allocate memory for new entity
                            e->settings(sExplosion,a->x,a->y); // set x and y coordinates of explosion animation to be
                            // where the asteroid and bullet collided
                            e->name="explosion"; // name the explosion entity
                            entities.push_back(e); // add explosion to entity pointer list

                        }


                    if (a->name == "player" && b->name == "asteroid")
                        if ( isCollide(a,b) ) // check if the player has collided with an asteroid
                        {
                            explosionSound.play();
                            b->life = false;  // set the asteroids life to false, so it deletes off the screen from main program code

                            Entity *e = new Entity();
                            e->settings(sExplosion_ship,a->x,a->y);
                            e->name="explosion";
                            entities.push_back(e);

                            p->settings(sPlayer,W/2,H/2,0,20);
                            p->dx = 0; p->dy = 0;
                            gameState = GAMEOVER;
                        }
                }


            if (p->thrust){
                p->anim = sPlayer_go; // animate fire shooting out of spaceship if thrust button is pressed
            }
            else {
                p->anim = sPlayer; // animate spaceship without fire shooting out if thrust is not pressed
            }

            ////// add ufo to screen after time has passed //////////////////////////
            if (!ufoExists(entities) && rand()%1000 == 0) {
                ufo *u = new ufo(); // allocate memory for new ufo
                u->settings(sUFO,20,50,270,50); // set ufo position and display settings
                entities.push_back(u); // add ufo to list of entities

                // Play warning sound in a loop when the ufo is created
                UFOsound.setLoop(true);
                UFOsound.play();
            }

            // create 15 new asteroids when all have been destroyed
            if (asteroid::getCount() == 0) {
                for(int i=0;i<15;i++) // create 15 asteroid entities in the game
                {
                    asteroid *a = new asteroid(); // allocate memory for current asteroid object
                    a->settings(sRock, rand()%W, rand()%H, rand()%360, 25); //
                    // set current asteroids animation, start coordinates (random location), angle of rotation, and radius
                    entities.push_back(a); // add each asteroid to list of game entities
                }
            }

            for(auto e:entities)
                if (e->name == "explosion")
                    if (e->anim.isEnd()) e->life = 0; // if the explosion is done animating, end its life (deletes it in main)

//            if (rand()%150 == 0) {  // create a new asteroid after random amounts of time pass in the game
//               asteroid *a = new asteroid();
//               a->settings(sRock, 0,rand()%H, rand()%360, 25); // initialize asteroid settings
//               entities.push_back(a); // add new asteroid to list of entities
//             }


            for (auto i=entities.begin(); i!=entities.end();){ // iterate through all entities in game
                Entity *e = *i; // initialize entity pointer

                e->update(); // update location of the current entity using its pointer
                e->anim.update(); // display animation of current entity

                if (e->life == false) {
                    i = entities.erase(i); delete e; // if entity dies, remove it from the screen
                    // and free memory e points to
                }

                else {
                    i++;
                }
            }


            //////draw//////
            app.draw(background); // draw galaxy background

            for(auto i:entities) // draw all entities using polymorphism
                i->draw(app);
        }


        if (gameState == GAMEOVER) {
            // Display game over screen
            Text gameOverText("Game Over", boldFont, 60); // Game over message
            gameOverText.setFillColor(Color::White); // Set text color
            gameOverText.setPosition(W / 2 - gameOverText.getGlobalBounds().width / 2, H / 4); // Position game over message

            Text retryText("Press the space bar to retry", mediumFont, 30); // Retry message
            retryText.setFillColor(Color::White); // Set text color
            retryText.setPosition(W / 2 - retryText.getGlobalBounds().width / 2, H / 2); // Position retry message

            // Draw game over and retry messages
            app.clear(); // Clear the window
            app.draw(background); // Draw background
            app.draw(gameOverText); // Draw game over message
            app.draw(retryText); // Draw retry message
        }


       app.display(); // display game

    }

    return 0;
}
