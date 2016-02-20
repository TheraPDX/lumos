// This #include statement was automatically added by the Particle IDE.
#include "HTU21D/HTU21D.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_TSL2561_U/Adafruit_TSL2561_U.h"

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
UDP Udp;

void configureSensor(void) {
    /* You can also manually set the gain or enable auto-gain support */
    // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
    // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
    tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
    /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
    // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
    //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

}

int sendUdp(double lux) {

    char tempString[128];
    int sent;
    Udp.begin(4444);
    Udp.beginPacket(IPAddress(10, 45, 0, 109), 4444);
    sprintf(tempString, "lux value=%f", lux);
    sent = Udp.write(tempString);
        if (sent < 1) {
            Udp.endPacket();
            Udp.stop();
            return -1;
        }
    Udp.endPacket();
    Udp.stop();
    //Particle.publish("Sent a UDP packet");
    return 0;
}

void setup(void) {
 
    //Particle.publish("Light Sensor Startup...");
  
    /* Initialise the sensor */
    if(!tsl.begin()) {
        Particle.publish("Ooops, no TSL2561 detected. Check  wiring or I2C ADDR!");
        while(1);
    }
  
    /* Setup the sensor gain and integration time */
    configureSensor();

}

void loop(void) {
    
    /* Get a new sensor event */ 
    sensors_event_t event;
    tsl.getEvent(&event);
    char tempLightString[128];
  
    /* Display the results (light is measured in lux) */
    if (event.light) {
        //Particle.publish("Received a light event");
        if (sendUdp(event.light)<0){
           Particle.publish("Failed to send a UDP packet");   
        }
    }
    else {
        /* If event.light = 0 lux the sensor is probably saturated
        and no reliable data could be generated! */
        sendUdp(0);
       // Particle.publish("Sensor overload, sending 0");
    }
    delay(500);
}