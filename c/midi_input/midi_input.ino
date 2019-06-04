#include <SoftwareSerial.h>

SoftwareSerial midiSerial(10, 11);

void setup(){
    Serial.begin(9600);
    while(!Serial);

    midiSerial.begin(31250);
}

void loop(){
    midiSerial.listen();
    
    while(midiSerial.available() > 0){
        Serial.write(
            midiSerial.read()
        );
    }

}
