
#include <stdint.h>

enum DEVICE {
  DEVICE_1 = 1,
  SIMONSAY,
  MORSE,
  DEVICE_N
};


enum WireBool {
    WIRE_FALSE = 0,
    WIRE_TRUE = 1,
    UNDEF = 255,
};


class Module {
public:
    /**
     * Status definitions
     */
    enum class STATUS {
        NOT_INIT = 100,
        READY = 101,
        RUNNING = 102,
        NEW_STRIKE = 110,
        FAILED = 200,
        SUCCESS = 201,
        FINISHED = 202
    };

    enum CMD {
        RESET,
        INIT,
        START,
        STATUS_UPDATE,
        STRIKE_0,
        STRIKE_1,
        STRIKE_2,
        HAS_VOWELS,
        HAS_ODD,
        EXPLODED,
        FINISHED,
        UNDEFINED
    };

public:
    Module();

    void init(int device_id);

    void (*handle_reset_ptr)() = nullptr;
    void (*handle_init_ptr)() = nullptr;
    void (*handle_start_ptr)() = nullptr;

    void (*handle_strike_ptr)(uint8_t) = nullptr;
    void (*handle_exploded_ptr)() = nullptr;
    void (*handle_finished_ptr)() = nullptr;

    WireBool has_odd() const;
    WireBool has_vowels() const;

    uint8_t get_strike_count() const;

    void update_status(STATUS new_status);
    STATUS get_status() const;

protected:

    // virtual functions to be implemented by derived classes
    virtual void handle_reset();
    virtual void handle_init();
    virtual void handle_start();
    
    virtual void handle_strike(uint8_t strike_count);
    virtual void handle_exploded();
    virtual void handle_finished();

private:

    void receiveEvent(int howMany);
    static void receiveEventWrapper(int howMany);
    void requestEvent();
    static void requestEventWrapper();


private:
    static Module* self; // uses of static function warpper
    volatile STATUS status = STATUS::NOT_INIT;
    CMD lastCommand = CMD::UNDEFINED;

    WireBool serial_number_has_vowels = WireBool::UNDEF;
    WireBool serial_number_has_odd = WireBool::UNDEF;

    uint8_t strike_count = 0;
};

#include "module.ipp"