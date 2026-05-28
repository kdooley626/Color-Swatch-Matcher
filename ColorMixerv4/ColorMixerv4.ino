// Assign pins to switches and motor driver input and outputs

// Red color motor pins
#define Red_sp 9
#define Red_1 8 
#define Red_2 7

// Yellow color motor pins
#define Yellow_sp 6
#define Yellow_1 3
#define Yellow_2 4

// Blue color motor pins
#define Blue_sp 5
#define Blue_1 12
#define Blue_2 13

// Set up pump structure
struct Pump {
  const char* name;
  int sp;
  int pin1;
  int pin2;
  float rate;
};
// Set up structure to determine color recipe. First three values are the ratio parts for each color, fourth value is the total mass to dispense
struct Recipe {
  float red;
  float yellow;
  float blue;
  float mass;
};

// Define pump rates
float red_rate = 1.571;
float blue_rate = 1.532;
float yellow_rate = 1.462;

Pump redPump = {"red", Red_sp, Red_1, Red_2, red_rate};
Pump yellowPump = {"yellow", Yellow_sp, Yellow_1, Yellow_2, yellow_rate};
Pump bluePump = {"blue", Blue_sp, Blue_1, Blue_2, blue_rate};
Pump pumps[] = {redPump, yellowPump, bluePump};

const int NUM_PUMPS = sizeof(pumps) / sizeof(pumps[0]);


// Set up for serial input



void setup() {
 
  Serial.begin(9600);
 

  // Initialize pins for all pump colors
  for (int i = 0; i < NUM_PUMPS; i++) {
    pinMode(pumps[i].sp, OUTPUT);
    pinMode(pumps[i].pin1, OUTPUT);
    pinMode(pumps[i].pin2, OUTPUT);
    digitalWrite(pumps[i].pin1, LOW);
    digitalWrite(pumps[i].pin2, LOW);
    analogWrite(pumps[i].sp, 0);

  }
  
 
  //

}
void loop() {
  

  // Check if data is available
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n'); // read until Enter key
    cmd.trim(); // remove whitespace
    if (checkString(cmd)==true) {
      Recipe recipe = parseInput(cmd);
      float ratioTotal = (float)(recipe.red + recipe.yellow + recipe.blue);
      float redRatio = recipe.red / ratioTotal ;
      float yellowRatio = recipe.yellow / ratioTotal ;
      float blueRatio = recipe.blue / ratioTotal ;
      float redMass = redRatio * recipe.mass ;
      float yellowMass = yellowRatio * recipe.mass ;
      float blueMass = blueRatio * recipe.mass;
  
      makePrimary(redPump, redMass);
      delay(500);
      makePrimary(yellowPump, yellowMass);
      delay(500);
      makePrimary(bluePump, blueMass);
      
    } else if (cmd == "ping") {
      Serial.println("PONG");
      }
    else {
      Serial.println("Invalid input");
    }
    Serial.println("DONE");
  
  }
 
}

// Function to find index for a pump color
int findPumpIndex(String color) {
  for (int i = 0; i < NUM_PUMPS; i++) {
    
    if (color == pumps[i].name) {
      return i;
    }
  }
  return -1;
}
// Function to parse string
Recipe parseInput(String data) {
  int first = data.indexOf(',');
  int second = data.indexOf(',', first + 1);
  int third = data.indexOf(',', second + 1);

  Recipe input;
  input.red = data.substring(0, first).toFloat();
  input.yellow = data.substring(first+1, second).toFloat();
  input.blue = data.substring(second+1, third).toFloat();
  input.mass = data.substring(third+1).toFloat();
  return input;
}
// Function to verify string is a valid input
bool checkString(String data) {
  bool validity = true;
  int first = data.indexOf(',');
  int second = data.indexOf(',', first + 1);
  int third = data.indexOf(',', second + 1);
  if (first == -1 || second == -1 || third == -1) {
    validity = false;
  }
  return validity;
  
}

// Function to generate a certain mass of primary color  (ie a color that does not require mixing)
void makePrimary(const Pump& pump, float mass) {
  if (mass == 0) {
    return;
  }
  float t = (float) (mass/pump.rate);
  unsigned long delayTime= (unsigned long)(round(t * 1000));

  analogWrite(pump.sp, 255);
  digitalWrite(pump.pin1, HIGH);
  digitalWrite(pump.pin2, LOW);
  delay(delayTime);
  digitalWrite(pump.pin1, LOW);
  analogWrite(pump.sp, 0);
  Serial.print("Dispensed "); Serial.print(mass, 1); Serial.print(" grams of "); Serial.println(pump.name);
  Serial.print("Dispense time: "); Serial.print(delayTime); Serial.println("ms");
}












