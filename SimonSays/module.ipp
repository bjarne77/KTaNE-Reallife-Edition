#include <Wire.h>
#include <Arduino.h>

static Module* Module::self = nullptr;

Module::Module()
{
    Module::self = this;
}

void Module::init(int device_id) {
    Serial.print("Init Device...");
    Serial.print(device_id);
    Serial.println("");
    Wire.begin(device_id);                // join i2c bus with address #device_id
    Wire.onReceive(Module::receiveEventWrapper);
    Wire.onRequest(Module::requestEventWrapper);
}

WireBool Module::has_odd() const {
    return this->serial_number_has_odd;
}

WireBool Module::has_vowels() const {
    return this->serial_number_has_vowels;
}

uint8_t Module::get_strike_count() const {
    return this->strike_count;
}

void Module::update_status(STATUS new_status) {
    Wire.write((uint8_t)new_status);
}

Module::STATUS Module::get_status() const {
    return this->status;
}


void Module::handle_reset() {}
void Module::handle_init() {}
void Module::handle_start() {}

void Module::handle_strike(uint8_t strike_count) {}
void Module::handle_exploded() {}
void Module::handle_finished() {}

void Module::receiveEvent(int howMany) {
  this->lastCommand = CMD::UNDEFINED; // Befehl zurücksetzen

  while (Wire.available()) {
    this->lastCommand = (CMD)Wire.read();
  }

  // Serial.print("Empfangener Befehl: ");
  // Serial.println(lastCommand);
}

static void Module::receiveEventWrapper(int howMany) {
    if (Module::self != nullptr) {
        Module::self->receiveEvent(howMany);
    }
}


void Module::requestEvent() {
    switch(this->lastCommand) {
    case CMD::RESET:
        Serial.println("Handle Reset");
        this->handle_reset();
        if(this->handle_reset_ptr != nullptr) {
            this->handle_reset_ptr();
        }

        this->status = STATUS::NOT_INIT;
        break;

    case CMD::INIT:
        if(status != STATUS::NOT_INIT) {
            Serial.println("Can't init without reset");
            break;
        }

        Serial.println("Handle Init");
        this->handle_init();
        if(this->handle_init_ptr != nullptr) {
            this->handle_init_ptr();
        }

        this->status = STATUS::READY;
        break;

    case CMD::START:
        Serial.println("Handle Start");
        this->handle_start();
        if(this->handle_start_ptr != nullptr) {
            this->handle_start_ptr();
        }

        this->status = STATUS::RUNNING;
        break;

    case CMD::STATUS_UPDATE:
        // Serial.println("Handle Status");
        //Wire.write((uint8_t)this->status);
        if(this->status == STATUS::NEW_STRIKE) {
            this->status = STATUS::RUNNING;
        }
        break;

    case CMD::STRIKE_0:
        Serial.println("Handle Strike 0");
        this->strike_count = 0;
        this->handle_strike(0);
        if(this->handle_strike_ptr != nullptr) {
            this->handle_strike_ptr(0);
        }
        break;

    case CMD::STRIKE_1:
        Serial.println("Handle Strike 1");
        this->strike_count = 1;
        this->handle_strike(1);
        if(this->handle_strike_ptr != nullptr) {
            this->handle_strike_ptr(1);
        }
        break;

    case CMD::STRIKE_2:
        Serial.println("Handle Strike 2");
        this->strike_count = 2;
        this->handle_strike(2);
        if(this->handle_strike_ptr != nullptr) {
            this->handle_strike_ptr(2);
        }
        break;

    case CMD::HAS_VOWELS:
        Serial.println("Handle HAS_VOWELS");
        this->serial_number_has_vowels = WireBool::WIRE_TRUE;
        break;

    case CMD::HAS_ODD:
        Serial.println("Handle HAS_ODD");
        this->serial_number_has_odd = WireBool::WIRE_TRUE;
        break;

    case CMD::EXPLODED:
        Serial.println("Handle EXPLODED");
        this->handle_exploded();
        break;

    case CMD::FINISHED:
        Serial.println("Handle FINISHED");
        this->handle_finished();
        break;

    case CMD::UNDEFINED: // fall through
    default:
        Serial.println("Handle undef cmd");
        break;
  }
}

static void Module::requestEventWrapper() {
    if (Module::self != nullptr) {
        Module::self->requestEvent();
    }
}