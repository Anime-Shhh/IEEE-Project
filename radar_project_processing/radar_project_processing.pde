import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;
PFont orcFont;
int iAngle;
int iDistance;
int[] lastDistances = new int[360];  // Array to store last distances

void setup() {
    size(1300, 800);
    smooth();
    
    myPort = new Serial(this, "/dev/cu.usbserial-10", 9600);
    myPort.clear();
    myPort.bufferUntil('\n');
    
    orcFont = loadFont("OCRAExtended-30.vlw");
    
    // Initialize lastDistances
    for(int i = 0; i < 360; i++) {
        lastDistances[i] = 0;
    }
}

void draw() {
    background(0);
    fill(98, 245, 31);
    textFont(orcFont);

    DrawRadar();
    DrawLine();
    DrawObject();
    DrawText();
}

void serialEvent(Serial myPort) {
    try {
        String data = myPort.readStringUntil('\n');
        if (data == null) return;
        
        int commaIndex = data.indexOf(",");
        String angle    = data.substring(0, commaIndex);
        String distance = data.substring(commaIndex+1).trim();
        
        iAngle    = StringToInt(angle);
        iDistance = StringToInt(distance);
        
        if (iAngle >= 0 && iAngle < 360) {
            lastDistances[iAngle] = iDistance;
        }
    }
    catch(RuntimeException e) {
        // ignore malformed lines
    }
}

void DrawRadar() {
    pushMatrix();
    translate(width/2, height/2);
    float radarSize = min(width, height) * 0.9;
    noFill();
    strokeWeight(2);
    stroke(98, 245, 31);
    
    arc(0, 0, radarSize, radarSize, 0, TWO_PI);
    arc(0, 0, radarSize*0.8, radarSize*0.8, 0, TWO_PI);
    arc(0, 0, radarSize*0.6, radarSize*0.6, 0, TWO_PI);
    arc(0, 0, radarSize*0.4, radarSize*0.4, 0, TWO_PI);
    
    for(int angle = 0; angle < 360; angle += 30) {
        float x = (radarSize/2) * cos(radians(angle));
        float y = (radarSize/2) * sin(radians(angle));
        line(0, 0, x, y);
    }
    popMatrix();
}

void DrawLine() {
    pushMatrix();
    translate(width/2, height/2);
    strokeWeight(9);
    stroke(30, 250, 60);
    float angle  = radians(iAngle);
    float radius = min(width, height) * 0.45;
    float x      = radius * cos(angle);
    float y      = radius * sin(angle);
    line(0, 0, x, y);
    popMatrix();
}

void DrawObject() {
    pushMatrix();
    translate(width/2, height/2);
    strokeWeight(9);
    stroke(255, 10, 10);
    float maxRangePixels = min(width, height) * 0.45;
    float scalePerCm     = maxRangePixels / 40.0;
    
    for (int angle = 0; angle < 360; angle++) {
        int distance = lastDistances[angle];
        if (distance > 0 && distance < 40) {
            float r = distance * scalePerCm;
            float angleRad = radians(angle);
            float x1 = r * cos(angleRad);
            float y1 = r * sin(angleRad);
            float x2 = maxRangePixels * cos(angleRad);
            float y2 = maxRangePixels * sin(angleRad);
            line(x1, y1, x2, y2);
        }
    }
    popMatrix();
}

void DrawText() {
    fill(98, 245, 31);
    textAlign(LEFT, TOP);
    textSize(25);
    text("Angle: " + iAngle + "°", 15, 15);
    text("Distance: " + (iDistance > 40 ? "Out of range" : iDistance + "cm"), 15, 45);
}

int StringToInt(String s) {
    int val = 0;
    for (int i = 0; i < s.length(); i++) {
        char c = s.charAt(i);
        if (c >= '0' && c <= '9') {
            val = (val * 10) + (c - '0');
        }
    }
    return val;
}