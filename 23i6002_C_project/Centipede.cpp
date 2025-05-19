
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <SFML/Graphics/Text.hpp>
using namespace sf;
using namespace std;

//------------------------------------------------------------SCREEN ACCESSORIES----------------------------------------------------//
const int resolutionX = 640;
const int resolutionY = 640;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX;
const int gameColumns = resolutionY / boxPixelsY;
int gameGrid[gameRows][gameColumns] = {};
const int x = 0;
const int y = 1;
//------------------------------------------------------------MUSHROOM VARIABLES---------------------------------------------------//
const int maxMushrooms = 20;
float mushrooms[maxMushrooms][3];
bool mushroomExists[maxMushrooms];

//------------------------------------------------------------CENTIPEDE VARIABLES--------------------------------------------------//
const int centipedeSegments = 12;
float centipedeX[centipedeSegments];
float centipedeY[centipedeSegments];
const float centipedeSpeed = 0.3f;
bool centipedemoveright[centipedeSegments];
bool centipedeExists[centipedeSegments];
bool segmentHit[centipedeSegments] = {false};
bool centipedeIsHead[centipedeSegments] = {false};


const int exists = 2;
const int health = 2;
//-------------------------------------------------------------------SCORING-------------------------------------------------------//
const std::string scoreFileName = "score.txt";
void saveScoreToFile(int score);
int loadScoreFromFile();
int playerScore=0;
//------------------------------------------------------------------FUNCTION DECLARATION-------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------//
void initializeCentipede();
void updateCentipede(float bullet[], float player[]);
void drawCentipede(RenderWindow& window, Texture& headTexture, Texture& bodyTexture);
void drawPlayer(RenderWindow& window, float player[], Sprite& playerSprite);
void moveBullet(float bullet[], Clock& bulletClock);
void drawBullet(RenderWindow& window, float bullet[], Sprite& bulletSprite);
void drawMushrooms(RenderWindow& window, Texture& mushroomTexture, int boxPixelsX, int boxPixelsY);
void drawBackground(RenderWindow& window, Texture& backgroundTexture);
void handlePlayerInput(float player[]);
void movePlayer(float player[], Clock& playerClock);
void centipededirectionsetter();
void initializePlayer(float player[]);
void destroyCentipede();
void centipedeSplit(int segmentIndex, float bullet[], RenderWindow& window, Texture& headTexture, Texture& bodyTexture);
void destroymushrooms(float bullet[], int playerScore, RenderWindow& window);



int main() 
{

    srand(time(0));

//----------------------------------------------------------Screen resolution-----------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
   RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede");
    window.setSize(Vector2u(640, 640));
    window.setPosition(Vector2i(100, 0));
//----------------------------------------------------------TEXTURES--------------------------------------------------------//
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("Screenshots/b.png");
    drawBackground(window, backgroundTexture);
//---------------------------------------------------------------------------------------------------------------------------//
    Texture bodyTexture; 
    bodyTexture.loadFromFile("Screenshots/a.jpeg");

    Texture headTexture;
    headTexture.loadFromFile("Screenshots/head.jpeg");

    Texture mushroomTexture;
    mushroomTexture.loadFromFile("Screenshots/p.jpeg");
//----------------------------------------------------------------------------------------------------------------------------------//
    Sprite playerSprite;
    Texture playerTexture;
    playerTexture.loadFromFile("Textures/player.png");
    playerSprite.setTexture(playerTexture);
    playerSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
//------------------------------------------------------------------------------------------------------------------------------------//
    Clock bulletClock;
    Texture bulletTexture;
    Sprite bulletSprite;
    bulletTexture.loadFromFile("Textures/bullet.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

//-----------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------PLAYER VARIABLES------------------------------------------------------------
    float player[2] = {};
    player[x] = (gameColumns / 2) * boxPixelsX;
    player[y] = (gameColumns * 3 / 4) * boxPixelsY;
    Clock playerClock;
//----------------------------------------------------------------------------------------------------------------------------------------------//

   sf::Font font;
   font.loadFromFile("h.ttf");

//-------------------------------------------------------------------BULLET VARIABLES---------------------------------------------------//    
    float bullet[3] = {};
    bullet[x] = player[x];
    bullet[y] = player[y] - boxPixelsY;
    bullet[health] = 0;
    bullet[exists] = false;
    
    initializeCentipede();
    centipededirectionsetter();
//--------------------------------------------------------------------MUSHROOM INITIALIZING-------------------------------------------//
    // Initializing mushrooms
    for (int i = 0; i < maxMushrooms; ++i) {
        mushrooms[i][x] = rand() % resolutionX;
        do {
            mushrooms[i][y] = rand() % resolutionY;
        } while (!(mushrooms[i][y] > 64 && mushrooms[i][y] < 800));

        mushrooms[i][health] = 2;
        mushroomExists[i] = true;
    }


//playerScore =loadScoreFromFile();
sf::Text scoreText;
scoreText.setFont(font);
scoreText.setCharacterSize(20);
scoreText.setFillColor(sf::Color::White);
scoreText.setPosition(10, 10);

        sf::Music bgMusic;
	bgMusic.openFromFile("Music/field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);




  
while (window.isOpen()) {
    window.clear();
//---------------------------------------------------------------FUNCTION CALLING-------------------------------------------------------------//
    drawBackground(window, backgroundTexture);
    drawMushrooms(window, mushroomTexture, boxPixelsX, boxPixelsY);
    drawCentipede(window, headTexture, bodyTexture);
    handlePlayerInput(player);
    movePlayer(player, playerClock);
    drawPlayer(window, player, playerSprite);
    updateCentipede(bullet, player);
    destroymushrooms(bullet, playerScore, window);

      


 //------------------------------------------------------KEYBOARD SPACEBAR FOR BULLET-------------------------------------------------------//
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !bullet[exists]) {
        bullet[x] = player[x] + boxPixelsX / 2;
        bullet[y] = player[y] - boxPixelsY;
        bullet[health] = 1;
        bullet[exists] = true;
    }

    if (bullet[exists] == true) {
        moveBullet(bullet, bulletClock);
        drawBullet(window, bullet, bulletSprite);
    }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !bullet[exists]) {
            bullet[x] = player[x] + boxPixelsX / 2;
            bullet[y] = player[y] - boxPixelsY;
            bullet[health] = 1;
            bullet[exists] = true;
        }

            
// Check for bullet hits
if (bullet[exists]) {
    for (int i = 0; i < centipedeSegments; ++i) {
        if (centipedeExists[i] && !segmentHit[i] &&
            bullet[x] + boxPixelsX >= centipedeX[i] && bullet[x] <= centipedeX[i] + boxPixelsX &&
            bullet[y] + boxPixelsY >= centipedeY[i] && bullet[y] <= centipedeY[i] + boxPixelsY) {
            
            centipedeSplit(i, bullet, window, headTexture,bodyTexture);
            
            //playerScore+=10;
            cout << "Bullet hit on body segment. Player Score: " << playerScore << endl;
            // Resetting the bullet and marking the segment as hit
            bullet[health] = 0;
            bullet[exists] = false;
            segmentHit[i] = true;

            // If the centipede segment is moving to the left, callling split for the previous segment
           if (i > 0 && centipedemoveright[i - 1]) {
                centipedeSplit(i - 1, bullet, window, headTexture,bodyTexture);
            }
        }
    }
}
      
//---------------------------------------------------------------------SCORING---------------------------------------------------------------//
// tried to implement it but player score goes infinity again and again that is why i have commented out though on terminal it shows correct
string scoreString = "Player Score: ";
//cout << "Debug: " << scoreString << endl;
//cout << "Player Score: " << playerScore << endl;
scoreText.setString(scoreString);
window.draw(scoreText);         
  //-------------------------------------------------------------------------------------------------------------------------------------//          
       Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
        }

        window.display();
    }

    return 0;
}
//------------------------------------------------------------------END OF MAIN---------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//---------------------------------------------------------------FUNCTIONS----------------------------------------------------//
//---------------------------------------------------------------1--------------------------------------------------//
// handling centipede direction 
void centipededirectionsetter() {
    for (int n = 0; n < centipedeSegments; n++)
        centipedemoveright[n] = true;
       
}
//----------------------------------------------------------------2----------------------------------------------------------//
//Initializing centipede from top corner (0,0)
void initializeCentipede() {
    for (int i = 0; i < centipedeSegments; ++i) {
        centipedeX[i] = i * boxPixelsX;
        centipedeY[i] = 0;
        centipedeExists[i] = true;

        if (i == centipedeSegments - 1) {
            centipedeIsHead[i] = true;
        }
    }
}
//---------------------------------------------------------------3-------------------------------------------------------//
//drawing centipede on screen
void drawCentipede(RenderWindow& window, Texture& headTexture, Texture& bodyTexture) 
{
    for (int i = 0; i < centipedeSegments; ++i) {
        if (centipedeExists[i] && !segmentHit[i]) {
            Sprite centipedeSprite;

            if (centipedeIsHead[i]) {
                centipedeSprite.setTexture(headTexture);
            } else 
              {
                centipedeSprite.setTexture(bodyTexture);
            }

            centipedeSprite.setPosition(centipedeX[i], centipedeY[i]);
            centipedeSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
            window.draw(centipedeSprite);
        }
    }
}
//--------------------------------------------------------------4-----------------------------------------------------------//
//one of the important function of the code 
void updateCentipede(float bullet[], float player[]) {
    for (int i = 0; i < centipedeSegments; ++i) {
        if (centipedeExists[i] && !segmentHit[i]) {
            // Check for player hits
            if (player[x] + boxPixelsX >= centipedeX[i] && player[x] <= centipedeX[i] + boxPixelsX &&
                player[y] + boxPixelsY >= centipedeY[i] && player[y] <= centipedeY[i] + boxPixelsY) {
                // Player hitting by centipede, eliminating the player(fighter)
                initializePlayer(player);
            }

            // Move centipede
            if (centipedemoveright[i]) {
                centipedeX[i] += centipedeSpeed;
            } else {
                centipedeX[i] -= centipedeSpeed;

}
//-----------------------------------------for edges of screen and move back and for logic------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------------------------//
// Check for reaching the edges of the screen
if (centipedeX[i] >= resolutionX - boxPixelsX || centipedeX[i] <= 0) 
{
    // Check for the centipede at bottom-left corner
    if (centipedeY[i] >= resolutionY - boxPixelsY)
    {
             // move up one row
       centipedeY[i] -= boxPixelsY;
        // Change direction to move right
        centipedemoveright[i] = true;
        //debugging for better analysis
     // cout << "Centipede at bottom-left corner. Ascending to row " << centipedeY[i] / boxPixelsY << ". Moving right." << endl;
    } 
  
  else if (centipedeY[i] < resolutionY - boxPixelsY)
    {
        // Moving down if not at the bottom-left corner and within the player area
        centipedeY[i] += boxPixelsY;

        // Changing direction if the centipede is within the player area
        centipedemoveright[i] = !centipedemoveright[i];

        // Debug information
       //cout << "Centipede within player area. Moving down to row " << centipedeY[i] / boxPixelsY << ". Changing direction." << endl;
    }
}
// if bullet is hitting centipede's head then calling destroy function
if (bullet[exists] && centipedeIsHead[i] &&
                bullet[x] + boxPixelsX >= centipedeX[i] && bullet[x] <= centipedeX[i] + boxPixelsX &&
                bullet[y] + boxPixelsY >= centipedeY[i] && bullet[y] <= centipedeY[i] + boxPixelsY) {
                // Bullet hit the head of the centipede, destroy the entire centipede
                destroyCentipede();
            }
        }
    }
 }
//---------------------------------------------------------5-----------------------------------------//
//----------------------------------------------for eliminating fighter------------------------------//
void initializePlayer(float player[]) {
    // Setting player position outside the visible area to eliminate the player
    player[x] = -1000;
    player[y] = -1000;
}
//----------------------------------------------------------6---------------------------------------//
// for drawing player
void drawPlayer(RenderWindow& window, float player[], Sprite& playerSprite) {
    playerSprite.setPosition(player[x], player[y]);
    window.draw(playerSprite);
}
//--------------------------------------------------------7-------------------------------------//
//for drawing bullet
void drawBullet(RenderWindow& window, float bullet[], Sprite& bulletSprite) {
    bulletSprite.setPosition(bullet[x], bullet[y]);
    window.draw(bulletSprite);
}

//--------------------------------------------------------8------------------------------------------//
//for moving bullet
void moveBullet(float bullet[], Clock& bulletClock) {
    const float bulletSpeed = 0.6f;
    const Time bulletMoveInterval = milliseconds(10);

    if (bulletClock.getElapsedTime() >= bulletMoveInterval) {
        bulletClock.restart();
        bullet[y] -= boxPixelsY / 2 * bulletSpeed;

        if (bullet[y] < -boxPixelsY) {
            bullet[health] = 0;
            bullet[exists] = false;
        }
    }
}
//--------------------------------------------------------9--------------------------------------------//
//for drawing background
void drawBackground(RenderWindow& window, Texture& backgroundTexture) {
    Sprite backgroundSprite(backgroundTexture);
    window.draw(backgroundSprite);
}
//--------------------------------------------------------10-------------------------------------------------//
//for scoring (file handling)
int loadScoreFromFile() {
    std::ifstream scoreFile("score.txt");
    int score = 0;

    if (scoreFile.is_open()) {
        scoreFile >> score;
        scoreFile.close();
        std::cout << "Score loaded from file." << std::endl;
    } else {
        cout << "Unable to open the score file. Using default score of 0." << std::endl;
    }

    return score;
}

//-----------------------------------------------------11----------------------------------------------//
//handles player movement in all directions basically reacting to what user is pressing
void handlePlayerInput(float player[]) {
    const float playerSpeed = 0.2f;

    if (Keyboard::isKeyPressed(Keyboard::Left) && player[x] > 0) {
        player[x] -= playerSpeed;
    }

    if (Keyboard::isKeyPressed(Keyboard::Right) && player[x] < resolutionX - boxPixelsX) {
        player[x] += playerSpeed;
    }

    if (Keyboard::isKeyPressed(Keyboard::Up) && player[y] > 0) {
        player[y] -= playerSpeed;
    }

    if (Keyboard::isKeyPressed(Keyboard::Down) && player[y] < resolutionY - boxPixelsY) {
        player[y] += playerSpeed;
    }
}

//-----------------------------------------------------12-----------------------------------------//
// this function is basically for restricting player upto five rows
void movePlayer(float player[], Clock& playerClock) {
    const float playerSpeed = 0.4f;
    const int playerAreaTop = resolutionY - (5 * boxPixelsY);
    const int playerAreaBottom = resolutionY;

    if (playerClock.getElapsedTime().asMilliseconds() < 20)
        return;

    if (Keyboard::isKeyPressed(Keyboard::Up) && player[y] > playerAreaTop) {
        player[y] -= playerSpeed;
    }

    if (Keyboard::isKeyPressed(Keyboard::Down) && player[y] < playerAreaBottom - boxPixelsY) {
        player[y] += playerSpeed;
    }

    if (player[y] < playerAreaTop) {
        player[y] = playerAreaTop;
    }

    playerClock.restart();
}
//------------------------------------------------------13---------------------------------------------//
// for drawing mushrooms randomly within screen on random locations
void drawMushrooms(RenderWindow& window, Texture& mushroomTexture, int boxPixelsX, int boxPixelsY) {
    for (int i = 0; i < maxMushrooms; ++i) {
        if (mushroomExists[i]) {
            int mushroomGridX = mushrooms[i][x] / boxPixelsX;
            int mushroomGridY = mushrooms[i][y] / boxPixelsY;

            float posX = mushroomGridX * boxPixelsX;
            float posY = mushroomGridY * boxPixelsY;

            Sprite mushroomSprite(mushroomTexture);
if (mushrooms[i][health] == 2) {
   
        mushroomSprite.setTextureRect(IntRect(1, 1, boxPixelsX, boxPixelsY));
 
} else {
   mushroomSprite.setColor(Color::Red);
    // Set the default texture rectangle or fill color for other cases

}      mushroomSprite.setPosition(posX, posY);
            window.draw(mushroomSprite);
        }
    }
}
//--------------------------------------------------------------14--------------------------------------------------//
// for bullet hitting head of centipede 
void destroyCentipede() {
    for (int i = 0; i < centipedeSegments; ++i) {
        segmentHit[i] = true;
    }
   //playerScore+=20;
}

//--------------------------------------------------------------15------------------------------------------------//
//one of the important function of code 
void centipedeSplit(int segmentIndex, float bullet[], RenderWindow& window, Texture& headTexture, Texture& bodyTexture) {
    // Checking bullet exists 
    if (bullet[exists]) {
        // Calculating split distance
        float splitDistance = 1.9* boxPixelsY; //how far splitted segments go

        // Checkingif the bullet is within the hit segment's boundaries
        if (bullet[x] + boxPixelsX / 2 >= centipedeX[segmentIndex] &&
            bullet[x] - boxPixelsX / 2 <= centipedeX[segmentIndex] + boxPixelsX &&
            bullet[y] + boxPixelsY / 2 >= centipedeY[segmentIndex] &&
            bullet[y] - boxPixelsY / 2 <= centipedeY[segmentIndex] + boxPixelsY) {

            //-------------------------------------logic for finding next segment index ( splitted index)------------------------------//
            int nextSegmentIndex = segmentIndex + 2;
           // while (nextSegmentIndex < centipedeSegments && !centipedeExists[nextSegmentIndex]) {
             //   ++nextSegmentIndex;
            //}

            // Checking if the next segment exists
            if (nextSegmentIndex < centipedeSegments) {
                // Changing the direction of segments before the hit point ( to avoid overlapping)
                for (int n = 0; n < segmentIndex; ++n) 
                 {
                    centipedemoveright[n] = !centipedemoveright[n]; 
                }
          
                // Changing the direction of segments after the hit point to move downward ( to avoid overlapping)
                for (int n = nextSegmentIndex; n < centipedeSegments; ++n) {
                    centipedemoveright[n] = false; // Move downward (to avoid overlapping)
                    
                     centipededirectionsetter();
                    // Adjusting the position of segments 
                    centipedeY[n] += splitDistance;
                }
      //---------------------------------------------logic for head of new spitted segments--------------------------------------//
                // Storing the index of the segment where the split occurred
                int splitSegmentIndex = nextSegmentIndex;
                
                // Calculating the index of the new head segment relative to the original head
                int newHeadIndex = (splitSegmentIndex - 1) - (segmentIndex + 1);
                
                // Setting head texture for the new head segment
                centipedeIsHead[newHeadIndex] = true;
           
                // Resetting the head flag for the old head segment
                centipedeIsHead[segmentIndex] = false;

                // Calculating the position of the new head segment
                float newHeadX = centipedeX[splitSegmentIndex]; // X position of the split segment
                float newHeadY = centipedeY[splitSegmentIndex] - splitDistance; // Y position above the split segment

                // Drawing the head sprite
                Sprite headSprite(headTexture);
                headSprite.setPosition(newHeadX, newHeadY);
                headSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
                window.draw(headSprite);

                // Marking the segment as hit to prevent further splitting
                segmentHit[segmentIndex] = true;
               
            } 
            else {
                // Resetting the hit flag for the original hit segment
                segmentHit[segmentIndex] = false;
                 
                // Set head texture for the current segment when there is no next segment
                centipedeIsHead[segmentIndex] = true;
               
                
            }
        }
    }
}
//-----------------------------------------------------------------16------------------------------------------------//
//for scoring 
void saveScoreToFile(int score) {
    std::ofstream scoreFile("score.txt");

    if (scoreFile.is_open()) {
        scoreFile << score;
        scoreFile.close();
       cout << "Score saved to file." << std::endl;
    } else {
        cout << "Unable to open the score file." << std::endl;
    }
}

//------------------------------------------------------------------17----------------------------------------------//
// for bullet hitting mushrooms
void destroymushrooms(float bullet[], int playerScore, RenderWindow& window) {
    // Check for player shots hitting mushrooms
    for (int i = 0; i < maxMushrooms; ++i) {
        if (!mushroomExists[i] || !bullet[exists]) {
            continue;
        }

        if (bullet[x] + boxPixelsX >= mushrooms[i][x] && bullet[x] <= mushrooms[i][x] + boxPixelsX &&
            bullet[y] + boxPixelsY >= mushrooms[i][y] && bullet[y] <= mushrooms[i][y] + boxPixelsY) {

            // Player shot hits a mushroom
            bullet[health] = 0;
            bullet[exists] = false;

            // Decreasing mushroom health 
            mushrooms[i][health]--;

            // Checking if the mushroom is completely destroyed
            if (mushrooms[i][health] <= 0) {
                mushroomExists[i] = false;
                //playerScore += 1;  om
                cout << "Destroyed a mushroom! Score +1" << endl;
            }
        }
    }

    // Save player score to file after updating
  // saveScoreToFile(playerScore);

    // Check for centipede hitting mushrooms
    for (int i = 0; i < maxMushrooms; ++i) {
        if (!mushroomExists[i] || !centipedeExists[centipedeSegments - 1]) {
            continue;
        }

        if (centipedeX[centipedeSegments - 1] + boxPixelsX >= mushrooms[i][x] &&
            centipedeX[centipedeSegments - 1] <= mushrooms[i][x] + boxPixelsX &&
            centipedeY[centipedeSegments - 1] + boxPixelsY >= mushrooms[i][y] &&
            centipedeY[centipedeSegments - 1] <= mushrooms[i][y] + boxPixelsY) {

            // Centipede hits a mushroom
            // Decreasing mushroom health
            mushrooms[i][health]--;

            // Checking if the mushroom is completely destroyed
            if (mushrooms[i][health] <= 0) {
                mushroomExists[i] = false;
                playerScore += 1; // Incrementing the player's score
            }

             //Descending the centipede after hitting a mushroom
            /*for (int j = 0; j < centipedeSegments-1; ++j) {
                if (centipedeExists[j]) {
                    centipedeY[j] += boxPixelsY; // Descending by one row
                }
            }
        }*/
    }
    }
    }
    

