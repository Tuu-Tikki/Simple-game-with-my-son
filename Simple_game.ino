#include <LiquidCrystal.h>
#include <IRremote.h>

//LCD matrix
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int xpotPin = A0;
int ypotPin = A1;            
int bpotPin = 6; 
int xval=0;       
int yval=0;
int bval=0;

//Remote controller
const int irReceiverPin = 7;
IRrecv irrecv(irReceiverPin);
decode_results results;

//LCD matrix coordinates vertical [0-1], horizontal [0-15]
struct COORDINATES {
  int vertical;
  int horizontal;
} ;

//settings
const char obstacle_symbol = 'N';
int game_field[2][16] = {0};
const int obstacles_amount = 7;
const COORDINATES obstacles_coordinates[obstacles_amount] = {{0, 2},{1, 5}, {0, 7}, {1, 9}, {0, 11}, {1, 13},{0, 15}};
const long remote_controller_directions[4] = {0xFF18E7, 0xFF10EF, 0xFF4AB5, 0xFF5AA5}; //Up, Left, Down, Right
const int joystick_sensitivity[2] = {403, 603}; //consider only <403 and >603
const char joystick_player_symbol = '*';
COORDINATES joystick_player_coordinates = {0, 0};
const char remote_controller_player = '#';
COORDINATES remote_controller_player_coordinates = {1, 0};
bool lost = false;

void setup() {
  initializeRemoteController();
  initializeLCD();
  
  setObstacles();
  
  showStartMessage();
}

void loop() {
  if (lost) {
    startAgain();
    return;
  }

  int joystick_direction = getJoystickDirection();
  if (joystick_direction) {
    joystick_player_coordinates = movePlayer(joystick_player_coordinates, joystick_direction);
  }
  int remote_controller_direction = getRemoteControllerDirection();
  if (remote_controller_direction) {
  remote_controller_player_coordinates = movePlayer(remote_controller_player_coordinates, remote_controller_direction);
  }
  
  if (isHitAnObstacle(joystick_player_coordinates) || isHitAnObstacle(remote_controller_player_coordinates)) {
    showFail();
    startAgain();
  }

  if (isWin(joystick_player_coordinates) || isWin(remote_controller_player_coordinates)) {
    showVictory();
    startAgain();
  }
  
  lcd.clear();
  showObstacles();
  showPlayer(joystick_player_coordinates, joystick_player_symbol);
  showPlayer(remote_controller_player_coordinates, remote_controller_player);

  delay(500);
}

void initializeRemoteController() {
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void initializeLCD() {
  lcd.begin(16,2);   
  pinMode(xpotPin,INPUT);
  pinMode(ypotPin,INPUT);
  pinMode(bpotPin,INPUT); 
}

void showStartMessage() {
  lcd.print("  Game ");
  lcd.setCursor(0,1);
  lcd.print("  is starting");
  delay(2000);
  lcd.clear();  
}

void setObstacles() {
  for (int i = 0; i < obstacles_amount; i++) {
    game_field[obstacles_coordinates[i].vertical][obstacles_coordinates[i].horizontal] = 1;
  }
}

void showObstacles() {
  for (int i = 0; i < obstacles_amount; i++)
  {
    lcd.setCursor(obstacles_coordinates[i].horizontal, obstacles_coordinates[i].vertical) ;       
    lcd.print(obstacle_symbol); 
  }
}
int getJoystickDirection() {
  int direction = 0; //0 - on place, 1 - up, 2 - left, 3 - down, 4 - right
  
  xval = analogRead(xpotPin);
  yval = analogRead(ypotPin);
    
  direction = yval < joystick_sensitivity[0] ? 4 : direction;
  direction = yval > joystick_sensitivity[1] ? 2 : direction;
  direction = xval < joystick_sensitivity[0] ? 1 : direction;
  direction = xval > joystick_sensitivity[1] ? 3 : direction;
  
  return direction;
}

int getRemoteControllerDirection() {
  int direction = 0; //0 - on place, 1 - up, 2 - left, 3 - down, 4 - right
  
  const long up = remote_controller_directions[0];
  const long left = remote_controller_directions[1];
  const long down = remote_controller_directions[2];
  const long right = remote_controller_directions[3];
  
  if (irrecv.decode(&results))
  {
    if (irrecv.decode(&results))
    {
      switch (results.value) {
        case up:
           direction = 1;
           break;
        case left:
           direction = 2;
           break;
        case down:
           direction = 3;
           break;  
        case right:
           direction = 4;
           break;   
        default:
           direction = 0;
           break;    
      }

      irrecv.resume();
    }
  }  
   
  return direction;
}

void showPlayer(COORDINATES player_coordinates, const char player_symbol) {
  lcd.setCursor(player_coordinates.horizontal, player_coordinates.vertical);
  lcd.print(player_symbol);  
}

COORDINATES movePlayer(COORDINATES player_coordinates, int direction) {
  switch (direction) {
    case 1:
      player_coordinates.vertical = player_coordinates.vertical == 1 ? 0 : player_coordinates.vertical;
      break;
    case 2:
      player_coordinates.horizontal = player_coordinates.horizontal > 0 ? player_coordinates.horizontal-1 : player_coordinates.horizontal;
      break;
    case 3:
      player_coordinates.vertical = player_coordinates.vertical == 0 ? 1 : player_coordinates.vertical;
      break;
    case 4:
      player_coordinates.horizontal = player_coordinates.horizontal < 15 ? player_coordinates.horizontal+1 : player_coordinates.horizontal;
      break;  
    default:
      break;        
  }
  
  return player_coordinates;
}

bool isHitAnObstacle(COORDINATES player_coordinates) {
    return game_field[player_coordinates.vertical][player_coordinates.horizontal];
}

bool isWin(COORDINATES player_coordinates){
  return player_coordinates.horizontal == 15 ? true : false;
  delay(500);
}

void startAgain() {  
    joystick_player_coordinates = {0, 0};
    remote_controller_player_coordinates = {1, 0};
    lost = false;
}

void showVictory() {
    lcd.clear(); 
    lcd.setCursor(3, 0) ;       
    lcd.print("Gewonnen");  
    delay(2000);
}

void showFail() {
    lcd.clear(); 
    lcd.setCursor(3, 0) ;       
    lcd.print("Verloren");
    delay(2000);  
}
