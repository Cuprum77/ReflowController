#pragma once

typedef struct
{
    // PID Variables
    float Kp;
    float Ki;
    float Kd;

    // Derivative Low-Pass Filter
    float tau;

    // PID Limits
    float min;
    float max;

    // Sample time
    float sampleTime;

    // PID memory
    float lastError;
    float integrator;
    float lastMeasurement;
    float differentiator;

    // PID output
    float output;
} PID_Controller;

class PID
{
public:
    PID();
    PID(float Kp, float Ki, float Kd);
    PID(float Kp, float Ki, float Kd, float tau, float min, float max, float sampleTime);

    void update(float setPoint, float measurement);
    void reset();

    void setKp(float Kp);
    void setKi(float Ki);
    void setKd(float Kd);
    void setTau(float tau);
    void setMin(float min);
    void setMax(float max);
    void setSampleTime(float sampleTime);

    bool getOutput();
    float output();
private:
    PID_Controller controller;
};