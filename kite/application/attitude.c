#include <stdio.h>

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bsp.h"
#include "mymath.h"
#include "filter.h"

static int16_t a_offset[3] = { -100, 300, -120 };
static float a[3]; //TODO: not to be static
static float filtered[3];
static int16_t g_offset[3];
//extern xQueueHandle xGetLedQueue(void);
static float q[4] = { 1, 0, 0, 0 };

float exInt = 0, eyInt = 0, ezInt = 0;        // scaled integral error
#define Kp 2.0f         // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.005f       // integral gain governs rate of convergence of gyroscope biases

void vAttitudeTask(void *arg)
{
    static portTickType last = 0;
    xQueueHandle xDataReady;
    sensor_t sensor_event;
    int16_t ax, ay, az;
    int16_t temp;
    int16_t gx, gy, gz;
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float e[3];
    float dq[4];
    float dt = 0.001;
    xDataReady = xGetDataReadyQueue();
    if (xDataReady == 0)
        while(1) {}
    for (;;) {
        if (xQueueReceive(xDataReady, &sensor_event, (portTickType)0)) {
            portTickType now = xTaskGetTickCount();
            //dt = (now == last) ? 0.002 : (now - last) * portTICK_RATE_MS / 1000.0f;
            //last = now;
            dt = now * portTICK_RATE_MS / 100;
            if (sensor_event.type == SENSOR_TYPE_MPU6000) {
                ax = ((sensor_event.data.value[0] << 8) | sensor_event.data.value[1]);
                ay = ((sensor_event.data.value[2] << 8) | sensor_event.data.value[3]);
                az = ((sensor_event.data.value[4] << 8) | sensor_event.data.value[5]);
                temp = ((sensor_event.data.value[6] << 8) | sensor_event.data.value[7]);
                gx = ((sensor_event.data.value[8] << 8) | sensor_event.data.value[9]);
                gy = ((sensor_event.data.value[10] << 8) | sensor_event.data.value[11]);
                gz = ((sensor_event.data.value[12] << 8) | sensor_event.data.value[13]);
                a[X] = ax; 
                a[Y] = ay;
                a[Z] = az;
                low_pass(a, filtered);

                // normalise the measurements
                norm = sqrt(ax*ax + ay*ay + az*az);
                ax = ax / norm;
                ay = ay / norm;
                az = az / norm;

                // estimated direction of gravity
                vx = -(2 * (q[1] * q[3] - q[0] * q[2]));
                vy = -(2 * (q[2] * q[3] + q[0] * q[1]));
                vz = -(q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);

                // error is sum of cross product between reference direction of field and direction measured by sensor
                ex = (ay * vz - az * vy);
                ey = (az * vx - ax * vz);
                ez = (ax * vy - ay * vx);

                // integral error scaled integral gain
                exInt = exInt + ex * Ki;
                eyInt = eyInt + ey * Ki;
                ezInt = ezInt + ez * Ki;

                // adjusted gyroscope measurements
                gx = gx + Kp * ex + exInt;
                gy = gy + Kp * ey + eyInt;
                gz = gz + Kp * ez + ezInt;
#if 0
                ax -= a_offset[0];
                ay -= a_offset[1];
                az -= a_offset[2];
                ax = ax * GRAVITY / 8192.0f;
                ay = ay * GRAVITY / 8192.0f;
                az = az * GRAVITY / 8192.0f;
                temp = (temp / 340.0f) + 36.0f;
                gx = gx / 65.0f;
                gy = gy / 65.0f;
                gz = gz / 65.0f;
                gx = toRadians(gx);
                gy = toRadians(gy);
                gz = toRadians(gz);
                dq[0] = 0.5f * (-q[1] * gx - q[2] * gy - q[3] * gz) * dt;
                dq[1] = 0.5f * ( q[0] * gx - q[3] * gy + q[2] * gz) * dt;
                dq[2] = 0.5f * ( q[3] * gx + q[0] * gy - q[1] * gz) * dt;
                dq[3] = 0.5f * (-q[2] * gx + q[1] * gy + q[0] * gz) * dt;

                q[0] += dq[0];
                q[1] += dq[1];
                q[2] += dq[2];
                q[3] += dq[3];
#if 0
                if (q[0] < 0) {
                    q[0] = -q[0];
                    q[1] = -q[1];
                    q[2] = -q[2];
                    q[3] = -q[3];
                }
#endif
                q_norm(&q[0]);
                qtoe(&e[0], &q[0]);
#endif
                /* Toggle LED */
                led_toggle(LED_BLUE);
                // printf("%d,%d,%d\n", ax, ay, az);
                // printf("%d,%d,%d\n", gx, gy, gz);
                // printf("%f,%f,%f\n", filtered.[X], filtered.[Y], filtered.[Z]);
                vTaskDelay(50 / portTICK_RATE_MS);

            }
        }
    }
}

void vAttitudeTask_init(void)
{
    xTaskHandle xHandle = NULL;
    xTaskCreate(vAttitudeTask, (const signed portCHAR *)"attitude", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle);
    configASSERT(xHandle);
}
