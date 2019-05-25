void setup()
{
}

void loop()
{
    static bool onoff = false;
    onoff = !onoff;
    digitalWrite(13,onoff);
    delay(1000);
}
