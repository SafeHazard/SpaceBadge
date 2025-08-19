// game_parent.h
#pragma once
#include <Arduino.h>

class game_parent {
public:
    game_parent(int* exp_change) {};
    virtual void PlayerInput(uint16_t x, uint16_t iny) = 0;
    virtual void Setup() = 0;
    virtual void Update() = 0;
    virtual void render() = 0;

    unsigned long start_time;
    unsigned long timer_length = 120000; // equals 2 min
    void start_timer() { start_time = millis(); };
    int time_left() { 
        unsigned long elapsed = millis() - start_time;
        if (elapsed >= timer_length)
            return 0;

        return 100 - ((elapsed * 100) / timer_length); 
    }; // returns the time left as a percentage

    // Score broadcasting for multiplayer
    void broadcastScore(int scorePercent);
    
    virtual ~game_parent() = default;
};

// Declare globally:
extern game_parent* game;