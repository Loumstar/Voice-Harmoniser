#define MAX_VOICES 10

typedef double note[3]; // first element is note number, second is frequency, third is volume

double note_frequency(int n){
    return pow(2, (n - 69) / 12) * 440.0;
}

void note_off(int n, double** notes){
    for(size_t i = 0; i < MAX_VOICES; i++){
        if((int) notes[i][0] = n){
            notes[i] = {0; 0; 0};
            break;
        }
    }
}

void note_on(int n, int vol, double** notes){
    for(size_t i = 0; i < MAX_VOICES; i++){
        if(notes[i][0] == 0.0){
            notes[i] = {
                (double) n, 
                note_frequency(n), 
                (double) vol / 128.0
            };
            break;
        }
    }
}

void handle_midi(int* msg, double** notes){
    if(msg[0] < 0xA0){ // only handle note on/off messages
        if(msg[0] < 0x90){
            note_off(msg[1], notes);
        } else {
            note_on(msg[1], msg[2], notes);
        }
    }
}

int* read_midi(int* msg, Stream &midiDevice){
    size_t msg_length = midiDevice.available();
    size_t i = 0;

    while(i < msg_length){
        msg[i] = midiDevice.read(); //need to check for status byte
        i++;
    }
}