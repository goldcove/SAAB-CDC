/*
 * Virtual C++ Class for RovingNetworks RN-52 Bluetooth modules
 * Copyright (C) 2013  Tim Otto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Created by: Tim Otto
 * Created on: Jun 21, 2013
 * Modified by: Sam Thompson
 * Last modified on: Dec 16, 2016
 */

#include <avr/io.h>
#include "RN52handler.h"

RN52handler BT;

/**
 * Checks for state of event indicator pin (GPIO2). Calls out onGPIO2() from RN52impl that will querry the RN52 module for its status.
 */

void RN52handler::update() {
    driver.update();
}

void RN52handler::bt_play() {
    driver.sendAVCRP(RN52::RN52driver::PLAYPAUSE);
}

void RN52handler::bt_prev() {
    driver.sendAVCRP(RN52::RN52driver::PREV);
}

void RN52handler::bt_next() {
    driver.sendAVCRP(RN52::RN52driver::NEXT);
}

void RN52handler::bt_vassistant() {
    driver.sendAVCRP(RN52::RN52driver::VASSISTANT);
}

void RN52handler::bt_volup() {
    driver.sendAVCRP(RN52::RN52driver::VOLUP);
}

void RN52handler::bt_voldown() {
    driver.sendAVCRP(RN52::RN52driver::VOLDOWN);
}

void RN52handler::bt_visible() {
    driver.visible(true);
}

void RN52handler::bt_invisible() {
    driver.visible(false);
}

void RN52handler::bt_reconnect() {
    driver.reconnectLast();
}

void RN52handler::bt_disconnect() {
    driver.disconnect();
}

void RN52handler::bt_set_maxvol() {
    driver.set_max_volume();
}

void RN52handler::bt_reboot() {
    driver.reboot();
}


/**
 * Debug function used only in 'bench' testing. Listens to input on serial console and calls out corresponding function.
 */

void RN52handler::monitor_serial_input() {
    int incomingByte = 0;
    
    if (Serial.available() > 0) {
        incomingByte = Serial.read();
        switch (incomingByte) {
            case 'V':
                bt_visible();
                Serial.println(F("Going into Discoverable Mode"));
                break;
            case 'I':
                bt_invisible();
                Serial.println(F("Going into non-Discoverable/Connectable Mode"));
                break;
            case 'C':
                bt_reconnect();
                Serial.println(F("Re-connecting to the Last Known Device"));
                break;
            case 'D':
                bt_disconnect();
                Serial.println(F("Disconnecting from the Current Device"));
                break;
            case 'P':
                bt_play();
                Serial.println(F("\"Play/Pause\" Current Track"));
                break;
            case 'N':
                bt_next();
                Serial.println(F("Skip to \"Next\" Track"));
                break;
            case 'R':
                bt_prev();
                Serial.println(F("Go back to \"Previous\" Track"));
                break;
            case 'A':
                bt_vassistant();
                Serial.println(F("Invoking Voice Assistant"));
                break;
            case 'B':
                bt_reboot();
                Serial.println(F("Rebooting the RN52"));
                break;
            case 'd':
            	driver.print_mac();
            	break;
            default:
                Serial.print(F("Invalid command."));
#if (DEBUGMODE==1) // Need the extended watchdog period to show this help.
            case 'H':
                Serial.println(F(" Try one of these instead:"));
                Serial.println(F(""));
                Serial.println(F("V - Go into Discoverable Mode"));
                Serial.println(F("I - Go into non-Discoverable but Connectable Mode"));
                Serial.println(F("C - Reconnect to Last Known Device"));
                Serial.println(F("D - Disconnect from Current Device"));
                Serial.println(F("P - Play/Pause Current Track"));
                Serial.println(F("N - Skip to Next Track"));
                Serial.println(F("R - Previous Track/Beginning of Track"));
                Serial.println(F("A - Invoke Voice Assistant"));
                Serial.println(F("B - Reboot the RN52 module"));
                Serial.println(F("H - Show this list of commands"));
#endif
                Serial.println(F(""));
                break;
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break; // just discard whitespace
        }
    }
}

void RN52handler::initialize() {
    driver.initialize();
}
