/**********************
 code to look for obstacles
**********************/

// servo defines
const int servoDelay = 500; // time in ms for servo to move

const int MIN_DISTANCE = 10;     // robot stops when object is nearer (in inches)
const int CLEAR_DISTANCE = 24;  // distance in inches considered attracive to move
const int MAX_DISTANCE = 150;   // the maximum range of the distance sensor

// servo angles             left, right, center 
const int servoAngles[] = {  170, 10,    90};


void lookBegin() {
  irSensorBegin();    // initialize sensors 
  softServoAttach();  // attaches the servo pin to the servo object 
}


// returns true if the given obstacle is detected
boolean lookForObstacle(int obstacle) {
  switch(obstacle) {
     case  OBST_FRONT_EDGE: return irEdgeDetect(DIR_LEFT) && irEdgeDetect(DIR_RIGHT); 
     case  OBST_LEFT_EDGE:  return irEdgeDetect(DIR_LEFT); 
     case  OBST_RIGHT_EDGE: return irEdgeDetect(DIR_RIGHT); 
     case  OBST_FRONT:      return lookAt(servoAngles[DIR_CENTER]) <= MIN_DISTANCE;      
  }
  return false; 
}


// Look for and avoid obstacles using servo to scan 
void roam() {
  int distance = lookAt(servoAngles[DIR_CENTER]);
//  Serial.print("Distance is ");
//  Serial.println(distance);
  
  if (distance == 0) { 
    moveStop();
//    Serial.println("No front sensor. Stopping...");
    return;  // no sensor
  } else if (distance <= MIN_DISTANCE) {
    moveStop();
//    Serial.println("Stopping...");
//    Serial.println("Scanning:");
    int leftDistance = lookAt(servoAngles[DIR_LEFT]);
    
    if (leftDistance > CLEAR_DISTANCE)  {
//      Serial.print("Moving left...");
      moveRotate(-90);
    } else {
      delay(500);
      int rightDistance = lookAt(servoAngles[DIR_RIGHT]);
      
      if (rightDistance > CLEAR_DISTANCE) {
//        Serial.println("Moving right...");
        moveRotate(90);
      } else {
//        Serial.println("No clearence...");
        distance = max(leftDistance, rightDistance);
        
        if (distance < CLEAR_DISTANCE/2) {
//          Serial.println("Moving back and turning around...");
          timedMove(MOV_BACK, 1000); // back up for one second  
          moveRotate(-180); // turn around
        } else {
          if (leftDistance > rightDistance) {
//            Serial.println("Moving left...");
            moveRotate(-90);
          } else {
//            Serial.println("Moving left...");
            moveRotate(90);
          }
        }
      } 
    }
  }
}


// returns the distance of objects at the given angle
int lookAt(int angle) {
  softServoWrite(angle, servoDelay); // wait for servo to get into position

  int distance, samples;
  long cume;
  distance = samples = cume = 0;
  for (int i = 0; i < 4; i++) {  
    distance = getDistance();
    if (distance > 0) {
      //  printlnValue(" D= ",distance);
      samples++;
      cume += distance;
    }  
  }
  if (samples > 0)
    distance = cume / samples;
  else
    distance = 0;

  if (angle != servoAngles[DIR_CENTER]) {
    Serial.print("looking at dir "); 
    Serial.print(angle), Serial.print(" distance= "); 
    Serial.println(distance); 
    softServoWrite(servoAngles[DIR_CENTER], servoDelay/2);    
  }
  
  return distance;   
}

// function to check if robot can continue moving in current direction 
// returns true if robot is not blocked moving in current direction
// this version only tests for obstacles in front
boolean checkMovement() {
  boolean isClear = true; // default return value if no obstacles
  if (moveGetState() == MOV_FORWARD) {
    if (lookForObstacle(OBST_FRONT) == true) {     
       isClear = false;   
    }
  }
  return isClear; 
}


// the following is based on loop code from myRobotEdge
// robot checks for edge and moves to avoid
//void avoidEdge() {
//   if( lookForObstacle(OBST_FRONT_EDGE) == true)
//  {
//     Serial.println("left and right sensors detected edge");
//     timedMove(MOV_BACK, 300);
//     moveRotate(120);
//     while(lookForObstacle(OBST_FRONT_EDGE) == true )
//        moveStop(); // stop motors if still over cliff
//  }
//  else if(lookForObstacle(OBST_LEFT_EDGE) == true)
//  {
//     Serial.println("left sensor detected edge");
//     timedMove(MOV_BACK, 100);
//     moveRotate(30);
//  }
//  else if(lookForObstacle(OBST_RIGHT_EDGE) == true)  
//  {
//     Serial.println("right sensor detected edge");
//     timedMove(MOV_BACK, 100);
//     moveRotate(-30); 
//  }
//}
