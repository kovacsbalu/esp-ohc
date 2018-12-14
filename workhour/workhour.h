class WorkHour {

  public:
    WorkHour(int relay_pin, int reset_pin, int led_pin);
    unsigned long sum_time = 0;
    int saved_relay_status = 0;

    void setup();
    void log_time();
    void check_reset();
    unsigned long calc_work_time();

  private:
    int _relay_pin;
    int _reset_pin;
    int _led_pin;

    unsigned long _session_start_time = millis();
    void _load();
    void _save();
    void _blink(int nn);
};