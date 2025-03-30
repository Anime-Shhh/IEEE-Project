import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;
PFont orcFont;
int iAngle;
int iDistance;

void setup() {
    size(1300, 800);
    smooth();
    
    // Change COM port and baud to match your setup
    myPort = new Serial(this, "/dev/cu.usbserial-10", 9600);
    myPort.clear();
    myPort.bufferUntil('\n');
    
    orcFont = loadFont("OCRAExtended-30.vlw");
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
    }
    catch(RuntimeException e) {
        // ignore malformed lines
    }
}

// ------------------------------------
// Draws the main radar arcs & radial lines
// now fully centered at screen center
void DrawRadar() {
    pushMatrix();
    
    // 1) Move center to middle of the window
    translate(width/2, height/2);

    // 2) Radar size = 90% of smaller window dimension
    float radarSize = min(width, height) * 0.9;
    noFill();
    strokeWeight(2);
    stroke(98, 245, 31);
    
    // Draw concentric arcs (0 → TWO_PI) for full 360
    arc(0, 0, radarSize, radarSize, 0, TWO_PI);
    arc(0, 0, radarSize*0.8, radarSize*0.8, 0, TWO_PI);
    arc(0, 0, radarSize*0.6, radarSize*0.6, 0, TWO_PI);
    arc(0, 0, radarSize*0.4, radarSize*0.4, 0, TWO_PI);
    
    // Draw radial lines every 30 degrees
    for(int angle=0; angle<360; angle+=30) {
      float x = (radarSize/2)*cos(radians(angle));
      float y = (radarSize/2)*sin(radians(angle));
      line(0, 0, x, y);
    }
    
    popMatrix();
}

// ------------------------------------
// Draw the sweeping line that shows current angle
void DrawLine() {
    pushMatrix();
    translate(width/2, height/2);
    
    strokeWeight(9);
    stroke(30, 250, 60);
    float angle  = radians(iAngle);
    float radius = min(width, height)*0.45; 
    float x      = radius*cos(angle);
    float y      = radius*sin(angle);
    line(0, 0, x, y);
    
    popMatrix();
}

// ------------------------------------
// Place the “object” on the radar, if in range
void DrawObject() {
    pushMatrix();
    translate(width/2, height/2);
    
    strokeWeight(9);
    stroke(255,10,10);

    // Adjust distance scale so 40cm roughly matches 45% of the screen’s smaller dimension
    float maxRangePixels = min(width, height)*0.45; 
    float scalePerCm     = maxRangePixels / 40.0;
    
    if (iDistance > 0 && iDistance < 40) {
        float r = iDistance * scalePerCm;
        float cosVal = cos(radians(iAngle));
        float sinVal = sin(radians(iAngle));
        
        float x1 = r * cosVal;
        float y1 = r * sinVal;
        float x2 = maxRangePixels * cosVal;
        float y2 = maxRangePixels * sinVal;

        line(x1, y1, x2, y2);
    }
    popMatrix();
}

// ------------------------------------
// HUD text at bottom or top (your choice)
void DrawText() {
    fill(98, 245, 31);
    textAlign(LEFT, TOP);
    
    // Example text at top-left corner
    textSize(25);
    text("Angle: " + iAngle + "°", 15, 15);
    text("Distance: " + (iDistance>40 ? "Out of range" : iDistance+"cm"), 15, 45);
}

// ------------------------------------
// Converts numeric string to int
int StringToInt(String s) {
    int val = 0;
    for (int i=0; i<s.length(); i++) {
      char c = s.charAt(i);
      if (c >= '0' && c <= '9') {
        val = (val*10) + (c-'0');
      }
    }
    return val;
}
