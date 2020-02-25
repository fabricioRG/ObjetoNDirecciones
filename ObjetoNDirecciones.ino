#include <AFMotor.h>
#define TRIGGER 52
#define ECHO 53

#define PIN_SENSOR_LEFT 32
#define PIN_SENSOR_RIGHT 33
#define PIN_SENSOR_UP 50
#define PIN_SENSOR_DOWN 51

#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_RIGHT 3
#define MOVE_LEFT 4
#define MOVE_UP_DIAGONAL_RIGHT 5
#define MOVE_DOWN_DIAGONAL_LEFT 6
#define MOVE_UP_DIAGONAL_LEFT 7
#define MOVE_DOWN_DIAGONAL_RIGHT 8
#define TURN_RIGHT 9
#define TURN_LEFT 10

#define SPEED 200
#define MAX_SPEED 10
#define MIN_SPEED 1

const float sonido = 34300.0; // Velocidad del sonido en cm/s
const float umbral1 = 10.0;

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

int estado = 0;
int movimiento = 0;
int actualSpeed = MAX_SPEED;

void setup() {     
  
  /*Inicializacion de entradas y salidas*/
  Serial1.begin(9600);  
  Serial.begin(9600);
  
  /*Inicializacion de detector de obstaculos IR*/
  pinMode(PIN_SENSOR_RIGHT, INPUT);  //definir pin como entrada
  pinMode(PIN_SENSOR_LEFT, INPUT);  //definir pin como entrada
  pinMode(PIN_SENSOR_UP, INPUT);  //definir pin como entrada
  pinMode(PIN_SENSOR_DOWN, INPUT);  //definir pin como entrada
  
  /*Inicializacion de sensor ultrasonico*/
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
  
  /*Inicializacion de Motor Power Shield*/
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void loop() {
  readBluetooth();
  readIR();
  delay(100);
}

/*
Movimiento en diferentes direcciones:

-MOVIMIENTO HACIA ADELANTE:
  motor1 FORWARD
  motor2 FORWARD
  motor3 FORWARD
  motor4 FORWARD
-MOVIMIENTO HACIA ATRAS:
  motor1 BACKWARD
  motor2 BACKWARD
  motor3 BACKWARD
  motor4 BACKWARD
-MOVIMIENTO HACIA LA DERECHA:
  motor1 FORWARD
  motor2 BACKWARD
  motor3 BACKWARD
  motor4 FORWARD
-MOVIMIENTO HACIA LA IZQUIERDA:
  motor1 BACKWARD
  motor2 FORWARD
  motor3 FORWARD
  motor4 BACKWARD
-MOVIMIENTO EN DIAGONAL DERECHA SUPERIOR:
  motor1 FORWARD
  motor2 RELEASE
  motor3 RELEASE
  motor4 FORWARD
-MOVIMIENTO EN DIAGONAL IZQUIERDA INFERIOR:
  motor1 BACKWARD
  motor2 RELEASE
  motor3 RELEASE
  motor4 BACKWARD
-MOVIMIENTO EN DIAGONAL IZQUIERDA SUPERIOR:
  motor1 RELEASE
  motor2 FORWARD
  motor3 FORWARD
  motor4 RELEASE
-MOVIMIENTO EN DIAGONAL DERECHA INFERIOR:
  motor1 RELEASE
  motor2 BACKWARD
  motor3 BACKWARD
  motor4 RELEASE
-GIRO HACIA LA IZQUIERDA:
  motor1 BACKWARD
  motor2 FORWARD
  motor3 BACKWARD
  motor4 FORWARD
-GIRO HACIA LA DERECHA:
  motor1 FORWARD
  motor2 BACKWARD
  motor3 FORWARD
  motor4 BACKWARD
*/

void moveMotorNum(int realSpeed, int motorNum, int motorDirection){
  switch(motorNum){
    case 1:
      motor1.setSpeed(realSpeed);
      motor1.run(motorDirection);
      break;
    case 2:
      motor2.setSpeed(realSpeed);
      motor2.run(motorDirection);
      break;
    case 3:
      motor3.setSpeed(realSpeed);
      motor3.run(motorDirection);
      break;
    case 4:
      motor4.setSpeed(realSpeed);
      motor4.run(motorDirection);
      break;
  }
}

void moveMotor(int relativeSpeed, int motorNum, int motorDirection){
  if(relativeSpeed > MAX_SPEED){
    relativeSpeed = MAX_SPEED;
  } else if (relativeSpeed < MIN_SPEED){
    relativeSpeed = MIN_SPEED;
  }
  int resultSpeed = getRealSpeed(relativeSpeed);
  moveMotorNum(resultSpeed, motorNum, motorDirection);
}

/*Funcion que devuelve la velocidad real basada en una velocidad relativa
 *Resultado calculado a partir de una regla de 3, por ejemplo (5 * 200) / 10 = 100
 */
int getRealSpeed(int relativeSpeed){
  return (relativeSpeed * SPEED) / MAX_SPEED;
}

//Funcion encargada de mover el objeto hacia adelante
void moveUp(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, FORWARD);
  moveMotor(speed4, 4, FORWARD);
}

//Funcion encargada de mover el objeto hacia atras
void moveDown(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, BACKWARD);
  moveMotor(speed4, 4, BACKWARD);
}

//Funcion encargada de mover el objeto hacia la derecha
void moveRight(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, BACKWARD);
  moveMotor(speed4, 4, FORWARD);
}

//Funcion encargada de mover el objeto hacia la izquierda
void moveLeft(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, FORWARD);
  moveMotor(speed4, 4, BACKWARD);
}

void moveUpDiagonalRight(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  notMoveMotor(2);
  notMoveMotor(3);
  moveMotor(speed4, 4, FORWARD);
}

void moveDownDiagonalLeft(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  notMoveMotor(2);
  notMoveMotor(3);
  moveMotor(speed4, 4, BACKWARD);
}

void moveUpDiagonalLeft(int speed1, int speed2, int speed3, int speed4){
  notMoveMotor(1);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, FORWARD);
  notMoveMotor(4);
}

void moveDownDiagonalRight(int speed1, int speed2, int speed3, int speed4){
  notMoveMotor(1);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, BACKWARD);
  notMoveMotor(4);
}

void turnLeft(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, BACKWARD);
  moveMotor(speed2, 2, FORWARD);
  moveMotor(speed3, 3, BACKWARD);
  moveMotor(speed4, 4, FORWARD);
}

void turnRight(int speed1, int speed2, int speed3, int speed4){
  moveMotor(speed1, 1, FORWARD);
  moveMotor(speed2, 2, BACKWARD);
  moveMotor(speed3, 3, FORWARD);
  moveMotor(speed4, 4, BACKWARD);
}

void notMoveMotor(int motorNum){
  switch(motorNum){
  case 1:
    motor1.run(RELEASE);
    break;
  case 2:
    motor2.run(RELEASE);
    break;
  case 3:
    motor3.run(RELEASE);
    break;
  case 4:
    motor4.run(RELEASE);
    break;
  }
}

//Funcion encargada de detener el objeto
void notMove(){
  notMoveMotor(1);
  notMoveMotor(2);
  notMoveMotor(3);
  notMoveMotor(4);
}

/*Funcion encargada de leer la entrada de bluetooth.
 *Dependiendo de la señal de entrada de bluetooth se moveran los motores
 */
void readBluetooth(){
  if(Serial1.available()>0){        // lee el bluetooth y almacena en estado
    estado = Serial1.read();
    switch(estado){
    case 'F':
    moveUp(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_UP;
    break;
    case 'L':
    moveLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_LEFT;
    break;
    case 'S':
    notMove();
    break;
    case 'R':
    moveRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_RIGHT;
    break;
    case 'G':
    moveDown(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_DOWN;
    break;
    case 'E':
    moveUpDiagonalRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_UP_DIAGONAL_RIGHT;
    break;
    case 'Z':
    moveDownDiagonalLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_DOWN_DIAGONAL_LEFT;
    break;
    case 'Q':
    moveUpDiagonalLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_UP_DIAGONAL_LEFT;
    break;
    case 'C':
    moveDownDiagonalRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = MOVE_DOWN_DIAGONAL_RIGHT;
    break;
    case 'X':
    turnLeft(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = TURN_LEFT;
    break;
    case 'Y':
    turnRight(actualSpeed, actualSpeed, actualSpeed, actualSpeed);
    movimiento = TURN_RIGHT;
    break;
    case '1':
    actualSpeed = 1;
    break;
    case '2':
    actualSpeed = 2;
    break;
    case '3':
    actualSpeed = 3;
    break;
    case '4':
    actualSpeed = 4;
    break;
    case '5':
    actualSpeed = 5;
    break;
    case '6':
    actualSpeed = 6;
    break;
    case '7':
    actualSpeed = 7;
    break;
    case '8':
    actualSpeed = 8;
    break;
    case '9':
    actualSpeed = 9;
    break;
    case '0':
    actualSpeed = 10;
    break;
    }
  }
  Serial.println(estado);
}

/*Funcion encargada de detectar proximidad por los costados del objeto*/
int readIR(){
  int upSensor = 1;
  int downSensor = 1;
  int rightSensor = 1;
  int leftSensor = 1;
  upSensor = digitalRead(PIN_SENSOR_UP);  //lectura digital de pin
  downSensor = digitalRead(PIN_SENSOR_DOWN);
  rightSensor = digitalRead(PIN_SENSOR_RIGHT);
  leftSensor = digitalRead(PIN_SENSOR_LEFT);
  
  switch(movimiento){
  case MOVE_UP:
  if(upSensor == LOW){
    notMove();
  }
  break;
  case MOVE_LEFT:
  if(leftSensor == LOW){
    notMove();
  }
  break;
  case MOVE_RIGHT:
  if(rightSensor == LOW){
    notMove();
  }
  break;
  case MOVE_DOWN:
  if(downSensor == LOW){
    notMove();
  }
  break;
  case MOVE_UP_DIAGONAL_RIGHT:
  if(upSensor == LOW || rightSensor == LOW){
    notMove();
  }
  break;
  case MOVE_DOWN_DIAGONAL_LEFT:
  if(downSensor == LOW || leftSensor == LOW){
    notMove();
  }
  break;
  case MOVE_UP_DIAGONAL_LEFT:
  if(upSensor == LOW || leftSensor == LOW){
    notMove();
  }
  break;
  case MOVE_DOWN_DIAGONAL_RIGHT:
  if(downSensor == LOW || rightSensor == LOW){
    notMove();
  }
  break;
  }
}

int proximity(){
  // Preparamos el sensor de ultrasonidos
  iniciarTrigger();

  // Obtenemos la distancia
  float distancia = calcularDistancia();
  return alerta(distancia);
}

// Función que comprueba si hay que lanzar alguna alerta visual o sonora
int alerta(float distancia)
{
  // Lanzamos alerta si estamos dentro del rango de peligro
  if (distancia <= umbral1)
  {
    // Lanzamos alertas
    Serial.println("Peligroooooooo!");
    return HIGH;
  }
  return LOW;
}

// Método que calcula la distancia a la que se encuentra un objeto.
// Devuelve una variable tipo float que contiene la distancia
float calcularDistancia()
{
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(ECHO, HIGH);

  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  float distancia = tiempo * 0.000001 * sonido / 2.0;
  delay(500);

  return distancia;
}

// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);

  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(TRIGGER, LOW);
}

